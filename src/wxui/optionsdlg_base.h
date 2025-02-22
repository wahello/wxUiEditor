////////////////////////////////////////////////////////////////////////////////
// Code generated by wxUiEditor -- see https://github.com/KeyWorksRW/wxUiEditor/
//
// DO NOT EDIT THIS FILE! Your changes will be lost if it is re-generated!
////////////////////////////////////////////////////////////////////////////////

#pragma once

#include <wx/dialog.h>
#include <wx/event.h>
#include <wx/gdicmn.h>

class OptionsDlgBase : public wxDialog
{
public:
    OptionsDlgBase() {}
    OptionsDlgBase(wxWindow *parent, wxWindowID id = wxID_ANY, const wxString& title = "Options",
        const wxPoint& pos = wxDefaultPosition, const wxSize& size = wxDefaultSize,
        long style = wxDEFAULT_DIALOG_STYLE, const wxString &name = wxDialogNameStr)
    {
        Create(parent, id, title, pos, size, style, name);
    }

    bool Create(wxWindow *parent, wxWindowID id = wxID_ANY, const wxString& title = "Options",
        const wxPoint& pos = wxDefaultPosition, const wxSize& size = wxDefaultSize,
        long style = wxDEFAULT_DIALOG_STYLE, const wxString &name = wxDialogNameStr);

protected:

    // Virtual event handlers -- override them in your derived class

    virtual void OnAffirmative(wxCommandEvent& event) { event.Skip(); }
    virtual void OnInit(wxInitDialogEvent& event) { event.Skip(); }

    // Validator variables

    bool m_isWakaTimeEnabled { true };
    bool m_sizers_all_borders { true };
    bool m_sizers_always_expand { true };
};
