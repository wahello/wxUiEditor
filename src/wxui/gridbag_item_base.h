////////////////////////////////////////////////////////////////////////////////
// Code generated by wxUiEditor -- see https://github.com/KeyWorksRW/wxUiEditor/
//
// DO NOT EDIT THIS FILE! Your changes will be lost if it is re-generated!
////////////////////////////////////////////////////////////////////////////////

#pragma once

#include <wx/dialog.h>
#include <wx/event.h>
#include <wx/gdicmn.h>
#include <wx/infobar.h>
#include <wx/radiobut.h>
#include <wx/spinctrl.h>

class GridBagItemBase : public wxDialog
{
public:
    GridBagItemBase() {}
    GridBagItemBase(wxWindow *parent, wxWindowID id = wxID_ANY, const wxString& title = "Append or Insert into wxGridBagSizer",
        const wxPoint& pos = wxDefaultPosition, const wxSize& size = wxDefaultSize,
        long style = wxDEFAULT_DIALOG_STYLE, const wxString &name = wxDialogNameStr)
    {
        Create(parent, id, title, pos, size, style, name);
    }

    bool Create(wxWindow *parent, wxWindowID id = wxID_ANY, const wxString& title = "Append or Insert into wxGridBagSizer",
        const wxPoint& pos = wxDefaultPosition, const wxSize& size = wxDefaultSize,
        long style = wxDEFAULT_DIALOG_STYLE, const wxString &name = wxDialogNameStr);

protected:

    // Virtual event handlers -- override them in your derived class

    virtual void OnColumn(wxSpinEvent& event) { event.Skip(); }
    virtual void OnInit(wxInitDialogEvent& event) { event.Skip(); }
    virtual void OnOK(wxCommandEvent& event) { event.Skip(); }
    virtual void OnRow(wxSpinEvent& event) { event.Skip(); }

    // Class member variables

    wxInfoBar* m_infoBar;
    wxRadioButton* m_radio_column;
    wxRadioButton* m_radio_row;
    wxSpinCtrl* m_spin_column;
    wxSpinCtrl* m_spin_row;
    wxSpinCtrl* m_spin_span_column;
    wxSpinCtrl* m_spin_span_row;
};
