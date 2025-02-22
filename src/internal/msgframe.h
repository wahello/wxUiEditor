/////////////////////////////////////////////////////////////////////////////
// Purpose:   Stores messages
// Author:    Ralph Walden
// Copyright: Copyright (c) 2020-2021 KeyWorks Software (Ralph Walden)
// License:   Apache License -- see ../../LICENSE
/////////////////////////////////////////////////////////////////////////////

#pragma once

#include <wx/log.h>

#include "msgframe_base.h"

class MsgFrame : public MsgFrameBase
{
public:
    MsgFrame(std::vector<ttlib::cstr>* pMsgs, bool* pDestroyed, wxWindow* parent = nullptr);

    void OnNodeSelected();

    void AddErrorMsg(ttlib::sview msg);
    void Add_wxErrorMsg(ttlib::sview msg);

    void AddWarningMsg(ttlib::sview msg);
    void Add_wxWarningMsg(ttlib::sview msg);

    void AddInfoMsg(ttlib::sview msg) { m_textCtrl->AppendText(msg.wx_str()); };
    void AddEventMsg(ttlib::sview msg) { m_textCtrl->AppendText(msg.wx_str()); };

    void Add_wxInfoMsg(ttlib::sview msg);

protected:
    void UpdateNodeInfo();

    // Handlers for MsgFrameBase events
    void OnClear(wxCommandEvent& WXUNUSED(event)) override;
    void OnClose(wxCloseEvent& WXUNUSED(event)) override;
    void OnEvents(wxCommandEvent& WXUNUSED(event)) override;
    void OnHide(wxCommandEvent& WXUNUSED(event)) override;
    void OnInfo(wxCommandEvent& WXUNUSED(event)) override;
    void OnPageChanged(wxBookCtrlEvent& WXUNUSED(event)) override;
    void OnParent(wxCommandEvent& event) override;
    void OnSaveAs(wxCommandEvent& WXUNUSED(event)) override;
    void OnWarnings(wxCommandEvent& WXUNUSED(event)) override;

private:
    std::vector<ttlib::cstr>* m_pMsgs;
    bool* m_pDestroyed;

    bool m_isNodeInfoPage { false };
    bool m_isXrcPage { false };
};
