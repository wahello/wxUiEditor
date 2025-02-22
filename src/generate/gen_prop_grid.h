/////////////////////////////////////////////////////////////////////////////
// Purpose:   wxPropertyGrid generator
// Author:    Ralph Walden
// Copyright: Copyright (c) 2020-2022 KeyWorks Software (Ralph Walden)
// License:   Apache License -- see ../../LICENSE
/////////////////////////////////////////////////////////////////////////////

#pragma once

#include "base_generator.h"  // BaseGenerator -- Base Generator class

class PropertyGridGenerator : public BaseGenerator
{
public:
    wxObject* CreateMockup(Node* node, wxObject* parent) override;
    void AfterCreation(wxObject* wxobject, wxWindow* /*wxparent*/, Node* /* node */, bool /* is_preview */) override;

    std::optional<ttlib::cstr> GenConstruction(Node* node) override;

    bool GetIncludes(Node* node, std::set<std::string>& set_src, std::set<std::string>& set_hdr) override;
};
