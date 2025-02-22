////////////////////////////////////////////////////////////////////////////////
// Code generated by wxUiEditor -- see https://github.com/KeyWorksRW/wxUiEditor/
//
// DO NOT EDIT THIS FILE! Your changes will be lost if it is re-generated!
////////////////////////////////////////////////////////////////////////////////

#include <wx/sizer.h>
#include <wx/stattext.h>

#include "code_compare_base.h"

#include "WinMerge.xpm"

bool CodeCompare::Create(wxWindow* parent, wxWindowID id, const wxString& title,
        const wxPoint& pos, const wxSize& size, long style, const wxString &name)
{
    if (!wxDialog::Create(parent, id, title, pos, size, style, name))
        return false;

    auto* dlg_sizer = new wxBoxSizer(wxVERTICAL);

    auto* box_sizer = new wxBoxSizer(wxVERTICAL);
    dlg_sizer->Add(box_sizer, wxSizerFlags().Border(wxALL));

    auto* staticText_2 = new wxStaticText(this, wxID_ANY, "If you have WinMergeU.exe installed, and you have previously generated classes, then you can use this to compare what would be generated now. This can be helpful if you need to confirm code generation changes without having to actually write out the code.\n");
    staticText_2->Wrap(320);
    box_sizer->Add(staticText_2, wxSizerFlags().Border(wxLEFT|wxRIGHT|wxTOP, wxSizerFlags::GetDefaultBorder()));

    auto* grid_sizer = new wxGridSizer(3);
    box_sizer->Add(grid_sizer, wxSizerFlags().Center().Border(wxALL));

    m_radio_cplusplus = new wxRadioButton(this, wxID_ANY, "&C++", wxDefaultPosition, wxDefaultSize, wxRB_GROUP);
    grid_sizer->Add(m_radio_cplusplus, wxSizerFlags().Border(wxALL));

    m_radio_python = new wxRadioButton(this, wxID_ANY, "&Python");
    grid_sizer->Add(m_radio_python, wxSizerFlags().Border(wxALL));

    box_sizer->AddSpacer(15);

    auto* staticText = new wxStaticText(this, wxID_ANY, "C&hanged Classes:");
    box_sizer->Add(staticText, wxSizerFlags().Border(wxALL));

    m_list_changes = new wxListBox(this, wxID_ANY);
    m_list_changes->Enable(false);
    m_list_changes->SetMinSize(wxSize(250, 200));
    box_sizer->Add(m_list_changes, wxSizerFlags().Expand().Border(wxALL));

    m_btn = new wxButton(this, wxID_ANY, "&WinMerge...");
#if wxCHECK_VERSION(3, 1, 6)
        m_btn->SetBitmap(wxBitmapBundle::FromBitmap(wxImage(WinMerge_xpm)));
#else
    m_btn->SetBitmap(wxImage(WinMerge_xpm));
#endif  // wxCHECK_VERSION(3, 1, 6)
    m_btn->Enable(false);
    box_sizer->Add(m_btn, wxSizerFlags().Border(wxALL));

    auto* stdBtn = CreateStdDialogButtonSizer(wxCLOSE|wxNO_DEFAULT);
    stdBtn->GetCancelButton()->SetDefault();
    dlg_sizer->Add(CreateSeparatedSizer(stdBtn), wxSizerFlags().Expand().Border(wxALL));

    SetSizerAndFit(dlg_sizer);
    Centre(wxBOTH);

    // Event handlers
    Bind(wxEVT_INIT_DIALOG, &CodeCompare::OnInit, this);
    m_radio_cplusplus->Bind(wxEVT_RADIOBUTTON, &CodeCompare::OnCPlusPlus, this);
    m_radio_python->Bind(wxEVT_RADIOBUTTON, &CodeCompare::OnPython, this);
    m_btn->Bind(wxEVT_BUTTON, &CodeCompare::OnWinMerge, this);

    return true;
}
