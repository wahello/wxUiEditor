////////////////////////////////////////////////////////////////////////////////
// Code generated by wxUiEditor -- see https://github.com/KeyWorksRW/wxUiEditor/
//
// DO NOT EDIT THIS FILE! Your changes will be lost if it is re-generated!
////////////////////////////////////////////////////////////////////////////////

#pragma once

#include <wx/dialog.h>
#include <wx/gdicmn.h>
#include <wx/stattext.h>
#include <wx/toolbook.h>

class ToolbookBase : public wxDialog
{
public:
    ToolbookBase(wxWindow* parent, wxWindowID id = wxID_ANY,
        const wxString& title = wxString::FromUTF8("wxToolbook"),
        const wxPoint& pos = wxDefaultPosition, const wxSize& size = wxDefaultSize,
        long style = wxDEFAULT_DIALOG_STYLE);

protected:

    // Class member variables

    wxStaticText* m_staticText;
    wxToolbook* m_toolbook;
};
