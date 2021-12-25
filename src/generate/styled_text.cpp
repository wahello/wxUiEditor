/////////////////////////////////////////////////////////////////////////////
// Purpose:   wxStyledText (scintilla) generate
// Author:    Ralph Walden
// Copyright: Copyright (c) 2020-2021 KeyWorks Software (Ralph Walden)
// License:   Apache License -- see ../../LICENSE
/////////////////////////////////////////////////////////////////////////////

// See https://docs.wxwidgets.org/trunk/classwx_styled_text_ctrl.html for wxWidgets documentation
// See https://www.scintilla.org/ScintillaDoc.html for Scintilla documentation

#include <wx/stc/stc.h>  // A wxWidgets implementation of Scintilla.

#include "ttmultistr.h"  // multistr -- Breaks a single string into multiple strings

#include "styled_text.h"

#include "gen_common.h"    // GeneratorLibrary -- Generator classes
#include "node.h"          // Node class
#include "node_creator.h"  // NodeCreator -- Class used to create nodes
#include "utils.h"         // Utility functions that work with properties

// Call g_NodeCreator->GetConstantAsInt("wx_define") to get the #defined integer value -- see node_creator.h

std::map<std::string, const char*> g_stc_flags = {

    { "no wrapping", "wxSTC_WRAP_NONE" },
    { "word", "wxSTC_WRAP_WORD" },
    { "character", "wxSTC_WRAP_CHAR" },
    { "whitespace", "wxSTC_WRAP_WHITESPACE" },

    // Visual flags
    { "end", "wxSTC_WRAPVISUALFLAG_END" },
    { "start", "wxSTC_WRAPVISUALFLAG_START" },
    { "margin", "wxSTC_WRAPVISUALFLAG_MARGIN" },

    { "end_text", "wxSTC_WRAPVISUALFLAGLOC_END_BY_TEXT" },
    { "start_text", "wxSTC_WRAPVISUALFLAGLOC_START_BY_TEXT" },

    { "fixed", "wxSTC_WRAPINDENT_FIXED" },
    { "same", "wxSTC_WRAPINDENT_SAME" },
    { "indent", "wxSTC_WRAPINDENT_INDENT" },

    { "symbol", "wxSTC_MARGIN_SYMBOL" },
    { "number", "wxSTC_MARGIN_NUMBER" },
    { "background", "wxSTC_MARGIN_BACK" },
    { "foreground", "wxSTC_MARGIN_FORE" },
    { "text", "wxSTC_MARGIN_TEXT" },
    { "right-aligned text", "wxSTC_MARGIN_RTEXT" },
    { "colour", "wxSTC_MARGIN_COLOUR" },

    // { "deep_indent", "SC_WRAPINDENT_DEEPINDENT" }, // not supported in 3.1.15
};

