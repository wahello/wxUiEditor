////////////////////////////////////////////////////////////////////////////////
// Code generated by wxUiEditor -- see https://github.com/KeyWorksRW/wxUiEditor/
//
// DO NOT EDIT THIS FILE! Your changes will be lost if it is re-generated!
////////////////////////////////////////////////////////////////////////////////

#include <wx/bitmap.h>
#include <wx/icon.h>
#include <wx/image.h>

#include "ribbon_ids.h"
#include "gen_enums.h"     // Enumerations for generators

using namespace GenEnum;

#include "menubutton_base.h"

#include <wx/mstream.h>  // Memory stream classes

// Convert a data array into a wxImage
inline wxImage GetImageFromArray(const unsigned char* data, size_t size_data)
{
    wxMemoryInputStream strm(data, size_data);
    wxImage image;
    image.LoadFile(strm);
    return image;
};

namespace wxue_img
{
    extern const unsigned char stddialogbuttonsizer_png[540];
    extern const unsigned char toggle_button_png[293];
    extern const unsigned char wxButton_png[516];
    extern const unsigned char wxCommandLinkButton_png[598];
}

MenuButton::MenuButton() : wxMenu()
{
    if (!wxImage::FindHandler(wxBITMAP_TYPE_PNG))
        wxImage::AddHandler(new wxPNGHandler);

    auto menu_item = Append(gen_wxButton, wxString::FromUTF8("Insert wxButton"));
    menu_item->SetBitmap(GetImageFromArray(wxue_img::wxButton_png, sizeof(wxue_img::wxButton_png)));

    auto menu_item_2 = Append(gen_wxToggleButton, wxString::FromUTF8("Insert wxToggleButton"));
    menu_item_2->SetBitmap(GetImageFromArray(wxue_img::toggle_button_png, sizeof(wxue_img::toggle_button_png)));

    auto menu_item_3 = Append(gen_wxStdDialogButtonSizer, wxString::FromUTF8("Insert wxStdDialogButtonSizer"));
    menu_item_3->SetBitmap(GetImageFromArray(wxue_img::stddialogbuttonsizer_png, sizeof(wxue_img::stddialogbuttonsizer_png)));

    auto menu_item_4 = Append(gen_wxCommandLinkButton, wxString::FromUTF8("Insert wxCommandLinkButton"));
    menu_item_4->SetBitmap(GetImageFromArray(wxue_img::wxCommandLinkButton_png, sizeof(wxue_img::wxCommandLinkButton_png)));
}

namespace wxue_img
{

    const unsigned char stddialogbuttonsizer_png[540] {
    137,80,78,71,13,10,26,10,0,0,0,13,73,72,68,82,0,0,0,22,0,0,0,22,8,6,0,0,0,196,180,108,59,0,0,0,4,115,66,73,84,8,8,8,8,124,8,100,136,
    0,0,0,9,112,72,89,115,0,0,10,240,0,0,10,240,1,66,172,52,152,0,0,1,190,73,68,65,84,56,203,237,148,177,107,83,81,24,197,127,175,94,219,
    64,95,104,130,84,67,187,88,49,144,98,44,116,104,160,130,131,133,118,16,42,8,175,142,117,113,84,23,41,14,226,31,160,69,90,112,82,176,
    75,161,83,193,193,209,193,77,155,150,180,5,111,37,137,130,73,85,26,26,209,84,243,242,210,251,158,92,151,244,181,37,104,22,223,32,244,
    76,151,203,225,199,199,249,206,189,112,164,255,86,134,6,29,8,57,8,176,6,45,0,44,203,250,43,60,155,205,34,165,52,146,201,164,78,36,18,
    180,242,34,37,2,96,110,238,25,245,154,13,6,251,243,31,56,223,153,186,139,148,146,84,42,197,236,236,76,75,175,15,118,170,59,148,62,189,
    231,93,238,3,199,58,79,208,27,57,142,50,58,112,156,93,150,150,150,49,12,3,0,211,52,113,170,59,20,242,171,60,120,248,148,209,201,219,
    140,13,197,15,47,173,225,21,0,174,171,40,228,54,248,90,107,199,206,101,248,222,107,242,121,219,227,252,192,0,74,185,104,189,159,148,
    235,42,94,191,89,227,222,244,99,132,87,109,206,183,225,21,123,23,149,31,53,226,253,231,112,188,45,214,191,213,248,248,197,225,202,120,
    140,213,116,115,142,158,48,137,132,0,76,30,221,191,65,124,108,146,151,79,230,137,93,184,228,123,124,240,169,238,40,107,153,52,63,203,
    187,196,207,118,209,41,194,164,51,27,45,27,16,141,68,41,109,149,24,28,185,74,40,84,33,191,125,0,92,220,44,210,222,21,163,175,71,208,
    118,250,12,225,80,27,39,123,4,74,41,10,249,95,62,196,182,109,138,155,69,168,87,168,212,65,148,87,48,251,47,35,188,50,30,97,160,218,60,
    241,194,226,139,63,247,178,145,155,82,10,128,225,193,62,166,110,222,98,226,250,53,94,61,95,224,226,248,168,15,218,243,26,26,244,132,
    101,241,47,123,252,86,202,224,94,94,112,127,197,145,2,215,111,160,196,192,217,55,22,50,89,0,0,0,0,73,69,78,68,174,66,96,130
    };

