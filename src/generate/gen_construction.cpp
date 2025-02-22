/////////////////////////////////////////////////////////////////////////////
// Purpose:   Top level Object construction code
// Author:    Ralph Walden
// Copyright: Copyright (c) 2020-2022 KeyWorks Software (Ralph Walden)
// License:   Apache License -- see ../../LICENSE
/////////////////////////////////////////////////////////////////////////////

#include "code.h"             // Code -- Helper class for generating code
#include "gen_base.h"         // BaseCodeGenerator -- Generate Src and Hdr files for Base Class
#include "gen_common.h"       // Common component functions
#include "gen_lang_common.h"  // Common mulit-language functions
#include "node.h"             // Node class
#include "node_decl.h"        // NodeDeclaration class
#include "write_code.h"       // Write code to Scintilla or file

// clang-format off

// These are the types that need to have generator->GenAdditionalCode() called after the type is constructed
GenType aftercode_types[] = {
    type_menubar,
    type_menu,
    type_submenu,
    type_tool,
    type_listbook,
    type_simplebook,
    type_notebook,
    type_auinotebook,
    type_treelistctrl,
    gen_type_unknown
};
// clang-format on

void BaseCodeGenerator::GenConstruction(Node* node)
{
    auto type = node->gen_type();
    auto declaration = node->GetNodeDeclaration();
    auto generator = declaration->GetGenerator();
    if (!generator)
        return;

    if (node->HasValue(prop_platforms) && node->value(prop_platforms) != "Windows|Unix|Mac")
    {
        BeginPlatformCode(node);
    }

    bool need_closing_brace = false;
    Code gen_code(node, m_language);

    // Always start with a tab in case there are line breaks. The leading tab will be ignored
    // if the line is not broken.
    gen_code.Add((m_language == GEN_LANG_CPLUSPLUS) ? "\t" : "\t\t");
    gen_code.SetBreakAt(80);
    std::optional<ttlib::sview> scode;
    std::optional<ttlib::cstr> result;

    scode = generator->CommonConstruction(gen_code);
    if (!scode && is_cpp() && is_cpp())
    {
        // Python form constructor will have already been called, so we should only get here
        // if there is no CommonConstruction that supports Python.
        result = generator->GenConstruction(node);
        if (result)
            scode = *result;
    }

    if (scode)
    {
        // Don't add blank lines when adding tools to a toolbar
        if (type != type_aui_tool && type != type_tool)
        {
            m_source->writeLine();
        }

        // Check for any indentation via a brace or line break with multiple tabs, and if so,
        // don't remove the whitespace
        m_source->writeLine(*scode, (ttlib::is_found(scode->find('{')) || ttlib::is_found(scode->find("\n\t\t"))) ?
                                        indent::none :
                                        indent::auto_no_whitespace);
        if (scode->starts_with("\t{"))
        {
            need_closing_brace = true;
        }
    }
    GenSettings(node);

    if (type == type_ribbontoolbar)
    {
        BeginBrace();
        // A wxRibbonToolBar can only have abstract children that consist of the tools.
        for (const auto& child: node->GetChildNodePtrs())
        {
            gen_code.clear();
            scode = child->GetGenerator()->CommonConstruction(gen_code);
            if (!scode && is_cpp())
            {
                result = child->GetGenerator()->GenConstruction(child.get());
                if (result)
                    scode = *result;
            }
            if (scode)
                m_source->writeLine(*scode);
        }
        EndBrace();
        gen_code.clear();
        gen_code.NodeName().Function("Realize()").EndFunction();
        m_source->writeLine(gen_code.m_code);
        return;
    }
    else if (type == type_tool_dropdown && node->GetChildCount())
    {
        BeginBrace();
        m_source->writeLine("wxMenu* menu = new wxMenu;");
        auto menu_node_ptr = g_NodeCreator.NewNode(gen_wxMenu);
        menu_node_ptr->prop_set_value(prop_var_name, "menu");
        for (const auto& child: node->GetChildNodePtrs())
        {
            auto old_parent = child->GetParent();
            child->SetParent(menu_node_ptr.get());
            if (auto gen = child->GetNodeDeclaration()->GetGenerator(); gen)
            {
                gen_code.clear();
                scode = gen->CommonConstruction(gen_code);
                if (!scode && is_cpp())
                {
                    result = gen->GenConstruction(child.get());
                    if (result)
                        scode = *result;
                }
                if (scode)
                    m_source->writeLine(*scode);
            }
            GenSettings(child.get());
            // A submenu can have children
            if (child->GetChildCount())
            {
                for (const auto& grandchild: child->GetChildNodePtrs())
                {
                    if (auto gen = grandchild->GetNodeDeclaration()->GetGenerator(); gen)
                    {
                        gen_code.clear();
                        scode = gen->CommonConstruction(gen_code);
                        if (!scode && is_cpp())
                        {
                            result = gen->GenConstruction(grandchild.get());
                            if (result)
                                scode = *result;
                        }
                        if (scode)
                            m_source->writeLine(*scode);
                    }
                    GenSettings(grandchild.get());
                    // A submenu menu item can also be a submenu with great grandchildren.
                    if (grandchild->GetChildCount())
                    {
                        for (const auto& great_grandchild: grandchild->GetChildNodePtrs())
                        {
                            if (auto gen = great_grandchild->GetNodeDeclaration()->GetGenerator(); gen)
                            {
                                gen_code.clear();
                                result = gen->CommonConstruction(gen_code);
                                if (!result && is_cpp())
                                    result = gen->GenConstruction(great_grandchild.get());
                                if (result)
                                    m_source->writeLine(result.value());
                            }
                            GenSettings(great_grandchild.get());
                            // It's possible to have even more levels of submenus, but we'll stop here.
                        }
                    }
                }
            }
            child->SetParent(old_parent);
        }
        m_source->writeLine(ttlib::cstr() << node->get_node_name() << LangPtr() << "SetDropdownMenu(menu);");
        EndBrace();
        return;
    }

    auto parent = node->GetParent();

    if (GenAfterChildren(node, need_closing_brace))
    {
        return;
    }

    if (parent->IsSizer())
    {
        GenParentSizer(node, need_closing_brace);
    }
    else if (parent->IsToolBar() && !node->isType(type_tool) && !node->isType(type_tool_separator))
    {
        ttlib::cstr code;
        if (parent->isType(type_toolbar_form))
            code << "AddControl(" << node->prop_as_string(prop_var_name) << ");";
        else
            code << parent->prop_as_string(prop_var_name) << LangPtr() << "AddControl("
                 << node->prop_as_string(prop_var_name) << ");";
        m_source->writeLine(code);
    }
    else if (node->gen_type() == type_widget && parent->isGen(gen_wxChoicebook))
    {
        ttlib::cstr code;
        code << parent->get_node_name() << LangPtr() << "GetControlSizer()" << LangPtr() << "Add(" << node->get_node_name();
        code << ", " << GetWidgetName(m_language, "wxSizerFlags") << "().Expand().Border("
             << GetWidgetName(m_language, "wxALL") << "))";
        if (m_language == GEN_LANG_CPLUSPLUS)
            code << ';';
        m_source->writeLine(code);
    }

    if (node->isGen(gen_PageCtrl) && node->GetChildCount())
    {
        // type_page will have already constructed the code for the child. However, we still need to generate
        // settings and process any grandchildren.

        auto page_child = node->GetChild(0);
        if (page_child)
        {
            GenSettings(page_child);

            for (const auto& child: page_child->GetChildNodePtrs())
            {
                GenConstruction(child.get());
            }
        }
    }
    else
    {
        for (const auto& child: node->GetChildNodePtrs())
        {
            GenConstruction(child.get());
        }
    }

    if (node->IsSizer())
    {
        if (!parent->IsSizer() && !parent->isGen(gen_wxDialog) && !parent->isGen(gen_PanelForm))
        {
            // The parent node is not a sizer -- which is expected if this is the parent sizer underneath a form or
            // wxPanel.

            ttlib::cstr code;
            if (parent->isGen(gen_wxRibbonPanel))
            {
                code << parent->get_node_name() << LangPtr() << "SetSizerAndFit(" << node->get_node_name() << ");";
            }
            else
            {
                if (GetParentName(node) != "this")
                    code << GetParentName(node) << LangPtr();
                code << "SetSizerAndFit(" << node->get_node_name() << ");";
            }

            m_source->writeLine();
            m_source->writeLine(code);
        }
    }
    else if (type == type_splitter)
    {
        ttlib::cstr code(node->get_node_name());

        if (node->GetChildCount() == 1)
        {
            code << LangPtr() << "Initialize(" << node->GetChild(0)->get_node_name() << ");";
            m_source->writeLine(code);
        }
        else if (node->GetChildCount() > 1)
        {
            if (node->prop_as_string(prop_splitmode) == "wxSPLIT_VERTICAL")
                code << LangPtr() << "SplitVertically(";
            else
                code << LangPtr() << "SplitHorizontally(";

            code << node->GetChild(0)->get_node_name() << ", " << node->GetChild(1)->get_node_name() << ");";
            m_source->writeLine(code);

            if (auto sash_pos = node->get_prop_ptr(prop_sashpos)->as_int(); sash_pos != 0 && sash_pos != -1)
            {
                code = node->get_node_name();
                code << LangPtr() << "SetSashPosition(" << node->prop_as_string(prop_sashpos) << ");";
                m_source->writeLine(code);
            }
        }
    }

    else
    {
        for (size_t idx = 0; aftercode_types[idx] != gen_type_unknown; ++idx)
        {
            if (type == aftercode_types[idx])
            {
                result = generator->GenAdditionalCode(code_after_children, node, m_language);
                if (!result)
                {
                    if (m_language == GEN_LANG_CPLUSPLUS)
                        result = generator->GenAdditionalCode(code_after_children, node);
                    else
                        result = generator->GenAdditionalCode(code_after_children, node, m_language);
                }

                if (result && result.value().size())
                {
                    m_source->writeLine(result.value(), indent::none);
                }
                m_source->writeLine();
                break;
            }
        }
    }

    // A wxRibbonBar needs to be realized after all children have been created

    if (node->isGen(gen_wxRibbonBar))
    {
        m_source->writeLine(ttlib::cstr() << node->get_node_name() << LangPtr() << "Realize();");
    }

    if (node->HasValue(prop_platforms) && node->value(prop_platforms) != "Windows|Unix|Mac")
    {
        EndPlatformCode();
    }
}