wxObject* StyledTextGenerator::CreateMockup(Node* node, wxObject* parent)
{
    auto scintilla =
        new wxStyledTextCtrl(wxStaticCast(parent, wxWindow), wxID_ANY, DlgPoint(parent, node, prop_pos),
                             DlgSize(parent, node, prop_size), GetStyleInt(node), node->prop_as_wxString(prop_var_name));

    if (node->HasValue(prop_hint))
    {
        scintilla->SetHint(node->prop_as_wxString(prop_hint));
    }
    else
    {
        scintilla->SetHint("To turn off line numbers, expand Margins and set margin_0_width to zero");
    }

    // Wrap category settings

    if (!node->prop_as_string(prop_stc_wrap_mode).is_sameas("no wrapping"))
        scintilla->SetWrapMode(g_NodeCreator.GetConstantAsInt(g_stc_flags.at(node->prop_as_string(prop_stc_wrap_mode))));
    if (node->HasValue(prop_stc_wrap_visual_flag))
    {
        ttlib::multistr flags(node->prop_as_string(prop_stc_wrap_visual_flag), '|');
        int value = 0;
        for (auto& iter: flags)
        {
            value |= g_NodeCreator.GetConstantAsInt(g_stc_flags.at(iter));
        }
        if (value)
        {
            scintilla->SetWrapVisualFlags(value);
        }
    }
    if (node->HasValue(prop_stc_wrap_visual_location))
    {
        ttlib::multistr flags(node->prop_as_string(prop_stc_wrap_visual_location), '|');
        int value = 0;
        for (auto& iter: flags)
        {
            value |= g_NodeCreator.GetConstantAsInt(g_stc_flags.at(iter));
        }
        if (value)
        {
            scintilla->SetWrapVisualFlagsLocation(value);
        }
    }
    if (!node->prop_as_string(prop_stc_wrap_indent_mode).is_sameas("fixed"))
    {
        scintilla->SetWrapIndentMode(
            g_NodeCreator.GetConstantAsInt(g_stc_flags.at(node->prop_as_string(prop_stc_wrap_indent_mode))));
    }
    if (node->HasValue(prop_stc_wrap_start_indent))
    {
        scintilla->SetWrapStartIndent(node->prop_as_int(prop_stc_wrap_start_indent));
    }

    // Margin category settings

    if (node->prop_as_int(prop_stc_left_margin_width) != 1)
    {
        scintilla->SetMarginLeft(node->prop_as_int(prop_stc_left_margin_width));
    }
    if (node->prop_as_int(prop_stc_right_margin_width) != 1)
    {
        scintilla->SetMarginRight(node->prop_as_int(prop_stc_right_margin_width));
    }
    if (!node->prop_as_bool(prop_stc_select_wrapped_line))
    {
        scintilla->SetMarginOptions(wxSTC_MARGINOPTION_SUBLINESELECT);
    }

    if (node->prop_as_int(prop_stc_margin_0_width) != 0)
    {
        int width = node->prop_as_int(prop_stc_margin_0_width);
        if (width < 0)
        {
            wxString numbers("_");
            while (width < 0)
            {
                numbers << '9';
                ++width;
            }
            width = scintilla->TextWidth(wxSTC_STYLE_LINENUMBER, numbers);
        }
        scintilla->SetMarginWidth(0, width);

        if (auto result = g_stc_flags.find(node->prop_as_string(prop_stc_margin_0_type)); result != g_stc_flags.end())
            scintilla->SetMarginType(0, g_NodeCreator.GetConstantAsInt(result->second));

        if (node->prop_as_string(prop_stc_margin_0_type) == "colour" && node->HasValue(prop_stc_margin_0_colour))
        {
            scintilla->SetMarginBackground(0, node->prop_as_wxColour(prop_stc_margin_0_colour));
        }

        scintilla->SetMarginMask(0, node->prop_as_bool(prop_stc_margin_0_mask_folders) ? wxSTC_MASK_FOLDERS :
                                                                                         ~wxSTC_MASK_FOLDERS);
        scintilla->SetMarginSensitive(0, node->prop_as_bool(prop_stc_margin_0_mouse));
    }

    if (node->prop_as_int(prop_stc_margin_1_width) != 0)
    {
        int width = node->prop_as_int(prop_stc_margin_1_width);
        if (width < 0)
        {
            wxString numbers("_");
            while (width < 0)
            {
                numbers << '9';
                ++width;
            }
            width = scintilla->TextWidth(wxSTC_STYLE_LINENUMBER, numbers);
        }
        scintilla->SetMarginWidth(1, width);

        if (auto result = g_stc_flags.find(node->prop_as_string(prop_stc_margin_1_type)); result != g_stc_flags.end())
            scintilla->SetMarginType(1, g_NodeCreator.GetConstantAsInt(result->second));

        if (node->prop_as_string(prop_stc_margin_1_type) == "colour" && node->HasValue(prop_stc_margin_1_colour))
        {
            scintilla->SetMarginBackground(1, node->prop_as_wxColour(prop_stc_margin_1_colour));
        }

        scintilla->SetMarginMask(1, node->prop_as_bool(prop_stc_margin_1_mask_folders) ? wxSTC_MASK_FOLDERS :
                                                                                         ~wxSTC_MASK_FOLDERS);
        scintilla->SetMarginSensitive(1, node->prop_as_bool(prop_stc_margin_1_mouse));
    }
    else
    {
        scintilla->SetMarginWidth(1, 0);
    }

    if (node->prop_as_int(prop_stc_margin_2_width) != 0)
    {
        int width = node->prop_as_int(prop_stc_margin_2_width);
        if (width < 0)
        {
            wxString numbers("_");
            while (width < 0)
            {
                numbers << '9';
                ++width;
            }
            width = scintilla->TextWidth(wxSTC_STYLE_LINENUMBER, numbers);
        }
        scintilla->SetMarginWidth(2, width);

        if (auto result = g_stc_flags.find(node->prop_as_string(prop_stc_margin_2_type)); result != g_stc_flags.end())
            scintilla->SetMarginType(2, g_NodeCreator.GetConstantAsInt(result->second));

        if (node->prop_as_string(prop_stc_margin_2_type) == "colour" && node->HasValue(prop_stc_margin_2_colour))
        {
            scintilla->SetMarginBackground(2, node->prop_as_wxColour(prop_stc_margin_2_colour));
        }

        scintilla->SetMarginMask(2, node->prop_as_bool(prop_stc_margin_2_mask_folders) ? wxSTC_MASK_FOLDERS :
                                                                                         ~wxSTC_MASK_FOLDERS);
        scintilla->SetMarginSensitive(2, node->prop_as_bool(prop_stc_margin_2_mouse));
    }

    if (node->prop_as_int(prop_folding) != 0)
    {
        // scintilla->SetMarginSensitive(1, true);

        scintilla->SetProperty("fold", "1");
        scintilla->SetFoldFlags(wxSTC_FOLDFLAG_LINEBEFORE_CONTRACTED | wxSTC_FOLDFLAG_LINEAFTER_CONTRACTED);
    }

    scintilla->SetIndentationGuides(node->prop_as_int(prop_indentation_guides));
    scintilla->SetUseTabs((node->prop_as_int(prop_use_tabs)));
    scintilla->SetTabWidth(node->prop_as_int(prop_tab_width));
    scintilla->SetTabIndents((node->prop_as_int(prop_tab_indents)));
    scintilla->SetBackSpaceUnIndents((node->prop_as_int(prop_backspace_unindents)));
    scintilla->SetIndent(node->prop_as_int(prop_tab_width));
    scintilla->SetViewEOL((node->prop_as_int(prop_view_eol)));
    scintilla->SetViewWhiteSpace(node->prop_as_int(prop_view_whitespace));

    if (node->HasValue(prop_font))
    {
        scintilla->StyleSetFont(wxSTC_STYLE_DEFAULT, node->prop_as_font(prop_font));
    }

    scintilla->Bind(wxEVT_LEFT_DOWN, &BaseGenerator::OnLeftClick, this);

    return scintilla;
}