    const unsigned char toggle_button_png[293] {
    137,80,78,71,13,10,26,10,0,0,0,13,73,72,68,82,0,0,0,22,0,0,0,22,8,6,0,0,0,196,180,108,59,0,0,0,4,115,66,73,84,8,8,8,8,124,8,100,136,
    0,0,0,9,112,72,89,115,0,0,10,240,0,0,10,240,1,66,172,52,152,0,0,0,199,73,68,65,84,56,203,99,96,24,5,163,0,29,48,158,191,249,230,191,
    97,212,90,170,26,122,126,89,48,3,35,131,241,204,255,167,119,164,81,213,96,83,143,89,12,76,12,12,12,12,127,254,99,199,166,162,140,12,
    166,162,140,40,124,116,121,116,61,172,76,16,195,89,24,24,24,24,56,153,49,109,53,16,102,100,184,240,246,63,6,219,82,12,193,198,166,23,
    198,199,105,48,186,56,140,125,243,195,127,6,117,1,70,134,155,31,254,99,53,152,139,5,201,96,24,7,29,32,139,195,216,184,104,24,32,24,20,
    184,92,204,201,204,192,240,230,251,127,6,17,78,70,172,46,102,65,54,152,3,139,139,191,252,254,207,192,195,202,8,103,195,0,76,45,76,158,
    3,135,111,89,24,24,24,24,216,152,176,75,254,250,251,159,36,62,93,50,200,40,24,5,152,0,0,191,144,85,198,46,125,47,182,0,0,0,0,73,69,78,
    68,174,66,96,130
    };

    const unsigned char wxButton_png[516] {
    137,80,78,71,13,10,26,10,0,0,0,13,73,72,68,82,0,0,0,22,0,0,0,22,8,6,0,0,0,196,180,108,59,0,0,0,4,115,66,73,84,8,8,8,8,124,8,100,136,
    0,0,0,9,112,72,89,115,0,0,10,240,0,0,10,240,1,66,172,52,152,0,0,1,166,73,68,65,84,56,203,237,148,63,72,91,113,16,199,63,63,243,30,13,
    1,33,1,229,97,213,162,17,74,16,83,11,14,133,130,109,186,57,21,196,108,14,14,85,236,80,28,59,57,4,92,4,39,17,218,165,131,142,106,218,
    60,161,197,12,29,94,4,81,81,252,243,180,162,136,74,82,147,208,36,24,37,26,212,52,121,29,172,80,225,5,28,210,33,224,119,60,238,62,119,
    112,247,61,184,87,217,74,0,24,134,97,148,12,40,132,0,16,55,208,31,27,75,37,1,251,213,32,62,159,79,72,69,161,217,20,27,219,251,156,100,
    115,200,246,26,220,143,27,169,180,230,73,134,35,100,172,10,78,197,198,73,236,144,221,99,153,103,45,143,110,149,122,60,30,163,194,180,
    109,54,69,104,114,156,113,109,139,68,50,206,202,247,0,159,190,233,192,25,107,179,95,8,44,69,56,79,134,153,9,4,208,195,167,166,8,201,
    44,24,221,91,96,62,225,160,239,77,15,205,85,22,114,201,53,134,7,103,89,126,253,246,58,225,60,133,58,185,128,220,234,165,251,185,211,
    20,108,58,113,58,22,229,129,226,162,190,74,2,4,114,181,147,39,74,156,157,3,128,11,118,213,49,116,71,39,157,237,77,72,22,113,119,112,
    5,130,220,213,5,185,194,223,64,225,55,87,121,43,54,27,128,140,210,234,198,216,12,177,253,235,146,66,145,37,154,130,31,186,220,72,49,
    29,253,103,6,184,228,104,53,196,220,113,35,109,245,0,22,236,45,94,250,95,202,124,158,152,98,39,157,189,59,216,94,247,20,239,139,90,102,
    62,12,241,110,224,61,35,95,163,116,244,118,209,240,79,78,195,43,47,29,117,105,62,142,78,23,55,200,214,250,98,201,12,226,87,131,104,154,
    118,125,21,126,53,248,127,44,125,115,212,165,130,106,154,38,238,191,101,25,235,15,10,55,147,200,59,100,25,65,0,0,0,0,73,69,78,68,174,
    66,96,130
    };

