////////////////////////////////////////////////////////////////////////////////
// Code generated by wxUiEditor -- see https://github.com/KeyWorksRW/wxUiEditor/
//
// DO NOT EDIT THIS FILE! Your changes will be lost if it is re-generated!
////////////////////////////////////////////////////////////////////////////////

#include <wx/button.h>
#include <wx/persist.h>
#include <wx/persist/toplevel.h>
#include <wx/sizer.h>
#include <wx/stattext.h>

#include "dlg_gen_results.h"

bool GeneratedResultsDlg::Create(wxWindow* parent, wxWindowID id, const wxString& title,
        const wxPoint& pos, const wxSize& size, long style, const wxString &name)
{
    if (!wxDialog::Create(parent, id, title, pos, size, style, name))
        return false;

    auto* dlg_sizer = new wxBoxSizer(wxVERTICAL);

    auto* staticText = new wxStaticText(this, wxID_ANY, "Updated files:");
    dlg_sizer->Add(staticText, wxSizerFlags().Border(wxALL));

    m_lb_files = new wxListBox(this, wxID_ANY);
    m_lb_files->SetMinSize(ConvertDialogToPixels(wxSize(150, 100)));
    dlg_sizer->Add(m_lb_files, wxSizerFlags(1).Expand().Border(wxALL));

    auto* staticText_2 = new wxStaticText(this, wxID_ANY, "Additional information:");
    dlg_sizer->Add(staticText_2, wxSizerFlags().Border(wxALL));

    m_lb_info = new wxListBox(this, wxID_ANY);
    m_lb_info->SetMinSize(ConvertDialogToPixels(wxSize(100, 40)));
    dlg_sizer->Add(m_lb_info, wxSizerFlags().Expand().Border(wxALL));

    auto* stdBtn = CreateStdDialogButtonSizer(wxOK|wxNO_DEFAULT);
    dlg_sizer->Add(CreateSeparatedSizer(stdBtn), wxSizerFlags().Expand().Border(wxALL));

    SetSizerAndFit(dlg_sizer);
    Centre(wxBOTH);

    wxPersistentRegisterAndRestore(this, "GeneratedResultsDlg");

    return true;
}