std::optional<ttlib::cstr> StyledTextGenerator::GenConstruction(Node* node)
{
    ttlib::cstr code;
    if (node->IsLocal())
        code << "auto ";
    code << node->get_node_name() << GenerateNewAssignment(node);
    code << GetParentName(node) << ", " << node->prop_as_string(prop_id);

    GeneratePosSizeFlags(node, code);

    return code;
}

std::optional<ttlib::cstr> StyledTextGenerator::GenSettings(Node* node, size_t& auto_indent)
{
    ttlib::cstr code;

    // There are potentially a LOT of settings, so we put them all in a bracket pair to make them easier to identifiy

    auto_indent = false;
    code << "\t{";

    if (node->HasValue(prop_hint))
    {
        code << "\n\t\t" << node->get_node_name() << "->SetHint(" << GenerateQuotedString(node->prop_as_string(prop_hint))
             << ");";
    }

    if (node->prop_as_bool(prop_folding))
    {
        code << "\n\t\t" << node->get_node_name() << "->SetProperty(\"fold\", \"1\");";
        code << "\n\t\t" << node->get_node_name()
             << "->SetFoldFlags(wxSTC_FOLDFLAG_LINEBEFORE_CONTRACTED | wxSTC_FOLDFLAG_LINEAFTER_CONTRACTED);";
    }

    //////////// Wrap category settings ////////////

    if (!node->prop_as_string(prop_stc_wrap_mode).is_sameas("no wrapping"))
    {
        code << "\n\t\t" << node->get_node_name() << "->SetWrapMode("
             << g_stc_flags.at(node->prop_as_string(prop_stc_wrap_mode)) << ");";
    }
    if (node->HasValue(prop_stc_wrap_visual_flag))
    {
        ttlib::multistr flags(node->prop_as_string(prop_stc_wrap_visual_flag), '|');
        ttlib::cstr value;
        for (auto& iter: flags)
        {
            if (value.size())
                value << '|';
            value << g_stc_flags.at(iter);
        }
        if (value.size())
            code << "\n\t\t" << node->get_node_name() << "->SetWrapVisualFlags(" << value << ");";
    }
    if (node->HasValue(prop_stc_wrap_visual_location))
    {
        ttlib::multistr flags(node->prop_as_string(prop_stc_wrap_visual_location), '|');
        ttlib::cstr value;
        for (auto& iter: flags)
        {
            if (value.size())
                value << '|';
            value << g_stc_flags.at(iter);
        }
        if (value.size())
            code << "\n\t\t" << node->get_node_name() << "->SetWrapVisualFlagsLocation(" << value << ");";
    }
    if (!node->prop_as_string(prop_stc_wrap_indent_mode).is_sameas("fixed"))
    {
        code << "\n\t\t" << node->get_node_name() << "->SetWrapIndentMode("
             << g_stc_flags.at(node->prop_as_string(prop_stc_wrap_indent_mode)) << ");";
    }
    if (node->HasValue(prop_stc_wrap_start_indent))
    {
        code << "\n\t\t" << node->get_node_name() << "->SetWrapStartIndent(" << node->prop_as_int(prop_stc_wrap_start_indent)
             << ");";
    }

    //////////// Margin category settings ////////////

    if (node->prop_as_int(prop_stc_left_margin_width) != 1)
    {
        code << "\n\t\t" << node->get_node_name() << "->SetMarginLeft(" << node->prop_as_int(prop_stc_left_margin_width)
             << ");";
    }
    if (node->prop_as_int(prop_stc_right_margin_width) != 1)
    {
        code << "\n\t\t" << node->get_node_name() << "->SetMarginRight(" << node->prop_as_int(prop_stc_right_margin_width)
             << ");";
    }
    if (!node->prop_as_bool(prop_stc_select_wrapped_line))
    {
        code << "\n\t\t" << node->get_node_name() << "->SetMarginOptions(wxSTC_MARGINOPTION_SUBLINESELECT);";
    }

    if (node->prop_as_int(prop_stc_margin_0_width) != 0)
    {
        code << "\n";  // start with a blank line
        int width = node->prop_as_int(prop_stc_margin_0_width);
        if (width < 0)
        {
            ttlib::cstr numbers("_");
            while (width < 0)
            {
                numbers << '9';
                ++width;
            }
            code << "\n\t\t" << node->get_node_name() << "->SetMarginWidth(0, ";
            code << node->get_node_name() << "->TextWidth(wxSTC_STYLE_LINENUMBER, " << '"' << numbers << "\"));";
        }
        else
        {
            code << "\n\t\t" << node->get_node_name() << "->SetMarginWidth(0, " << width << ");";
        }

        if (auto result = g_stc_flags.find(node->prop_as_string(prop_stc_margin_0_type)); result != g_stc_flags.end())
        {
            code << "\n\t\t" << node->get_node_name() << "->SetMarginType(0, " << result->second << ");";
        }
        if (node->prop_as_string(prop_stc_margin_0_type) == "colour" && node->HasValue(prop_stc_margin_0_colour))
        {
            code << "\n\t\t" << node->get_node_name() << "->SetMarginBackground(0, "
                 << ConvertColourToString(node->prop_as_wxColour(prop_stc_margin_0_colour)) << ");";
        }
        code << "\n\t\t" << node->get_node_name() << "->SetMarginMask(0, "
             << (node->prop_as_bool(prop_stc_margin_0_mask_folders) ? "wxSTC_MASK_FOLDERS" : "~wxSTC_MASK_FOLDERS") << ");";
        if (node->prop_as_bool(prop_stc_margin_0_mouse))
        {
            code << "\n\t\t" << node->get_node_name() << "->SetMarginSensitive(0, true);";
        }
    }

    if (node->prop_as_int(prop_stc_margin_1_width) != 0)
    {
        code << "\n";  // start with a blank line
        int width = node->prop_as_int(prop_stc_margin_1_width);
        if (width < 0)
        {
            ttlib::cstr numbers("_");
            while (width < 0)
            {
                numbers << '9';
                ++width;
            }
            code << "\n\t\t" << node->get_node_name() << "->SetMarginWidth(1, ";
            code << node->get_node_name() << "->TextWidth(wxSTC_STYLE_LINENUMBER, " << '"' << numbers << "\"));";
        }
        else
        {
            code << "\n\t\t" << node->get_node_name() << "->SetMarginWidth(1, " << width << ");";
        }

        if (auto result = g_stc_flags.find(node->prop_as_string(prop_stc_margin_1_type)); result != g_stc_flags.end())
        {
            code << "\n\t\t" << node->get_node_name() << "->SetMarginType(1, " << result->second << ");";
        }
        if (node->prop_as_string(prop_stc_margin_1_type) == "colour" && node->HasValue(prop_stc_margin_1_colour))
        {
            code << "\n\t\t" << node->get_node_name() << "->SetMarginBackground(1, "
                 << ConvertColourToString(node->prop_as_wxColour(prop_stc_margin_1_colour)) << ");";
        }
        code << "\n\t\t" << node->get_node_name() << "->SetMarginMask(1, "
             << (node->prop_as_bool(prop_stc_margin_1_mask_folders) ? "wxSTC_MASK_FOLDERS" : "~wxSTC_MASK_FOLDERS") << ");";
        if (node->prop_as_bool(prop_stc_margin_1_mouse))
        {
            code << "\n\t\t" << node->get_node_name() << "->SetMarginSensitive(1, true);";
        }
    }

    if (node->prop_as_int(prop_stc_margin_2_width) != 0)
    {
        code << "\n";  // start with a blank line
        int width = node->prop_as_int(prop_stc_margin_2_width);
        if (width < 0)
        {
            ttlib::cstr numbers("_");
            while (width < 0)
            {
                numbers << '9';
                ++width;
            }
            code << "\n\t\t" << node->get_node_name() << "->SetMarginWidth(2, ";
            code << node->get_node_name() << "->TextWidth(wxSTC_STYLE_LINENUMBER, " << '"' << numbers << "\"));";
        }
        else
        {
            code << "\n\t\t" << node->get_node_name() << "->SetMarginWidth(2, " << width << ");";
        }

        if (auto result = g_stc_flags.find(node->prop_as_string(prop_stc_margin_2_type)); result != g_stc_flags.end())
        {
            code << "\n\t\t" << node->get_node_name() << "->SetMarginType(2, " << result->second << ");";
        }
        if (node->prop_as_string(prop_stc_margin_2_type) == "colour" && node->HasValue(prop_stc_margin_2_colour))
        {
            code << "\n\t\t" << node->get_node_name() << "->SetMarginBackground(2, "
                 << ConvertColourToString(node->prop_as_wxColour(prop_stc_margin_2_colour)) << ");";
        }
        code << "\n\t\t" << node->get_node_name() << "->SetMarginMask(2, "
             << (node->prop_as_bool(prop_stc_margin_2_mask_folders) ? "wxSTC_MASK_FOLDERS" : "~wxSTC_MASK_FOLDERS") << ");";
        if (node->prop_as_bool(prop_stc_margin_2_mouse))
        {
            code << "\n\t\t" << node->get_node_name() << "->SetMarginSensitive(2, true);";
        }
    }

    //////////// Other settings ////////////

    // Default is true, so only set if false
    if (!node->prop_as_bool(prop_line_numbers))
    {
        code << "\n\t\t" << node->get_node_name() << "->SetMarginWidth(0, 0);";
    }
    else
    {
        code << "\n\t\t" << node->get_node_name() << "->SetMarginType(0, wxSTC_MARGIN_NUMBER);";
        code << "\n\t\t" << node->get_node_name() << "->SetMarginWidth(0, " << node->get_node_name()
             << "->TextWidth(wxSTC_STYLE_LINENUMBER, \"_99999\"));";
    }

    // BUGBUG: [KeyWorks - 12-10-2020] Indentation is not a boolean -- there are 4 possible values
    if (!node->prop_as_bool(prop_indentation_guides))
        code << "\n\t\t" << node->get_node_name() << "->SetIndentationGuides(0);";

    // Default is true, so only set if false
    if (!node->prop_as_bool(prop_use_tabs))
    {
        code << "\n\t\t" << node->get_node_name() << "->SetUseTabs(false);";

        if (node->prop_as_int(prop_tab_width) != 8)
            code << "\n\t\t" << node->get_node_name() << "->SetTabWidth(" << node->prop_as_string(prop_tab_width) << ");";
    }

    // Default is true, so only set if false
    if (!node->prop_as_bool(prop_tab_indents))
        code << "\n\t\t" << node->get_node_name() << "->SetTabIndents(false);";

    // Default is false, so only set if true
    if (node->prop_as_bool(prop_backspace_unindents))
        code << "\n\t\t" << node->get_node_name() << "->SetBackSpaceUnIndents(true);";

    // Default is false, so only set if true
    if (node->prop_as_bool(prop_view_eol))
        code << "\n\t\t" << node->get_node_name() << "->SetViewEOL(true);";

    // Default is false, so only set if true
    if (node->prop_as_bool(prop_view_whitespace))
        code << "\n\t\t" << node->get_node_name() << "->SetViewWhiteSpace(true);";

    // Default is false, so only set if true
    if (node->prop_as_bool(prop_read_only))
        code << "\n\t\t" << node->get_node_name() << "->SetReadOnly(true);";

    code << "\n\t}";

    if (code.is_sameas("\t{\n\t}"))
        code.clear();  // means there were no settings

    return code;
}

std::optional<ttlib::cstr> StyledTextGenerator::GenEvents(NodeEvent* event, const std::string& class_name)
{
    return GenEventCode(event, class_name);
}

bool StyledTextGenerator::GetIncludes(Node* node, std::set<std::string>& set_src, std::set<std::string>& set_hdr)
{
    InsertGeneratorInclude(node, "#include <wx/stc/stc.h>", set_src, set_hdr);
    return true;
}
