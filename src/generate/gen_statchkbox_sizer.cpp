/////////////////////////////////////////////////////////////////////////////
// Purpose:   wxStaticBoxSizer with wxCheckBox generator
// Author:    Ralph Walden
// Copyright: Copyright (c) 2020-2022 KeyWorks Software (Ralph Walden)
// License:   Apache License -- see ../../LICENSE
/////////////////////////////////////////////////////////////////////////////

#include <wx/checkbox.h>
#include <wx/sizer.h>
#include <wx/statbox.h>

#include "gen_common.h"  // GeneratorLibrary -- Generator classes
#include "node.h"        // Node class

#include "pugixml.hpp"  // xml read/write/create/process

#include "gen_statchkbox_sizer.h"

wxObject* StaticCheckboxBoxSizerGenerator::CreateMockup(Node* node, wxObject* parent)
{
    long style_value = 0;
    if (node->prop_as_string(prop_style).contains("wxALIGN_RIGHT"))
        style_value |= wxALIGN_RIGHT;

    m_checkbox = new wxCheckBox(wxStaticCast(parent, wxWindow), wxID_ANY, node->prop_as_wxString(prop_label),
                                wxDefaultPosition, wxDefaultSize, style_value);
    if (node->prop_as_bool(prop_checked))
        m_checkbox->SetValue(true);

    auto staticbox = new wxStaticBox(wxStaticCast(parent, wxWindow), wxID_ANY, m_checkbox);

    auto sizer = new wxStaticBoxSizer(staticbox, node->prop_as_int(prop_orientation));
    if (auto dlg = wxDynamicCast(parent, wxDialog); dlg)
    {
        if (!dlg->GetSizer())
            dlg->SetSizer(sizer);
    }

    auto min_size = node->prop_as_wxSize(prop_minimum_size);
    if (min_size.x != -1 || min_size.y != -1)
        sizer->SetMinSize(min_size);

    if (node->HasValue(prop_tooltip))
        m_checkbox->SetToolTip(node->prop_as_wxString(prop_tooltip));

    return sizer;
}

void StaticCheckboxBoxSizerGenerator::AfterCreation(wxObject* wxobject, wxWindow* /*wxparent*/, Node* node,
                                                    bool /* is_preview */)
{
    if (node->as_bool(prop_hidden))
    {
        if (auto sizer = wxStaticCast(wxobject, wxSizer); sizer)
            sizer->ShowItems(false);
    }
}

bool StaticCheckboxBoxSizerGenerator::OnPropertyChange(wxObject* /* widget */, Node* node, NodeProperty* prop)
{
    if (prop->isProp(prop_tooltip))
    {
        m_checkbox->SetToolTip(node->prop_as_wxString(prop_tooltip));
    }

    return false;
}

std::optional<ttlib::cstr> StaticCheckboxBoxSizerGenerator::GenConstruction(Node* node)
{
    ttlib::cstr code;
    code << node->prop_as_string(prop_checkbox_var_name) << " = new wxCheckBox(";
    code << GetParentName(node) << ", " << node->prop_as_string(prop_id) << ", ";

    if (node->prop_as_string(prop_label).size())
    {
        code << GenerateQuotedString(node->prop_as_string(prop_label));
    }
    else
    {
        code << "wxEmptyString";
    }

    code << ");\n";

    if (auto result = GenValidatorSettings(node); result)
    {
        code << result.value() << '\n';
    }

    if (node->IsLocal())
        code << "auto* ";

    ttlib::cstr parent_name("this");
    if (!node->GetParent()->IsForm())
    {
        auto parent = node->GetParent();
        while (parent)
        {
            if (parent->IsContainer())
            {
                parent_name = parent->get_node_name();
                break;
            }
            else if (parent->isGen(gen_wxStaticBoxSizer) || parent->isGen(gen_StaticCheckboxBoxSizer) ||
                     parent->isGen(gen_StaticRadioBtnBoxSizer))
            {
                parent_name.clear();
                parent_name << parent->get_node_name() << "->GetStaticBox()";
                break;
            }
            parent = parent->GetParent();
        }
    }

    code << node->get_node_name() << " = new wxStaticBoxSizer(new wxStaticBox(" << parent_name << ", wxID_ANY,";
    if (wxGetProject().value(prop_wxWidgets_version) == "3.1")
    {
        code << "\n#if wxCHECK_VERSION(3, 1, 1)\n\t";
        code << node->prop_as_string(prop_checkbox_var_name) << "),";
        code << "\n#else\n\t";
        code << "wxEmptyString),";
        code << "\n#endif\n";
        code << node->prop_as_string(prop_orientation) << ");";
    }
    else
    {
        code << node->prop_as_string(prop_checkbox_var_name) << "), " << node->prop_as_string(prop_orientation) << ");";
    }

    auto min_size = node->prop_as_wxSize(prop_minimum_size);
    if (min_size.GetX() != -1 || min_size.GetY() != -1)
    {
        code << "\n\t" << node->get_node_name() << "->SetMinSize(" << min_size.GetX() << ", " << min_size.GetY() << ");";
    }

    return code;
}

