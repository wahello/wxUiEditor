/////////////////////////////////////////////////////////////////////////////
// Purpose:   Ribbon component classes
// Author:    Ralph Walden
// Copyright: Copyright (c) 2020-2022 KeyWorks Software (Ralph Walden)
// License:   Apache License -- see ../../LICENSE
/////////////////////////////////////////////////////////////////////////////

#pragma once

#include "base_generator.h"  // BaseGenerator -- Base Generator class

class wxRibbonBarEvent;

class RibbonBarFormGenerator : public BaseGenerator
{
public:
    wxObject* CreateMockup(Node* node, wxObject* parent) override;
    void AfterCreation(wxObject* wxobject, wxWindow* /*wxparent*/, Node* /* node */, bool /* is_preview */) override;

    std::optional<ttlib::cstr> GenConstruction(Node* node) override;
    std::optional<ttlib::cstr> GenAdditionalCode(GenEnum::GenCodeType cmd, Node* node) override;
    std::optional<ttlib::cstr> GenSettings(Node* node, size_t& auto_indent) override;
    std::optional<ttlib::sview> GenEvents(Code&, NodeEvent*, const std::string&) override;

    bool GetIncludes(Node* node, std::set<std::string>& set_src, std::set<std::string>& set_hdr) override;

protected:
    void OnPageChanged(wxRibbonBarEvent& event);
};

class RibbonBarGenerator : public BaseGenerator
{
public:
    wxObject* CreateMockup(Node* node, wxObject* parent) override;
    void AfterCreation(wxObject* wxobject, wxWindow* /*wxparent*/, Node* /* node */, bool /* is_preview */) override;

    std::optional<ttlib::cstr> GenConstruction(Node* node) override;
    std::optional<ttlib::cstr> GenSettings(Node* node, size_t& auto_indent) override;

    bool GetIncludes(Node* node, std::set<std::string>& set_src, std::set<std::string>& set_hdr) override;

    int GenXrcObject(Node*, pugi::xml_node& /* object */, size_t /* xrc_flags */) override;
    void RequiredHandlers(Node*, std::set<std::string>& /* handlers */) override;

protected:
    void OnPageChanged(wxRibbonBarEvent& event);
};

class RibbonPageGenerator : public BaseGenerator
{
public:
    wxObject* CreateMockup(Node* node, wxObject* parent) override;

    std::optional<ttlib::cstr> GenConstruction(Node* node) override;
    std::optional<ttlib::cstr> GenSettings(Node* node, size_t& auto_indent) override;

    bool GetIncludes(Node* node, std::set<std::string>& set_src, std::set<std::string>& set_hdr) override;

    int GenXrcObject(Node*, pugi::xml_node& /* object */, size_t /* xrc_flags */) override;
};

class RibbonPanelGenerator : public BaseGenerator
{
public:
    wxObject* CreateMockup(Node* node, wxObject* parent) override;

    std::optional<ttlib::cstr> GenConstruction(Node* node) override;

    bool GetIncludes(Node* node, std::set<std::string>& set_src, std::set<std::string>& set_hdr) override;

    int GenXrcObject(Node*, pugi::xml_node& /* object */, size_t /* xrc_flags */) override;
};

class RibbonButtonBarGenerator : public BaseGenerator
{
public:
    wxObject* CreateMockup(Node* node, wxObject* parent) override;
    void AfterCreation(wxObject* wxobject, wxWindow* /*wxparent*/, Node* /* node */, bool /* is_preview */) override;

    std::optional<ttlib::cstr> GenConstruction(Node* node) override;

    bool GetIncludes(Node* node, std::set<std::string>& set_src, std::set<std::string>& set_hdr) override;

    int GenXrcObject(Node*, pugi::xml_node& /* object */, size_t /* xrc_flags */) override;
};

class RibbonButtonGenerator : public BaseGenerator
{
public:
    std::optional<ttlib::cstr> GenConstruction(Node* node) override;

    int GenXrcObject(Node*, pugi::xml_node& /* object */, size_t /* xrc_flags */) override;
};

class RibbonToolBarGenerator : public BaseGenerator
{
public:
    wxObject* CreateMockup(Node* node, wxObject* parent) override;
    void AfterCreation(wxObject* wxobject, wxWindow* /*wxparent*/, Node* /* node */, bool /* is_preview */) override;

    std::optional<ttlib::cstr> GenConstruction(Node* node) override;
    std::optional<ttlib::cstr> GenSettings(Node* node, size_t& auto_indent) override;

    bool GetIncludes(Node* node, std::set<std::string>& set_src, std::set<std::string>& set_hdr) override;

    int GenXrcObject(Node*, pugi::xml_node& /* object */, size_t /* xrc_flags */) override;
};

class RibbonToolGenerator : public BaseGenerator
{
public:
    std::optional<ttlib::cstr> GenConstruction(Node* node) override;

    int GenXrcObject(Node*, pugi::xml_node& /* object */, size_t /* xrc_flags */) override;
};

class RibbonGalleryGenerator : public BaseGenerator
{
public:
    wxObject* CreateMockup(Node* node, wxObject* parent) override;
    void AfterCreation(wxObject* wxobject, wxWindow* /*wxparent*/, Node* /* node */, bool /* is_preview */) override;

    std::optional<ttlib::cstr> GenConstruction(Node* node) override;

    bool GetIncludes(Node* node, std::set<std::string>& set_src, std::set<std::string>& set_hdr) override;

    int GenXrcObject(Node*, pugi::xml_node& /* object */, size_t /* xrc_flags */) override;
};

class RibbonGalleryItemGenerator : public BaseGenerator
{
public:
    std::optional<ttlib::cstr> GenConstruction(Node* node) override;

    int GenXrcObject(Node*, pugi::xml_node& /* object */, size_t /* xrc_flags */) override;
};
