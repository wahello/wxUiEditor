//////////////////////////////////////////////////////////////////////////
// Purpose:   wxToggleButton generator
// Author:    Ralph Walden
// Copyright: Copyright (c) 2020-2022 KeyWorks Software (Ralph Walden)
// License:   Apache License -- see ../../LICENSE
/////////////////////////////////////////////////////////////////////////////

#include <wx/tglbtn.h>  // This dummy header includes the proper header file for the

#include "gen_common.h"     // GeneratorLibrary -- Generator classes
#include "gen_xrc_utils.h"  // Common XRC generating functions
#include "node.h"           // Node class
#include "pugixml.hpp"      // xml read/write/create/process
#include "utils.h"          // Utility functions that work with properties
#include "write_code.h"     // WriteCode -- Write code to Scintilla or file

#include "gen_toggle_btn.h"

wxObject* ToggleButtonGenerator::CreateMockup(Node* node, wxObject* parent)
{
    auto widget = new wxToggleButton(wxStaticCast(parent, wxWindow), wxID_ANY, wxEmptyString,
                                     DlgPoint(parent, node, prop_pos), DlgSize(parent, node, prop_size), GetStyleInt(node));

    if (node->prop_as_bool(prop_markup))
        widget->SetLabelMarkup(node->prop_as_wxString(prop_label));
    else
        widget->SetLabel(node->prop_as_wxString(prop_label));

    widget->SetValue((node->prop_as_bool(prop_pressed)));

    if (node->HasValue(prop_bitmap))
        widget->SetBitmap(node->prop_as_wxBitmapBundle(prop_bitmap));

    if (node->HasValue(prop_disabled_bmp))
        widget->SetBitmapDisabled(node->prop_as_wxBitmapBundle(prop_disabled_bmp));

    if (node->HasValue(prop_pressed_bmp))
        widget->SetBitmapPressed(node->prop_as_wxBitmapBundle(prop_pressed_bmp));

    if (node->HasValue(prop_focus_bmp))
        widget->SetBitmapFocus(node->prop_as_wxBitmapBundle(prop_focus_bmp));

    if (node->HasValue(prop_current))
        widget->SetBitmapCurrent(node->prop_as_wxBitmapBundle(prop_current));

    if (node->HasValue(prop_position))
        widget->SetBitmapPosition(static_cast<wxDirection>(node->prop_as_int(prop_position)));

    if (node->HasValue(prop_margins))
        widget->SetBitmapMargins(node->prop_as_wxSize(prop_margins));

    widget->Bind(wxEVT_LEFT_DOWN, &BaseGenerator::OnLeftClick, this);

    return widget;
}

bool ToggleButtonGenerator::OnPropertyChange(wxObject* widget, Node* node, NodeProperty* prop)
{
    // We do not support changing the "markup" property because while the control displays correctly when markup is set,
    // it does not revert when markup is cleared (at least on Windows where markup controls whether a generic or native
    // version of the button is displayed).

    if (prop->isProp(prop_label))
    {
        auto ctrl = wxStaticCast(widget, wxToggleButton);
        if (node->prop_as_bool(prop_markup))
            ctrl->SetLabelMarkup(node->prop_as_wxString(prop_label));
        else
            ctrl->SetLabel(node->prop_as_wxString(prop_label));

        return true;
    }
    else if (prop->isProp(prop_markup))
    {
        // Turning markup on switches to generic rending of the button. However, you have to recreate it to switch it
        // off and go back to native rendering.

        if (node->prop_as_bool(prop_markup))
        {
            wxStaticCast(widget, wxToggleButton)->SetLabelMarkup(node->prop_as_wxString(prop_label));
            return true;
        }
    }
    else if (prop->isProp(prop_pressed))
    {
        wxStaticCast(widget, wxToggleButton)->SetValue(prop->as_bool());
        return true;
    }

    return false;
}

std::optional<ttlib::cstr> ToggleButtonGenerator::GenConstruction(Node* node)
{
    ttlib::cstr code;
    if (node->IsLocal())
        code << "auto* ";
    code << node->get_node_name() << GenerateNewAssignment(node);
    code << GetParentName(node) << ", " << node->prop_as_string(prop_id) << ", ";

    if (!node->prop_as_bool(prop_markup))
    {
        code << GenerateQuotedString(node, prop_label);
    }
    else
    {
        // prop_markup is set, so the actual label will be set in GenSettings()
        code << "wxEmptyString";
    }

    GeneratePosSizeFlags(node, code, true);
    if (code.contains("wxEmptyString)"))
        code.Replace(", wxEmptyString", "");

    return code;
}

std::optional<ttlib::cstr> ToggleButtonGenerator::GenSettings(Node* node, size_t& auto_indent)
{
    ttlib::cstr code;

    if (node->prop_as_bool(prop_pressed))
    {
        if (code.size())
            code << '\n';
        code << node->get_node_name() << "->SetValue(true)";
    }

    if (node->prop_as_bool(prop_markup))
    {
        if (code.size())
            code << '\n';
        code << node->get_node_name() << "->SetLabelMarkup(" << GenerateQuotedString(node->prop_as_string(prop_label))
             << ");";
    }

    if (node->HasValue(prop_bitmap))
    {
        auto_indent = indent::auto_keep_whitespace;

        if (node->HasValue(prop_position))
        {
            if (code.size())
                code << '\n';
            code << node->get_node_name() << "->SetBitmapPosition(" << node->prop_as_int(prop_position) << ");";
        }

        if (node->HasValue(prop_margins))
        {
            if (code.size())
                code << '\n';
            auto size = node->prop_as_wxSize(prop_margins);
            code << node->get_node_name() << "->SetBitmapMargins(" << size.GetWidth() << ", " << size.GetHeight() << ");";
        }

        GenBtnBimapCode(node, code);
    }

    return code;
}

bool ToggleButtonGenerator::GetIncludes(Node* node, std::set<std::string>& set_src, std::set<std::string>& set_hdr)
{
    InsertGeneratorInclude(node, "#include <wx/tglbtn.h>", set_src, set_hdr);
    return true;
}

int ToggleButtonGenerator::GenXrcObject(Node* node, pugi::xml_node& object, size_t xrc_flags)
{
    auto result = node->GetParent()->IsSizer() ? BaseGenerator::xrc_sizer_item_created : BaseGenerator::xrc_updated;
    auto item = InitializeXrcObject(node, object);

    GenXrcObjectAttributes(node, item, node->HasValue(prop_label) ? "wxToggleButton" : "wxBitmapToggleButton");

    ADD_ITEM_PROP(prop_label, "label")
    ADD_ITEM_BOOL(prop_pressed, "checked")

    GenXrcBitmap(node, item, xrc_flags);
    GenXrcStylePosSize(node, item);
    GenXrcWindowSettings(node, item);

    if (xrc_flags & xrc::add_comments)
    {
        GenXrcComments(node, item);
    }

    return result;
}

void ToggleButtonGenerator::RequiredHandlers(Node* /* node */, std::set<std::string>& handlers)
{
    handlers.emplace("wxToggleButtonXmlHandler");
}
