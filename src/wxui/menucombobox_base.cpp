////////////////////////////////////////////////////////////////////////////////
// Code generated by wxUiEditor -- see https://github.com/KeyWorksRW/wxUiEditor/
//
// DO NOT EDIT THIS FILE! Your changes will be lost if it is re-generated!
////////////////////////////////////////////////////////////////////////////////

#include "ui_images.h"

#include "ribbon_ids.h"
#include "gen_enums.h"     // Enumerations for generators

using namespace GenEnum;

#include "menucombobox_base.h"

namespace wxue_img
{
    extern const unsigned char bmp_combo_box_png[492];
    extern const unsigned char wxChoice_png[330];
    extern const unsigned char wxComboBox_png[233];
}

MenuCombobox::MenuCombobox() : wxMenu()
{
    if (!wxImage::FindHandler(wxBITMAP_TYPE_PNG))
        wxImage::AddHandler(new wxPNGHandler);

    auto* menu_item = Append(gen_wxComboBox, "Insert wxComboBox");
    menu_item->SetBitmap(wxue_img::bundle_wxComboBox_png());

    auto* menu_item_2 = Append(gen_wxChoice, "Insert wxChoice");
    menu_item_2->SetBitmap(wxue_img::bundle_wxChoice_png());

    auto* menu_item_3 = Append(gen_wxBitmapComboBox, "Insert wxBitmapComboBox");
    menu_item_3->SetBitmap(wxue_img::bundle_bmp_combo_box_png());
}