const char* BaseCodeGenerator::LangPtr() const
{
    switch (m_language)
    {
        case GEN_LANG_CPLUSPLUS:
            return "->";

        case GEN_LANG_PYTHON:
            return ".";

        default:
            FAIL_MSG("Unsupported language!")
            return "";
    }
}

void BaseCodeGenerator::BeginPlatformCode(Node* node)
{
    ttlib::cstr code;
    if (node->value(prop_platforms).contains("Windows"))
    {
        switch (m_language)
        {
            case GEN_LANG_CPLUSPLUS:
                code << "\n#if defined(__WINDOWS__)";
                break;

            case GEN_LANG_PYTHON:
                code << "\nif defined(__WINDOWS__)";
                break;
        }
    }
    if (node->value(prop_platforms).contains("Unix"))
    {
        switch (m_language)
        {
            case GEN_LANG_CPLUSPLUS:
                if (code.size())
                    code << " || ";
                else
                    code << "\n#if ";
                code << "defined(__UNIX__)";
                break;

            case GEN_LANG_PYTHON:
                if (code.size())
                    code << " || ";
                else
                    code << "\nif ";
                code << "defined(__UNIX__)";
                break;
        }
    }
    if (node->value(prop_platforms).contains("Mac"))
    {
        switch (m_language)
        {
            case GEN_LANG_CPLUSPLUS:
                if (code.size())
                    code << " || ";
                else
                    code << "\n#if ";
                code << "defined(__WXOSX__)";
                break;

            case GEN_LANG_PYTHON:
                if (code.size())
                    code << " || ";
                else
                    code << "\nif ";
                code << "defined(__WXOSX__)";
                break;
        }
    }

    m_source->writeLine(code);
    m_source->SetLastLineBlank();
    if (m_language == GEN_LANG_PYTHON)
        m_source->Indent();
}

