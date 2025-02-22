/////////////////////////////////////////////////////////////////////////////
// Purpose:   NodeDeclaration class
// Author:    Ralph Walden
// Copyright: Copyright (c) 2020-2021 KeyWorks Software (Ralph Walden)
// License:   Apache License -- see ../../LICENSE
/////////////////////////////////////////////////////////////////////////////

#pragma once

#include <wx/image.h>  // wxImage class

#include <map>
#include <optional>
#include <set>

#include "node_classes.h"  // Forward defintions of Node classes

#include "category.h"    // NodeCategory -- Node property categories
#include "gen_enums.h"   // Enumerations for generators
#include "node_types.h"  // NodeType -- Class for storing component types and allowable child count

using namespace GenEnum;

using DblStrMap = std::map<std::string, std::string, std::less<>>;
using PropDeclarationPtr = std::shared_ptr<PropDeclaration>;
using PropDeclarationMap = std::map<std::string, PropDeclarationPtr>;

namespace pugi
{
    class xml_node;
}

class NodeDeclaration
{
public:
    NodeDeclaration(ttlib::sview class_name, NodeType* type);
    ~NodeDeclaration();

    NodeCategory& GetCategory() { return m_category; }

    size_t GetPropertyCount() const { return m_properties.size(); }
    size_t GetEventCount() const { return m_events.size(); }

    PropDeclaration* GetPropDeclaration(size_t idx) const;

    NodeEventInfo* GetEventInfo(ttlib::sview name);
    const NodeEventInfo* GetEventInfo(size_t idx) const;

    PropDeclarationMap& GetPropInfoMap() { return m_properties; }

    NodeType* GetNodeType() const { return m_type; }

    GenType gen_type() const noexcept { return m_gen_type; }
    GenName gen_name() const noexcept { return m_gen_name; }

    bool isType(GenType type) const noexcept { return (type == m_gen_type); }
    bool isGen(GenName name) const noexcept { return (name == m_gen_name); }

    ttlib::sview DeclName() const noexcept { return ttlib::sview(m_name); }

    size_t AddBaseClass(NodeDeclaration* base)
    {
        m_base.emplace_back(base);
        return m_base.size() - 1;
    }

    bool isSubclassOf(GenName class_name) const;

    NodeDeclaration* GetBaseClass(size_t idx, bool inherited = true) const;

    void GetBaseClasses(std::vector<NodeDeclaration*>& classes, bool inherited = true) const;

    size_t GetBaseClassCount(bool inherited = true) const;
    bool hasBaseClasses() const { return m_base.size(); }

    void SetImage(wxImage image) { m_image = image; }
    wxImage GetImage() const { return m_image; }

    void SetGenerator(BaseGenerator* generator) { m_generator = generator; }
    BaseGenerator* GetGenerator() const { return m_generator; }

    void ParseEvents(pugi::xml_node& elem_obj, NodeCategory& category);

    const ttlib::cstr& GetGeneratorFlags() { return m_internal_flags; }
    void SetGeneratorFlags(std::string_view flags) { m_internal_flags = flags; }

    int_t GetAllowableChildren(GenType child_gen_type) const;

    void SetOverRideDefValue(GenEnum::PropName prop_name, std::string_view new_value)
    {
        m_override_def_values[prop_name] = new_value;
    }
    std::optional<ttlib::cstr> GetOverRideDefValue(GenEnum::PropName prop_name);

    void HideProperty(GenEnum::PropName prop_name) { m_hide_properties.emplace(prop_name); }
    bool IsPropHidden(GenEnum::PropName prop_name) { return (m_hide_properties.find(prop_name) != m_hide_properties.end()); }

private:
    ttlib::cstr m_internal_flags;

    wxImage m_image;  // The node's image, primarily used in the navigation pane

    NodeType* m_type;

    NodeCategory m_category;

    PropDeclarationMap m_properties;  // std::map<std::string, PropDeclarationPtr>
    std::map<std::string, std::unique_ptr<NodeEventInfo>, std::less<>> m_events;

    std::map<GenEnum::PropName, std::string> m_override_def_values;
    std::set<GenEnum::PropName> m_hide_properties;

    std::vector<NodeDeclaration*> m_base;  // base classes

    BaseGenerator* m_generator { nullptr };

    GenName m_gen_name;
    GenType m_gen_type;
    const char* m_name;  // this points into map_GenNames
};
