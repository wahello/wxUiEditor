//////////////////////////////////////////////////////////////////////////
// Purpose:   wxActivityIndicator generator
// Author:    Ralph Walden
// Copyright: Copyright (c) 2020-2022 KeyWorks Software (Ralph Walden)
// License:   Apache License -- see ../../LICENSE
/////////////////////////////////////////////////////////////////////////////

#include <wx/activityindicator.h>  // wxActivityIndicator declaration.

#include "gen_common.h"     // GeneratorLibrary -- Generator classes
#include "gen_xrc_utils.h"  // Common XRC generating functions
#include "node.h"           // Node class
#include "pugixml.hpp"      // xml read/write/create/process
#include "utils.h"          // Utility functions that work with properties

#include "gen_activity.h"

wxObject* ActivityIndicatorGenerator::CreateMockup(Node* node, wxObject* parent)
{
    auto widget = new wxActivityIndicator(wxStaticCast(parent, wxWindow), wxID_ANY, DlgPoint(parent, node, prop_pos),
                                          DlgSize(parent, node, prop_size), GetStyleInt(node));

    widget->Bind(wxEVT_LEFT_DOWN, &BaseGenerator::OnLeftClick, this);
    if (node->prop_as_bool(prop_auto_start))
    {
        widget->Start();
    }

    return widget;
}

std::optional<ttlib::cstr> ActivityIndicatorGenerator::GenConstruction(Node* node)
{
    ttlib::cstr code;
    if (node->IsLocal())
        code << "auto* ";
    code << node->get_node_name() << GenerateNewAssignment(node);
    code << GetParentName(node) << ", " << node->prop_as_string(prop_id);

    GeneratePosSizeFlags(node, code);

    return code;
}

std::optional<ttlib::cstr> ActivityIndicatorGenerator::GenSettings(Node* node, size_t& /* auto_indent */)
{
    if (node->prop_as_bool(prop_auto_start))
    {
        ttlib::cstr code;
        code << node->get_node_name() << "->Start();";
        return code;
    }
    else
    {
        return {};
    }
}

bool ActivityIndicatorGenerator::GetIncludes(Node* node, std::set<std::string>& set_src, std::set<std::string>& set_hdr)
{
    InsertGeneratorInclude(node, "#include <wx/activityindicator.h>", set_src, set_hdr);
    return true;
}

int ActivityIndicatorGenerator::GenXrcObject(Node* node, pugi::xml_node& object, size_t xrc_flags)
{
    auto result = node->GetParent()->IsSizer() ? BaseGenerator::xrc_sizer_item_created : BaseGenerator::xrc_updated;
    auto item = InitializeXrcObject(node, object);

    GenXrcObjectAttributes(node, item, "wxActivityIndicator");
    GenXrcStylePosSize(node, item);

    ADD_ITEM_BOOL(prop_auto_start, "running")

    GenXrcWindowSettings(node, item);

    if (xrc_flags & xrc::add_comments)
    {
        GenXrcComments(node, item);
    }

    return result;
}

void ActivityIndicatorGenerator::RequiredHandlers(Node* /* node */, std::set<std::string>& handlers)
{
    handlers.emplace("wxActivityIndicatorXmlHandler");
}
