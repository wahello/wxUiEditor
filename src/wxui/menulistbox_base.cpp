////////////////////////////////////////////////////////////////////////////////
// Code generated by wxUiEditor -- see https://github.com/KeyWorksRW/wxUiEditor/
//
// DO NOT EDIT THIS FILE! Your changes will be lost if it is re-generated!
////////////////////////////////////////////////////////////////////////////////

#include "ui_images.h"

#include "ribbon_ids.h"
#include "gen_enums.h"     // Enumerations for generators

using namespace GenEnum;

#include "menulistbox_base.h"

namespace wxue_img
{
    extern const unsigned char wxCheckListBox_png[498];
    extern const unsigned char wxEditListBox_png[707];
    extern const unsigned char wxListBox_png[310];
    extern const unsigned char wxListView_png[356];
    extern const unsigned char wxRearrangeCtrl_png[332];
    extern const unsigned char wxSimpleHtmlListBox_png[676];
}

MenuListbox::MenuListbox() : wxMenu()
{
    if (!wxImage::FindHandler(wxBITMAP_TYPE_PNG))
        wxImage::AddHandler(new wxPNGHandler);

    auto* menu_item = Append(gen_wxListBox, "Insert wxListBox");
    menu_item->SetBitmap(wxue_img::bundle_wxListBox_png());

    auto* menu_item_2 = Append(gen_wxCheckListBox, "Insert wxCheckListBox");
    menu_item_2->SetBitmap(wxue_img::bundle_wxCheckListBox_png());

    auto* menu_item_6 = Append(gen_wxEditableListBox, "Insert wxEditableListBox");
    menu_item_6->SetBitmap(wxue_img::bundle_wxEditListBox_png());

    auto* menu_item_3 = Append(gen_wxListView, "Insert wxListView");
    menu_item_3->SetBitmap(wxue_img::bundle_wxListView_png());

    auto* menu_item_4 = Append(gen_wxRearrangeCtrl, "Insert wxRearrangeCtrl");
    menu_item_4->SetBitmap(wxue_img::bundle_wxRearrangeCtrl_png());

    auto* menu_item_5 = Append(gen_wxSimpleHtmlListBox, "Insert wxSimpleHtmlListBox");
    menu_item_5->SetBitmap(wxue_img::bundle_wxSimpleHtmlListBox_png());
}
