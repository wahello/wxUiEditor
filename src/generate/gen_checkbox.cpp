/////////////////////////////////////////////////////////////////////////////
// Purpose:   wxCheckBox generator
// Author:    Ralph Walden
// Copyright: Copyright (c) 2020-2022 KeyWorks Software (Ralph Walden)
// License:   Apache License -- see ../../LICENSE
/////////////////////////////////////////////////////////////////////////////

#include <wx/checkbox.h>  // wxCheckBox class interface

#include "gen_common.h"  // GeneratorLibrary -- Generator classes
#include "node.h"        // Node class
#include "utils.h"       // Utility functions that work with properties

#include "gen_checkbox.h"

wxObject* CheckBoxGenerator::CreateMockup(Node* node, wxObject* parent)
{
    long style_value = 0;
    if (node->prop_as_string(prop_style).contains("wxALIGN_RIGHT"))
        style_value |= wxALIGN_RIGHT;

    auto widget =
        new wxCheckBox(wxStaticCast(parent, wxWindow), wxID_ANY, node->prop_as_wxString(prop_label),
                       DlgPoint(parent, node, prop_pos), DlgSize(parent, node, prop_size), style_value | GetStyleInt(node));

    if (node->prop_as_bool(prop_checked))
        widget->SetValue(true);

    widget->Bind(wxEVT_LEFT_DOWN, &BaseGenerator::OnLeftClick, this);

    return widget;
}

bool CheckBoxGenerator::OnPropertyChange(wxObject* widget, Node* node, NodeProperty* prop)
{
    if (prop->isProp(prop_label))
    {
        wxStaticCast(widget, wxCheckBox)->SetLabel(node->prop_as_wxString(prop_label));
        return true;
    }
    else if (prop->isProp(prop_checked))
    {
        wxStaticCast(widget, wxCheckBox)->SetValue(prop->as_bool());
        return true;
    }

    return false;
}

std::optional<ttlib::cstr> CheckBoxGenerator::GenConstruction(Node* node)
{
    ttlib::cstr code;
    if (node->IsLocal())
        code << "auto* ";
    code << node->get_node_name() << GenerateNewAssignment(node);
    code << GetParentName(node) << ", " << node->prop_as_string(prop_id) << ", ";

    auto& label = node->prop_as_string(prop_label);
    if (label.size())
    {
        code << GenerateQuotedString(label);
    }
    else
    {
        code << "wxEmptyString";
    }

    GeneratePosSizeFlags(node, code);

    return code;
}

std::optional<ttlib::cstr> CheckBoxGenerator::GenSettings(Node* node, size_t& /* auto_indent */)
{
    ttlib::cstr code;

    if (node->prop_as_bool(prop_checked))
        code << node->get_node_name() << "->SetValue(true);";

    return code;
}

bool CheckBoxGenerator::GetIncludes(Node* node, std::set<std::string>& set_src, std::set<std::string>& set_hdr)
{
    InsertGeneratorInclude(node, "#include <wx/checkbox.h>", set_src, set_hdr);
    return true;
}

int CheckBoxGenerator::GenXrcObject(Node* node, pugi::xml_node& object, size_t xrc_flags)
{
    auto result = node->GetParent()->IsSizer() ? BaseGenerator::xrc_sizer_item_created : BaseGenerator::xrc_updated;
    auto item = InitializeXrcObject(node, object);

    GenXrcObjectAttributes(node, item, "wxCheckBox");

    ADD_ITEM_PROP(prop_label, "label")
    ADD_ITEM_BOOL(prop_checked, "checked")

    GenXrcStylePosSize(node, item);
    GenXrcWindowSettings(node, item);

    if (xrc_flags & xrc::add_comments)
    {
        GenXrcComments(node, item);
    }

    return result;
}

void CheckBoxGenerator::RequiredHandlers(Node* /* node */, std::set<std::string>& handlers)
{
    handlers.emplace("wxCheckBoxXmlHandler");
}

//////////////////////////////////////////  Check3StateGenerator  //////////////////////////////////////////

wxObject* Check3StateGenerator::CreateMockup(Node* node, wxObject* parent)
{
    long style_value = wxCHK_3STATE | GetStyleInt(node);

    auto widget = new wxCheckBox(wxStaticCast(parent, wxWindow), wxID_ANY, node->prop_as_wxString(prop_label),
                                 DlgPoint(parent, node, prop_pos), DlgSize(parent, node, prop_size), style_value);

    auto& state = node->prop_as_string(prop_initial_state);
    if (state == "wxCHK_UNCHECKED")
        widget->Set3StateValue(wxCHK_UNCHECKED);
    else if (state == "wxCHK_CHECKED")
        widget->Set3StateValue(wxCHK_CHECKED);
    else
        widget->Set3StateValue(wxCHK_UNDETERMINED);

    widget->Bind(wxEVT_LEFT_DOWN, &BaseGenerator::OnLeftClick, this);

    return widget;
}

