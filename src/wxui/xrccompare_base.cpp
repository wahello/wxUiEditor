////////////////////////////////////////////////////////////////////////////////
// Code generated by wxUiEditor -- see https://github.com/KeyWorksRW/wxUiEditor/
//
// DO NOT EDIT THIS FILE! Your changes will be lost if it is re-generated!
////////////////////////////////////////////////////////////////////////////////

#include <wx/colour.h>
#include <wx/font.h>
#include <wx/settings.h>
#include <wx/sizer.h>
#include <wx/stattext.h>

#include "xrccompare_base.h"

bool XrcCompareBase::Create(wxWindow* parent, wxWindowID id, const wxString& title,
        const wxPoint& pos, const wxSize& size, long style, const wxString &name)
{
    if (!wxDialog::Create(parent, id, title, pos, size, style, name))
        return false;

    m_grid_bag_sizer = new wxGridBagSizer();

    auto* box_sizer = new wxBoxSizer(wxVERTICAL);
    m_grid_bag_sizer->Add(box_sizer, wxGBPosition(0, 0), wxGBSpan(1, 1), wxTOP|wxRIGHT|wxLEFT, 5);

    auto* staticText = new wxStaticText(this, wxID_ANY, "C++ Generated");
    {
        wxFontInfo font_info(9);
        font_info.Underlined();
        staticText->SetFont(wxFont(font_info));
    }
    staticText->SetForegroundColour(wxColour(255, 0, 0));
    box_sizer->Add(staticText, wxSizerFlags().Border(wxALL));

    m_static_line = new wxStaticLine(this, wxID_ANY, wxDefaultPosition, ConvertDialogToPixels(wxSize(-1, 100)),
        wxLI_VERTICAL);
    m_grid_bag_sizer->Add(m_static_line, wxGBPosition(0, 1), wxGBSpan(3, 1), wxALL, 5);

    auto* box_sizer_2 = new wxBoxSizer(wxVERTICAL);
    m_grid_bag_sizer->Add(box_sizer_2, wxGBPosition(0, 2), wxGBSpan(1, 1), wxTOP|wxRIGHT|wxLEFT, 5);

    auto* staticText_2 = new wxStaticText(this, wxID_ANY, "XRC Generated");
    {
        wxFontInfo font_info(9);
        font_info.Underlined();
        staticText_2->SetFont(wxFont(font_info));
    }
    staticText_2->SetForegroundColour(wxColour(0, 128, 0));
    box_sizer_2->Add(staticText_2, wxSizerFlags().Border(wxALL));

    SetSizerAndFit(m_grid_bag_sizer);
    Centre(wxBOTH);

    return true;
}