void BaseCodeGenerator::EndPlatformCode()
{
    switch (m_language)
    {
        case GEN_LANG_CPLUSPLUS:
            m_source->writeLine("#endif  // limited to specific platforms");
            break;

        case GEN_LANG_PYTHON:
            m_source->Unindent();
            break;
    }
}

void BaseCodeGenerator::BeginBrace()
{
    if (m_language == GEN_LANG_CPLUSPLUS)
        m_source->writeLine("{");
    m_source->Indent();
}

void BaseCodeGenerator::EndBrace()
{
    m_source->Unindent();
    if (m_language == GEN_LANG_CPLUSPLUS)
        m_source->writeLine("}");
}

void BaseCodeGenerator::GenSettings(Node* node)
{
    size_t auto_indent = indent::auto_no_whitespace;
    auto generator = node->GetGenerator();
    std::optional<ttlib::cstr> result;
    Code gen_code(node, m_language);
    result = generator->CommonSettings(gen_code, auto_indent);
    if (!result)
    {
        if (m_language == GEN_LANG_CPLUSPLUS)
            result = generator->GenSettings(node, auto_indent);
        else
            result = generator->GenSettings(node, auto_indent, m_language);
    }

    if (result && result.value().size())
    {
        m_source->writeLine(result.value(), auto_indent);
    }

    if (node->get_prop_ptr(prop_window_extra_style))
    {
        ttlib::cstr code;
        if (m_language == GEN_LANG_CPLUSPLUS)
        {
            if (result = GenValidatorSettings(node); result)
            {
                m_source->writeLine(result.value());
            }
        }

        if (m_language == GEN_LANG_CPLUSPLUS)
            GenerateWindowSettings(node, code);
        else
            GenerateWindowSettings(m_language, node, code);

        if (code.size())
            m_source->writeLine(code, indent::auto_keep_whitespace);
    }
}

