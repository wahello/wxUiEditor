/////////////////////////////////////////////////////////////////////////////
// Purpose:   Book component classes
// Author:    Ralph Walden
// Copyright: Copyright (c) 2020-2021 KeyWorks Software (Ralph Walden)
// License:   Apache License -- see ../../LICENSE
/////////////////////////////////////////////////////////////////////////////

#pragma once

#include "base_generator.h"  // BaseGenerator -- Base Generator class

class wxBookCtrlEvent;

class BookPageGenerator : public BaseGenerator
{
public:
    wxObject* CreateMockup(Node* node, wxObject* parent) override;

    std::optional<ttlib::cstr> GenConstruction(Node* node) override;
    bool GetIncludes(Node* node, std::set<std::string>& set_src, std::set<std::string>& set_hdr) override;
};

class PageCtrlGenerator : public BaseGenerator
{
public:
    wxObject* CreateMockup(Node* node, wxObject* parent) override;

    std::optional<ttlib::cstr> GenConstruction(Node* node) override;
};

class AuiNotebookGenerator : public BaseGenerator
{
public:
    wxObject* CreateMockup(Node* node, wxObject* parent) override;

    std::optional<ttlib::cstr> GenConstruction(Node* node) override;
    std::optional<ttlib::cstr> GenEvents(NodeEvent* event, const std::string& class_name) override;
    std::optional<ttlib::cstr> GenSettings(Node* node, size_t& /* auto_indent */) override;

    bool GetIncludes(Node* node, std::set<std::string>& set_src, std::set<std::string>& set_hdr) override;

protected:
    void OnPageChanged(wxBookCtrlEvent& event);
};

class ChoicebookGenerator : public BaseGenerator
{
public:
    wxObject* CreateMockup(Node* node, wxObject* parent) override;

    std::optional<ttlib::cstr> GenConstruction(Node* node) override;
    std::optional<ttlib::cstr> GenEvents(NodeEvent* event, const std::string& class_name) override;

    bool GetIncludes(Node* node, std::set<std::string>& set_src, std::set<std::string>& set_hdr) override;

protected:
    void OnPageChanged(wxBookCtrlEvent& event);
};

class ListbookGenerator : public BaseGenerator
{
public:
    wxObject* CreateMockup(Node* node, wxObject* parent) override;

    std::optional<ttlib::cstr> GenConstruction(Node* node) override;
    std::optional<ttlib::cstr> GenEvents(NodeEvent* event, const std::string& class_name) override;

    bool GetIncludes(Node* node, std::set<std::string>& set_src, std::set<std::string>& set_hdr) override;

protected:
    void OnPageChanged(wxBookCtrlEvent& event);
};

class NotebookGenerator : public BaseGenerator
{
public:
    wxObject* CreateMockup(Node* node, wxObject* parent) override;

    std::optional<ttlib::cstr> GenConstruction(Node* node) override;
    std::optional<ttlib::cstr> GenEvents(NodeEvent* event, const std::string& class_name) override;

    bool GetIncludes(Node* node, std::set<std::string>& set_src, std::set<std::string>& set_hdr) override;

protected:
    void OnPageChanged(wxBookCtrlEvent& event);
};

class SimplebookGenerator : public BaseGenerator
{
public:
    wxObject* CreateMockup(Node* node, wxObject* parent) override;

    std::optional<ttlib::cstr> GenConstruction(Node* node) override;
    std::optional<ttlib::cstr> GenEvents(NodeEvent* event, const std::string& class_name) override;
    std::optional<ttlib::cstr> GenSettings(Node* node, size_t& /* auto_indent */) override;

    bool GetIncludes(Node* node, std::set<std::string>& set_src, std::set<std::string>& set_hdr) override;

protected:
    void OnPageChanged(wxBookCtrlEvent& event);
};

class ToolbookGenerator : public BaseGenerator
{
public:
    wxObject* CreateMockup(Node* node, wxObject* parent) override;

    std::optional<ttlib::cstr> GenConstruction(Node* node) override;
    std::optional<ttlib::cstr> GenEvents(NodeEvent* event, const std::string& class_name) override;

    bool GetIncludes(Node* node, std::set<std::string>& set_src, std::set<std::string>& set_hdr) override;

protected:
    void OnPageChanged(wxBookCtrlEvent& event);
};

class TreebookGenerator : public BaseGenerator
{
public:
    wxObject* CreateMockup(Node* node, wxObject* parent) override;

    std::optional<ttlib::cstr> GenConstruction(Node* node) override;
    std::optional<ttlib::cstr> GenEvents(NodeEvent* event, const std::string& class_name) override;

    bool GetIncludes(Node* node, std::set<std::string>& set_src, std::set<std::string>& set_hdr) override;

protected:
    void OnPageChanged(wxBookCtrlEvent& event);
};
