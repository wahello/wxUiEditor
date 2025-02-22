////////////////////////////////////////////////////////////////////////////////
// Code generated by wxUiEditor -- see https://github.com/KeyWorksRW/wxUiEditor/
//
// DO NOT EDIT THIS FILE! Your changes will be lost if it is re-generated!
////////////////////////////////////////////////////////////////////////////////

#include <wx/button.h>
#include <wx/valgen.h>
#include <wx/valtext.h>

#include "newribbon_base.h"

bool NewRibbon::Create(wxWindow* parent, wxWindowID id, const wxString& title,
        const wxPoint& pos, const wxSize& size, long style, const wxString &name)
{
    if (!wxDialog::Create(parent, id, title, pos, size, style, name))
        return false;

    auto* box_sizer = new wxBoxSizer(wxVERTICAL);

    auto* box_sizer_3 = new wxBoxSizer(wxVERTICAL);
    box_sizer->Add(box_sizer_3, wxSizerFlags().Border(wxALL));

    auto* staticText_3 = new wxStaticText(this, wxID_ANY, "These are initial values -- all of them can be changed after the ribbon bar is created.");
    staticText_3->Wrap(300);
    box_sizer_3->Add(staticText_3, wxSizerFlags().Border(wxALL));

    m_infoBar = new wxInfoBar(this);
    m_infoBar->SetShowHideEffects(wxSHOW_EFFECT_NONE, wxSHOW_EFFECT_NONE);
    box_sizer_3->Add(m_infoBar, wxSizerFlags().Expand().Border(wxALL));

    m_class_sizer = new wxBoxSizer(wxHORIZONTAL);
    box_sizer->Add(m_class_sizer, wxSizerFlags().Expand().Border(wxALL));

    auto* staticText = new wxStaticText(this, wxID_ANY, "&Base class name:");
    staticText->SetToolTip("Change this to something unique to your project.");
    m_class_sizer->Add(staticText, wxSizerFlags().Center().Border(wxALL));

    m_classname = new wxTextCtrl(this, wxID_ANY, "MyRibbonBarBase");
    m_classname->SetValidator(wxTextValidator(wxFILTER_NONE, &m_base_class));
    m_classname->SetToolTip("Change this to something unique to your project.");
    m_class_sizer->Add(m_classname, wxSizerFlags(1).Border(wxALL));

    auto* box_sizer_2 = new wxBoxSizer(wxHORIZONTAL);
    box_sizer->Add(box_sizer_2, wxSizerFlags().Border(wxALL));

    m_staticText = new wxStaticText(this, wxID_ANY, "&Type:");
    box_sizer_2->Add(m_staticText, wxSizerFlags().Center().Border(wxALL));

    m_comboBox = new wxComboBox(this, wxID_ANY, wxEmptyString);
    m_comboBox->Append("Tool");
    m_comboBox->Append("Button");
    m_comboBox->Append("Gallery");
    m_panel_type = "Tool";  // set validator variable
    m_comboBox->SetValidator(wxGenericValidator(&m_panel_type));
    box_sizer_2->Add(m_comboBox, wxSizerFlags().Border(wxALL));

    auto* box_sizer_4 = new wxBoxSizer(wxHORIZONTAL);
    box_sizer->Add(box_sizer_4, wxSizerFlags().Border(wxALL));

    auto* staticText_4 = new wxStaticText(this, wxID_ANY, "&Pages:");
    box_sizer_4->Add(staticText_4, wxSizerFlags().Center().Border(wxLEFT|wxTOP|wxBOTTOM, wxSizerFlags::GetDefaultBorder()));

    m_spinCtrlPages = new wxSpinCtrl(this, wxID_ANY, wxEmptyString,
            wxDefaultPosition, wxDefaultSize, wxSP_ARROW_KEYS, 1, 7, 3);
    m_spinCtrlPages->SetValidator(wxGenericValidator(&m_num_pages));
    box_sizer_4->Add(m_spinCtrlPages, wxSizerFlags().Border(wxALL));

    auto* stdBtn = CreateStdDialogButtonSizer(wxOK|wxCANCEL);
    box_sizer->Add(CreateSeparatedSizer(stdBtn), wxSizerFlags().Expand().Border(wxALL));

    SetSizerAndFit(box_sizer);
    Centre(wxBOTH);

    // Event handlers
    Bind(wxEVT_INIT_DIALOG, &NewRibbon::OnInit, this);
    m_classname->Bind(wxEVT_TEXT,
        [this](wxCommandEvent&)
        {VerifyClassName();
        } );

    return true;
}