bool BaseCodeGenerator::GenAfterChildren(Node* node, bool need_closing_brace)
{
    auto generator = node->GetGenerator();
    Code gen_code(node, m_language);
    auto result = generator->CommonAfterChildren(gen_code);
    if (!result)
    {
        if (m_language == GEN_LANG_CPLUSPLUS)
            result = generator->GenAfterChildren(node);
        else if (m_language == GEN_LANG_PYTHON)
            result = generator->GenPythonAfterChildren(node);
    }

    if (result)
    {
        // If the node needs to write code after all children are constructed, then create the children first, then write
        // the post-child code. Note that in this case, no further handling of the node is done, so GenAfterChildren() is
        // required to handle all post-child construction code.

        for (const auto& child: node->GetChildNodePtrs())
        {
            GenConstruction(child.get());
        }

        m_source->writeLine(result.value(), indent::none);
        auto parent = node->GetParent();

        // Code for spacer's is handled by the component's GenConstruction() call
        if (parent->IsSizer() && !node->isGen(gen_spacer))
        {
            gen_code.clear();

            if (need_closing_brace)
                gen_code.Tab();
            gen_code.Tab().ParentName().Function("Add(").NodeName().Comma();

            if (parent->isGen(gen_wxGridBagSizer))
            {
                gen_code.Add("wxGBPosition(").as_string(prop_row).Comma().as_string(prop_column) << "), ";
                gen_code.Add("wxGBSpan(").as_string(prop_rowspan).Comma().as_string(prop_colspan) << "), ";

                if (node->HasValue(prop_borders))
                    gen_code.as_string(prop_borders);
                if (node->prop_as_string(prop_flags).size())
                {
                    if (node->HasValue(prop_borders))
                        gen_code.m_code += '|';
                    gen_code.as_string(prop_flags);
                }

                if (!node->HasValue(prop_borders) && !node->HasValue(prop_flags))
                    gen_code.m_code += '0';

                gen_code.as_string(prop_border_size).EndFunction();
                gen_code.m_code.Replace(", 0, 0)", ")");
            }
            else
            {
                gen_code.GenSizerFlags().EndFunction();
            }

            if (need_closing_brace)
            {
                m_source->writeLine(gen_code.m_code, indent::auto_keep_whitespace);
                if (m_language == GEN_LANG_CPLUSPLUS)
                {
                    m_source->writeLine("\t}");
                }
            }
            else
            {
                m_source->writeLine(gen_code.m_code);
            }
        }

        return true;
    }
    else
    {
        return false;
    }
}

