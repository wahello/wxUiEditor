////////////////////////////////////////////////////////////////////////////////
// Code generated by wxUiEditor -- see https://github.com/KeyWorksRW/wxUiEditor/
//
// DO NOT EDIT THIS FILE! Your changes will be lost if it is re-generated!
////////////////////////////////////////////////////////////////////////////////

#pragma once

#include <wx/bitmap.h>
#include <wx/button.h>
#include <wx/dialog.h>
#include <wx/event.h>
#include <wx/gdicmn.h>
#include <wx/icon.h>
#include <wx/image.h>
#include <wx/listbox.h>
#include <wx/sizer.h>
#include <wx/textctrl.h>

class InsertWidget : public wxDialog
{
public:
    InsertWidget() {}
    InsertWidget(wxWindow *parent, wxWindowID id = wxID_ANY, const wxString& title = "Insert Widget",
        const wxPoint& pos = wxDefaultPosition, const wxSize& size = wxDefaultSize,
        long style = wxDEFAULT_DIALOG_STYLE, const wxString &name = wxDialogNameStr)
    {
        Create(parent, id, title, pos, size, style, name);
    }

    bool Create(wxWindow *parent, wxWindowID id = wxID_ANY, const wxString& title = "Insert Widget",
        const wxPoint& pos = wxDefaultPosition, const wxSize& size = wxDefaultSize,
        long style = wxDEFAULT_DIALOG_STYLE, const wxString &name = wxDialogNameStr);

    ttlib::cstr GetWidget() { return m_widget; }

private:
    ttlib::cstr m_widget;

protected:

    // Event handlers

    void OnInit(wxInitDialogEvent& event);
    void OnKeyDown(wxKeyEvent& event);
    void OnListBoxDblClick(wxCommandEvent& event);
    void OnNameText(wxCommandEvent& event);
    void OnOK(wxCommandEvent& event);

private:

    // Class member variables

    wxListBox* m_listbox;
    wxStdDialogButtonSizer* m_stdBtn;
    wxButton* m_stdBtnOK;
    wxButton* m_stdBtnCancel;
    wxTextCtrl* m_text_name;
};