std::optional<ttlib::cstr> StaticCheckboxBoxSizerGenerator::GenSettings(Node* node, size_t& /* auto_indent */)
{
    ttlib::cstr code;
    if (node->prop_as_bool(prop_disabled))
    {
        code << node->get_node_name() << "->GetStaticBox()->Enable(false);";
    }

    if (node->HasValue(prop_tooltip))
    {
        if (code.size())
            code << "\n\t";
        code << node->prop_as_string(prop_checkbox_var_name) << "->SetToolTip("
             << GenerateQuotedString(node->prop_as_string(prop_tooltip)) << ");";
    }

    if (code.size())
        return code;
    else
        return {};
}

std::optional<ttlib::cstr> StaticCheckboxBoxSizerGenerator::GenAfterChildren(Node* node)
{
    ttlib::cstr code;
    if (node->as_bool(prop_hidden))
    {
        code << "\t" << node->get_node_name() << "->ShowItems(false);";
    }

    auto parent = node->GetParent();
    if (!parent->IsSizer() && !parent->isGen(gen_wxDialog) && !parent->isGen(gen_PanelForm))
    {
        if (code.size())
            code << '\n';
        code << "\n\t";

        // The parent node is not a sizer -- which is expected if this is the parent sizer underneath a form or
        // wxPanel.

        if (parent->isGen(gen_wxRibbonPanel))
        {
            code << parent->get_node_name() << "->SetSizerAndFit(" << node->get_node_name() << ");";
        }
        else
        {
            if (GetParentName(node) != "this")
                code << GetParentName(node) << "->";
            code << "SetSizerAndFit(" << node->get_node_name() << ");";
        }
    }

    if (code.size())
        return code;
    else
        return {};
}

bool StaticCheckboxBoxSizerGenerator::GetIncludes(Node* node, std::set<std::string>& set_src, std::set<std::string>& set_hdr)
{
    InsertGeneratorInclude(node, "#include <wx/sizer.h>", set_src, set_hdr);
    InsertGeneratorInclude(node, "#include <wx/statbox.h>", set_src, set_hdr);

    // The checkbox is always a class member, so we need to force it to be added to the header set
    set_hdr.insert("#include <wx/checkbox.h>");
    return true;
}

// ../../wxSnapShot/src/xrc/xh_sizer.cpp
// ../../../wxWidgets/src/xrc/xh_sizer.cpp
// See Handle_wxStaticBoxSizer()

int StaticCheckboxBoxSizerGenerator::GenXrcObject(Node* node, pugi::xml_node& object, size_t /* xrc_flags */)
{
    pugi::xml_node item;
    auto result = BaseGenerator::xrc_sizer_item_created;

    if (node->GetParent()->IsSizer())
    {
        GenXrcSizerItem(node, object);
        item = object.append_child("object");
    }
    else
    {
        item = object;
        result = BaseGenerator::xrc_updated;
    }

    item.append_attribute("class").set_value("wxStaticBoxSizer");
    item.append_attribute("name").set_value(node->prop_as_string(prop_var_name));
    item.append_child("orient").text().set(node->prop_as_string(prop_orientation));
    if (node->HasValue(prop_minimum_size))
    {
        item.append_child("minsize").text().set(node->prop_as_string(prop_minimum_size));
    }
    ADD_ITEM_BOOL(prop_hidden, "hideitems");

    auto checkbox = item.append_child("windowlabel");
    auto child = checkbox.append_child("object");
    child.append_attribute("class").set_value("wxCheckBox");
    child.append_child("label").text().set(node->prop_as_string(prop_label));
    if (node->prop_as_bool(prop_checked))
        child.append_child("checked").text().set("1");

    return result;
}

void StaticCheckboxBoxSizerGenerator::RequiredHandlers(Node* /* node */, std::set<std::string>& handlers)
{
    handlers.emplace("wxSizerXmlHandler");
}
