/////////////////////////////////////////////////////////////////////////////
// Purpose:   wxInfoBar generator
// Author:    Ralph Walden
// Copyright: Copyright (c) 2020-2022 KeyWorks Software (Ralph Walden)
// License:   Apache License -- see ../../LICENSE
/////////////////////////////////////////////////////////////////////////////

#pragma once

#include <wx/timer.h>  // wxTimer, wxStopWatch and global time-related functions

#include "base_generator.h"  // BaseGenerator -- Base Generator class

class wxTimerEvent;
class wxInfoBar;

class InfoBarGenerator : public BaseGenerator
{
public:
    wxObject* CreateMockup(Node* node, wxObject* parent) override;

    std::optional<ttlib::cstr> GenConstruction(Node* node) override;
    std::optional<ttlib::cstr> GenSettings(Node* node, size_t& auto_indent) override;

    bool GetIncludes(Node* node, std::set<std::string>& set_src, std::set<std::string>& set_hdr) override;

    int GenXrcObject(Node*, pugi::xml_node& /* object */, size_t /* xrc_flags */) override;
    void RequiredHandlers(Node*, std::set<std::string>& /* handlers */) override;

protected:
    void OnButton(wxCommandEvent& event);
    void OnTimer(wxTimerEvent&);

private:
    wxTimer m_timer;
    wxInfoBar* m_infobar;
};
