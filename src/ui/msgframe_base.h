////////////////////////////////////////////////////////////////////////////////
// Code generated by wxUiEditor -- see https://github.com/KeyWorksRW/wxUiEditor/
//
// DO NOT EDIT THIS FILE! Your changes will be lost if it is re-generated!
////////////////////////////////////////////////////////////////////////////////

#pragma once

#include <wx/event.h>
#include <wx/frame.h>
#include <wx/gdicmn.h>
#include <wx/menu.h>
#include <wx/textctrl.h>

class MsgFrameBase : public wxFrame
{
public:
    MsgFrameBase(wxWindow* parent, wxWindowID id = wxID_ANY,
        const wxString& title = wxString::FromUTF8("wxUiEditor Messages"),
        const wxPoint& pos = wxDefaultPosition, const wxSize& size = wxDefaultSize,
        long style = wxDEFAULT_FRAME_STYLE|wxTAB_TRAVERSAL);

    enum
    {
        id_event_msgs = wxID_HIGHEST + 1,
        id_hide,
        id_warning_msgs
    };

protected:

    // Class member variables

    wxMenuItem* m_menu_item_events;
    wxMenuItem* m_menu_item_info;
    wxMenuItem* m_menu_item_warnings;
    wxTextCtrl* m_textCtrl;

    // Virtual event handlers -- override them in your derived class

    virtual void OnClear(wxCommandEvent& event) { event.Skip(); }
    virtual void OnClose(wxCloseEvent& event) { event.Skip(); }
    virtual void OnEvents(wxCommandEvent& event) { event.Skip(); }
    virtual void OnHide(wxCommandEvent& event) { event.Skip(); }
    virtual void OnInfo(wxCommandEvent& event) { event.Skip(); }
    virtual void OnSaveAs(wxCommandEvent& event) { event.Skip(); }
    virtual void OnWarnings(wxCommandEvent& event) { event.Skip(); }
};
