/////////////////////////////////////////////////////////////////////////////
// Purpose:   Contains various images handling functions
// Author:    Ralph Walden
// Copyright: Copyright (c) 2020-2021 KeyWorks Software (Ralph Walden)
// License:   Apache License -- see ../LICENSE
/////////////////////////////////////////////////////////////////////////////

#pragma once

#include <wx/mstream.h>  // Memory stream classes

class wxAnimation;
class wxIcon;
class wxImage;

// This is the size of component images
constexpr const int GenImageSize = 22;

// This will first look for an associated PNG Header array, and if unavailable an XPM file.
// If neither can be found, a 16x16 question mark image is returned.
wxImage GetInternalImage(ttlib::sview name);

// If this is a PNG Header file, the alpha channel will be converted to a mask
wxIcon GetIconImage(ttlib::sview name);

// Converts the ASCII header file into binary data and loads it as an image. It's designed to
// read header files created by wxUiEditor or wxFormBuilder -- any other generated header
// file might or might not work.
wxImage GetHeaderImage(ttlib::sview filename, size_t* p_original_size = nullptr, ttString* p_mime_type = nullptr);

// Converts the ASCII header file into binary data and loads it as an animation. It's designed to
// read header files created by wxUiEditor or wxFormBuilder -- any other generated header
// file might or might not work.
bool GetAnimationImage(wxAnimation& animation, ttlib::sview filename);

wxAnimation LoadAnimationImage(wxAnimation& animation, const unsigned char* data, size_t size_data);

// Convert a data array into a wxImage
inline wxImage GetImageFromArray(const unsigned char* data, size_t size_data)
{
    wxMemoryInputStream strm(data, size_data);
    wxImage image;
    image.LoadFile(strm);
    return image;
};

// This is almost identical to GetImageFromArray() -- the only difference is that this one
// first tries to load the image via the PNG handler.
wxImage LoadHeaderImage(const unsigned char* data, size_t size_data);