void BaseCodeGenerator::GenParentSizer(Node* node, bool need_closing_brace)
{
    auto declaration = node->GetNodeDeclaration();
    auto generator = declaration->GetGenerator();

    auto result = generator->GenAdditionalCode(code_after_children, node, m_language);
    if (!result)
    {
        if (m_language == GEN_LANG_CPLUSPLUS)
            result = generator->GenAdditionalCode(code_after_children, node);
        else
            result = generator->GenAdditionalCode(code_after_children, node, m_language);
    }

    ttlib::cstr code;
    if (result && result.value().size())
    {
        m_source->writeLine(result.value(), indent::none);
    }

    // Code for spacer's is handled by the component's GenConstruction() call
    if (!node->isGen(gen_spacer))
    {
        if (node->isGen(gen_wxStdDialogButtonSizer))
        {
            if (node->get_form()->isGen(gen_wxDialog) && node->prop_as_bool(prop_static_line))
                code << node->GetParent()->get_node_name() << LangPtr() << "Add(CreateSeparatedSizer("
                     << node->get_node_name() << "), ";
            else
                code << node->GetParent()->get_node_name() << LangPtr() << "Add(" << node->get_node_name() << ", ";
        }
        else
        {
            if (need_closing_brace)
            {
                code << "\t";
            }
            code << node->GetParent()->get_node_name() << LangPtr() << "Add(" << node->get_node_name() << ", ";
        }

        if (node->GetParent()->isGen(gen_wxGridBagSizer))
        {
            code << "wxGBPosition(" << node->prop_as_string(prop_row) << ", " << node->prop_as_string(prop_column) << "), ";
            code << "wxGBSpan(" << node->prop_as_string(prop_rowspan) << ", " << node->prop_as_string(prop_colspan) << "), ";
            ttlib::cstr flags(node->prop_as_string(prop_borders));
            if (node->prop_as_string(prop_flags).size())
            {
                if (flags.size())
                    flags << '|';
                flags << node->prop_as_string(prop_flags);
            }

            if (flags.empty())
                flags << '0';

            code << flags << ", " << node->prop_as_string(prop_border_size) << ");";
            code.Replace(", 0, 0);", ");");
        }
        else
        {
            code << GenerateSizerFlags(m_language, node) << ")";
            if (m_language == GEN_LANG_CPLUSPLUS)
                code << ';';
        }
    }

    if (need_closing_brace)
    {
        m_source->writeLine(code, indent::auto_keep_whitespace);
        if (m_language == GEN_LANG_CPLUSPLUS)
        {
            m_source->writeLine("\t}");
        }
    }
    else
    {
        m_source->writeLine(code);
    }
}