bool Check3StateGenerator::OnPropertyChange(wxObject* widget, Node* node, NodeProperty* prop)
{
    if (prop->isProp(prop_label))
    {
        wxStaticCast(widget, wxCheckBox)->SetLabel(node->prop_as_wxString(prop_label));
        return true;
    }
    else if (prop->isProp(prop_initial_state))
    {
        auto& state = prop->as_string();
        if (state == "wxCHK_UNCHECKED")
            wxStaticCast(widget, wxCheckBox)->Set3StateValue(wxCHK_UNCHECKED);
        else if (state == "wxCHK_CHECKED")
            wxStaticCast(widget, wxCheckBox)->Set3StateValue(wxCHK_CHECKED);
        else
            wxStaticCast(widget, wxCheckBox)->Set3StateValue(wxCHK_UNDETERMINED);
        return true;
    }

    return false;
}

std::optional<ttlib::cstr> Check3StateGenerator::GenConstruction(Node* node)
{
    ttlib::cstr code;
    if (node->IsLocal())
        code << "auto* ";
    code << node->get_node_name() << " = new wxCheckBox(";
    code << GetParentName(node) << ", " << node->prop_as_string(prop_id) << ", ";

    auto& label = node->prop_as_string(prop_label);
    if (label.size())
    {
        code << GenerateQuotedString(label);
    }
    else
    {
        code << "wxEmptyString";
    }

    code << ", ";
    GenPos(node, code);
    code << ", ";
    GenSize(node, code);
    code << ", ";
    code << "wxCHK_3STATE";
    auto& style = node->prop_as_string(prop_style);
    if (style.size())
        code << '|' << style;
    auto& win_style = node->prop_as_string(prop_window_style);
    if (win_style.size())
        code << '|' << win_style;
    auto& win_name = node->prop_as_string(prop_window_name);
    if (win_name.size())
    {
        code << ", wxDefaultValidator, " << node->prop_as_string(prop_window_name);
    }
    code << ");";

    return code;
}

std::optional<ttlib::cstr> Check3StateGenerator::GenSettings(Node* node, size_t& /* auto_indent */)
{
    ttlib::cstr code;

    auto& state = node->prop_as_string(prop_initial_state);
    if (state == "wxCHK_CHECKED")
    {
        code << node->get_node_name() << "->Set3StateValue(wxCHK_CHECKED);";
    }
    else if (state == "wxCHK_UNDETERMINED")
    {
        code << node->get_node_name() << "->Set3StateValue(wxCHK_UNDETERMINED);";
    }

    return code;
}

bool Check3StateGenerator::GetIncludes(Node* node, std::set<std::string>& set_src, std::set<std::string>& set_hdr)
{
    InsertGeneratorInclude(node, "#include <wx/checkbox.h>", set_src, set_hdr);
    return true;
}

int Check3StateGenerator::GenXrcObject(Node* node, pugi::xml_node& object, size_t xrc_flags)
{
    auto result = node->GetParent()->IsSizer() ? BaseGenerator::xrc_sizer_item_created : BaseGenerator::xrc_updated;
    auto item = InitializeXrcObject(node, object);

    GenXrcObjectAttributes(node, item, "wxCheckBox");

    ADD_ITEM_PROP(prop_label, "label")

    if (node->prop_as_string(prop_initial_state) == "wxCHK_CHECKED")
    {
        item.append_child("checked").text().set("1");
    }
    else if (node->prop_as_string(prop_initial_state) == "wxCHK_UNDETERMINED")
    {
        item.append_child("checked").text().set("2");
    }

    ttlib::cstr styles(node->prop_as_string(prop_style));
    if (styles.size())
    {
        styles << '|';
    }
    styles << "wxCHK_3STATE";
    GenXrcPreStylePosSize(node, item, styles);

    GenXrcWindowSettings(node, item);

    if (xrc_flags & xrc::add_comments)
    {
        GenXrcComments(node, item);
    }

    return result;
}

void Check3StateGenerator::RequiredHandlers(Node* /* node */, std::set<std::string>& handlers)
{
    handlers.emplace("wxCheckBoxXmlHandler");
}
