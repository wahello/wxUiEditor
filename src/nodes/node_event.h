/////////////////////////////////////////////////////////////////////////////
// Purpose:   NodeEvent and NodeEventInfo classes
// Author:    Ralph Walden
// Copyright: Copyright (c) 2020 KeyWorks Software (Ralph Walden)
// License:   Apache License -- see ../../LICENSE
/////////////////////////////////////////////////////////////////////////////

#pragma once

class Node;

class NodeEventInfo
{
public:
    NodeEventInfo(std::string_view name, std::string_view event_class, std::string_view help) :
        m_name(name), m_event_class(event_class), m_help(help)
    {
    }

    const ttlib::cstr& get_name() const noexcept { return m_name; }
    const ttlib::cstr& get_event_class() const noexcept { return m_event_class; }
    const ttlib::cstr& get_help() const noexcept { return m_help; }

private:
    ttlib::cstr m_name;
    ttlib::cstr m_event_class;
    ttlib::cstr m_help;
};

class NodeEvent
{
public:
    NodeEvent(const NodeEventInfo* info, Node* node) : m_info(info), m_node(node) {}

    void set_value(std::string_view value) { m_value = value; }
    const ttlib::cstr& get_value() const noexcept { return m_value; }
    const ttlib::cstr& get_name() const noexcept { return m_info->get_name(); }

    Node* GetNode() const noexcept { return m_node; }
    const NodeEventInfo* GetEventInfo() const noexcept { return m_info; }

    // Currently only called in debug builds, but available for release builds should we need it
    size_t GetEventSize() const noexcept { return sizeof(*this) + (m_value.size() + 1); }

private:
    const NodeEventInfo* m_info;
    Node* m_node;

    ttlib::cstr m_value;  // handler function name
};