    const unsigned char wxCommandLinkButton_png[598] {
    137,80,78,71,13,10,26,10,0,0,0,13,73,72,68,82,0,0,0,22,0,0,0,22,8,6,0,0,0,196,180,108,59,0,0,0,4,115,66,73,84,8,8,8,8,124,8,100,136,
    0,0,0,9,112,72,89,115,0,0,10,240,0,0,10,240,1,66,172,52,152,0,0,1,248,73,68,65,84,56,203,237,148,75,75,148,97,24,134,175,247,48,51,159,
    51,19,54,149,134,49,72,24,90,89,155,232,176,30,8,90,212,42,164,8,198,77,25,65,16,228,186,213,252,129,126,65,86,127,64,193,77,100,171,
    6,55,89,139,200,24,67,38,176,100,102,96,14,169,152,205,248,241,126,239,231,219,34,148,72,99,92,184,17,188,151,207,226,226,57,220,247,
    3,7,218,183,18,0,206,57,183,103,64,33,4,128,216,132,206,205,190,7,224,68,239,0,97,24,98,173,37,8,2,28,142,134,108,48,246,229,25,107,
    230,39,247,206,221,103,240,240,32,30,222,38,136,242,247,249,45,240,248,228,20,185,92,78,232,191,161,192,22,208,24,131,239,251,56,225,
    104,168,58,51,213,119,124,93,46,82,92,41,146,61,155,101,168,239,22,73,153,100,167,97,51,153,140,211,255,22,141,49,56,28,129,12,176,17,
    75,104,45,61,209,30,178,167,135,121,49,55,198,231,31,179,212,63,214,40,255,170,144,29,24,38,221,145,222,113,37,219,192,50,34,41,217,
    10,19,165,9,234,173,58,81,25,69,9,69,115,163,137,211,10,167,37,21,191,202,243,249,151,44,54,75,140,12,142,144,166,179,61,120,77,54,201,
    45,60,229,77,237,237,246,54,226,64,60,5,192,42,240,218,124,32,82,75,241,196,187,219,30,172,125,205,163,190,7,116,29,235,165,106,151,
    136,139,24,10,133,47,12,115,102,129,146,169,178,129,163,67,196,184,28,63,207,141,163,215,161,182,139,85,172,175,54,57,41,143,51,26,203,
    18,68,44,214,88,116,50,74,193,91,164,220,26,167,108,90,28,145,9,174,117,92,225,97,226,38,253,46,205,18,223,218,131,99,177,24,190,239,
    19,46,27,140,239,131,18,172,116,91,198,214,95,81,180,101,78,69,211,12,197,51,220,137,92,165,43,232,68,35,119,62,158,16,66,20,62,205,
    108,121,38,145,72,160,181,70,107,141,82,10,39,32,84,142,110,149,226,146,119,134,199,135,110,115,113,163,31,207,215,72,9,66,137,255,187,
    98,124,114,170,109,162,46,252,185,28,5,166,41,48,189,187,72,111,154,122,175,98,157,207,231,197,193,183,220,199,250,13,35,117,201,26,
    23,87,197,65,0,0,0,0,73,69,78,68,174,66,96,130
    };

}
