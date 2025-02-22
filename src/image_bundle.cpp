/////////////////////////////////////////////////////////////////////////////
// Purpose:   Functions for working with wxBitmapBundle
// Author:    Ralph Walden
// Copyright: Copyright (c) 2022 KeyWorks Software (Ralph Walden)
// License:   Apache License -- see ../LICENSE
/////////////////////////////////////////////////////////////////////////////

#include <filesystem>
#include <fstream>
#include <sstream>
#include <thread>

#include <wx/artprov.h>   // wxArtProvider class
#include <wx/bmpbndl.h>   // includes wx/bitmap.h, wxBitmapBundle class interface
#include <wx/mstream.h>   // Memory stream classes
#include <wx/wfstream.h>  // File stream classes
#include <wx/zstream.h>   // zlib stream classes

#include "pugixml.hpp"  // xml parser

#include "node.h"           // Node class
#include "project_class.h"  // Project class
#include "utils.h"          // Utility functions that work with properties

// Note that we do *not* support @1_5x or @2x as suffixes. Since these suffixes will become part of the string name when
// converted to an embedded character array, the compiler will not accept the '@' character. We could of course change it,
// but then we don't know if it's unique if there is an actual filename that used a leading suffix '_' character instead of a
// leading '@'.

const char* suffixes[] = {
    "_1_5x",
    "_2x",
    "@1_5x",
    "@2x",
};

inline ttlib::cstr ConvertToLookup(const ttlib::cstr& description)
{
    ttlib::multiview parts(description, ';', tt::TRIM::both);
    ASSERT(parts.size() > 1)

    ttlib::cstr lookup_str;
    lookup_str << parts[0] << ';' << parts[1].filename();
    return lookup_str;
}

static bool CopyStreamData(wxInputStream* inputStream, wxOutputStream* outputStream, size_t size)
{
    size_t buf_size;
    if (size == tt::npos || size > (64 * 1024))
        buf_size = (64 * 1024);
    else
        buf_size = size;

    auto read_buf = std::make_unique<unsigned char[]>(buf_size);
    auto read_size = buf_size;

    size_t copied_data = 0;
    for (;;)
    {
        if (size != tt::npos && copied_data + read_size > size)
            read_size = size - copied_data;
        inputStream->Read(read_buf.get(), read_size);

        auto actually_read = inputStream->LastRead();
        outputStream->Write(read_buf.get(), actually_read);
        if (outputStream->LastWrite() != actually_read)
        {
            return false;
        }

        if (size == tt::npos)
        {
            if (inputStream->Eof())
                break;
        }
        else
        {
            copied_data += actually_read;
            if (copied_data >= size)
                break;
        }
    }

    return true;
}

wxBitmapBundle LoadSVG(EmbeddedImage* embed, ttlib::sview size_description)
{
    size_t org_size = (embed->array_size >> 32);
    auto str = std::make_unique<char[]>(org_size);
    wxMemoryInputStream stream_in(embed->array_data.get(), embed->array_size & 0xFFFFFFFF);
    wxZlibInputStream zlib_strm(stream_in);
    zlib_strm.Read(str.get(), org_size);
    return wxBitmapBundle::FromSVG(str.get(), get_image_prop_size(size_description));
}

void Project::CollectBundles()
{
    std::vector<Node*> forms;
    GetProject()->CollectForms(forms);

    for (const auto& form: forms)
    {
        CollectNodeBundles(form, form);

        if (form->HasProp(prop_icon) && form->HasValue(prop_icon))
        {
            if (!m_bundles.contains(ConvertToLookup(form->prop_as_string(prop_icon))))
            {
                ProcessBundleProperty(form->prop_as_string(prop_icon), form);
            }
        }
    }
}

void Project::CollectNodeBundles(Node* node, Node* form)
{
    for (auto& iter: node->get_props_vector())
    {
        if (!iter.HasValue())
            continue;

        if (iter.type() == type_image)
        {
            if (!m_bundles.contains(ConvertToLookup(iter.as_string())))
            {
                ProcessBundleProperty(iter.as_string(), form);
            }
        }
        else if (iter.type() == type_animation)
        {
            auto& value = iter.as_string();
            if (value.starts_with("Embed"))
            {
                ttlib::multiview parts(value, BMP_PROP_SEPARATOR, tt::TRIM::both);
                if (parts[IndexImage].size())
                {
                    if (!m_map_embedded.contains(parts[IndexImage].filename()))
                    {
                        AddEmbeddedImage(parts[IndexImage], form);
                    }
                }
            }
        }
    }

    for (const auto& child: node->GetChildNodePtrs())
    {
        CollectNodeBundles(child.get(), form);
    }
}

bool Project::AddNewEmbeddedBundle(const ttlib::multistr& parts, ttlib::cstr path, Node* form)
{
    ASSERT(parts.size() > 1)

    ttlib::cstr lookup_str;
    lookup_str << parts[0] << ';' << parts[1].filename();

    ImageBundle img_bundle;

    if (!path.file_exists())
    {
        if (GetProject()->HasValue(prop_art_directory))
        {
            ttlib::cstr art_path = GetProject()->prop_as_string(prop_art_directory);
            art_path.append_filename(path);
            if (!art_path.file_exists())
            {
                m_bundles[lookup_str] = img_bundle;
                return true;
            }
            path = std::move(art_path);
        }
        else
        {
            m_bundles[lookup_str] = img_bundle;
            return true;
        }
    }

    if (parts[IndexType].starts_with("SVG"))
    {
        if (AddSvgBundleImage(path, form))
        {
            img_bundle.lst_filenames.emplace_back(path);
            if (auto embed = GetEmbeddedImage(path); embed)
            {
                img_bundle.bundle = LoadSVG(embed, parts[IndexSize]);
                m_bundles[lookup_str] = std::move(img_bundle);
                return true;
            }
        }
    }

    if (!AddEmbeddedBundleImage(path, form))
    {
        return false;
    }

    img_bundle.lst_filenames.emplace_back(path);

    /*

        Look for suffix combinations -- it's fine if one of them doesn't exist

            _16x16, _24x24, _32x32
            _24x24, _36x36, _48x48
            any, _1_5x, _2x

    */

    if (auto pos = path.find_last_of('.'); ttlib::is_found(pos))
    {
        if (path.contains("_16x16."))
        {
            path.Replace("_16x16.", "_24x24.");
            if (path.file_exists())
            {
                if (auto added = AddEmbeddedBundleImage(path, form); added)
                {
                    img_bundle.lst_filenames.emplace_back(path);
                }
            }
            path.Replace("_24x24.", "_32x32.");
            if (path.file_exists())
            {
                if (auto added = AddEmbeddedBundleImage(path, form); added)
                {
                    img_bundle.lst_filenames.emplace_back(path);
                }
            }
        }
        else if (path.contains("_24x24."))
        {
            path.Replace("_24x24.", "_36x36.");
            if (path.file_exists())
            {
                if (auto added = AddEmbeddedBundleImage(path, form); added)
                {
                    img_bundle.lst_filenames.emplace_back(path);
                }
            }
            path.Replace("_36x36.", "_48x48.");
            if (path.file_exists())
            {
                if (auto added = AddEmbeddedBundleImage(path, form); added)
                {
                    img_bundle.lst_filenames.emplace_back(path);
                }
            }
        }
        else
        {
            ttlib::cstr additional_path;
            for (auto& iter: suffixes)
            {
                additional_path = path;
                additional_path.insert(pos, iter);
                if (additional_path.file_exists())
                {
                    if (auto added = AddEmbeddedBundleImage(additional_path, form); added)
                    {
                        img_bundle.lst_filenames.emplace_back(additional_path);
                    }
                }
            }
        }
    }

    if (img_bundle.lst_filenames.size() == 1)
    {
        if (auto embed = GetEmbeddedImage(img_bundle.lst_filenames[0]); embed)
        {
            wxMemoryInputStream stream(embed->array_data.get(), embed->array_size);
            wxImage image;
            image.LoadFile(stream);
            img_bundle.bundle = wxBitmapBundle::FromBitmap(image);
        }
    }
    else
    {
        wxVector<wxBitmap> bitmaps;
        for (auto& iter: img_bundle.lst_filenames)
        {
            if (auto embed = GetEmbeddedImage(iter); embed)
            {
                wxMemoryInputStream stream(embed->array_data.get(), embed->array_size);
                wxImage image;
                image.LoadFile(stream);
                ASSERT(image.IsOk())
                bitmaps.push_back(image);
            }
        }
        img_bundle.bundle = wxBitmapBundle::FromBitmaps(bitmaps);
    }

    m_bundles[lookup_str] = std::move(img_bundle);
    return true;
}

bool Project::AddEmbeddedBundleImage(ttlib::cstr path, Node* form)
{
    wxFFileInputStream stream(path.wx_str());
    if (!stream.IsOk())
    {
        return false;
    }
    wxImageHandler* handler;
    auto& list = wxImage::GetHandlers();
    for (auto node = list.GetFirst(); node; node = node->GetNext())
    {
        handler = (wxImageHandler*) node->GetData();
        if (handler->CanRead(stream))
        {
            wxImage image;
            if (handler->LoadFile(&image, stream))
            {
                m_map_embedded[path.filename().as_str()] = std::make_unique<EmbeddedImage>();
                auto embed = m_map_embedded[path.filename().as_str()].get();
                InitializeArrayName(embed, path.filename());
                embed->form = form;

                // If possible, convert the file to a PNG -- even if the original file is a PNG, since we might end up with
                // better compression.

                if (isConvertibleMime(handler->GetMimeType()))
                {
                    embed->type = wxBITMAP_TYPE_PNG;

                    wxMemoryOutputStream save_stream;

                    // Maximize compression
                    image.SetOption(wxIMAGE_OPTION_PNG_COMPRESSION_LEVEL, 9);
                    image.SetOption(wxIMAGE_OPTION_PNG_COMPRESSION_MEM_LEVEL, 9);
                    image.SaveFile(save_stream, "image/png");

                    auto read_stream = save_stream.GetOutputStreamBuffer();
                    stream.SeekI(0);
                    if (read_stream->GetBufferSize() <= (to_size_t) stream.GetLength())
                    {
                        embed->array_size = read_stream->GetBufferSize();
                        embed->array_data = std::make_unique<unsigned char[]>(embed->array_size);
                        memcpy(embed->array_data.get(), read_stream->GetBufferStart(), embed->array_size);
                    }
                    else
                    {
#if defined(_DEBUG) || defined(INTERNAL_TESTING)
                        size_t org_size = (to_size_t) stream.GetLength();
                        auto png_size = read_stream->GetBufferSize();
                        ttlib::cstr size_comparison;
                        size_comparison.Format("Original: %ku, new: %ku", org_size, png_size);
#endif

                        embed->type = handler->GetType();
                        embed->array_size = stream.GetSize();
                        embed->array_data = std::make_unique<unsigned char[]>(embed->array_size);
                        stream.Read(embed->array_data.get(), embed->array_size);
                    }
                }
                else
                {
                    embed->type = handler->GetType();

                    stream.SeekI(0);
                    embed->array_size = stream.GetSize();
                    embed->array_data = std::make_unique<unsigned char[]>(embed->array_size);
                    stream.Read(embed->array_data.get(), embed->array_size);
                }

                return true;
            }
        }
    }
    return false;
}

ImageBundle* Project::ProcessBundleProperty(const ttlib::multistr& parts, Node* node)
{
    ASSERT(parts.size() > 1)

    ttlib::cstr lookup_str;
    lookup_str << parts[0] << ';' << parts[1].filename();

    ASSERT_MSG(!m_bundles.contains(lookup_str), "ProcessBundleProperty should not be called if bundle already exists!")

    if (parts[IndexImage].empty())
    {
        return nullptr;
    }

    ImageBundle img_bundle;

    if (parts[IndexType].contains("Art"))
    {
        if (parts[IndexArtID].contains("|"))
        {
            ttlib::multistr id_client(parts[IndexArtID], '|');
            img_bundle.bundle = wxArtProvider::GetBitmapBundle(id_client[0], wxART_MAKE_CLIENT_ID_FROM_STR(id_client[1]));
        }
        else
        {
            img_bundle.bundle =
                wxArtProvider::GetBitmapBundle(parts[IndexArtID].wx_str(), wxART_MAKE_CLIENT_ID_FROM_STR("wxART_OTHER"));
        }

        m_bundles[lookup_str] = std::move(img_bundle);
        return &m_bundles[lookup_str];
    }
    else if (parts[IndexType].contains("Embed"))
    {
        if (AddNewEmbeddedBundle(parts, parts[IndexImage], node->get_form()))
        {
            return &m_bundles[lookup_str];
        }
        else
        {
            return nullptr;
        }
    }
    else if (parts[IndexType].contains("SVG"))
    {
        if (AddNewEmbeddedBundle(parts, parts[IndexImage], node->get_form()))
        {
            return &m_bundles[lookup_str];
        }
        else
        {
            return nullptr;
        }
    }

    auto image_first = GetProject()->GetPropertyBitmap(parts, false);
    if (!image_first.IsOk())
    {
        return nullptr;
    }

    img_bundle.lst_filenames.emplace_back(parts[IndexImage]);

    if (auto pos = parts[IndexImage].find_last_of('.'); ttlib::is_found(pos))
    {
        if (parts[IndexImage].contains("_16x16."))
        {
            ttlib::cstr path(parts[IndexImage]);
            path.Replace("_16x16.", "_24x24.");
            if (!path.file_exists())
            {
                if (GetProject()->HasValue(prop_art_directory))
                {
                    path = GetProject()->prop_as_string(prop_art_directory);
                    path.append_filename(parts[IndexImage]);
                    path.Replace("_16x16.", "_24x24.");
                    if (path.file_exists())
                    {
                        img_bundle.lst_filenames.emplace_back(path);
                    }
                }
            }
            else
            {
                img_bundle.lst_filenames.emplace_back(path);
            }

            // Note that path may now contain the prop_art_directory prefix
            path.Replace("_24x24.", "_32x32.");
            if (path.file_exists())
            {
                img_bundle.lst_filenames.emplace_back(path);
            }
        }
        else if (parts[IndexImage].contains("_24x24."))
        {
            ttlib::cstr path(parts[IndexImage]);
            path.Replace("_24x24.", "_36x36.");
            if (!path.file_exists())
            {
                if (GetProject()->HasValue(prop_art_directory))
                {
                    path = GetProject()->prop_as_string(prop_art_directory);
                    path.append_filename(parts[IndexImage]);
                    path.Replace("_24x24.", "_36x36.");
                    if (path.file_exists())
                    {
                        img_bundle.lst_filenames.emplace_back(path);
                    }
                }
            }
            else
            {
                img_bundle.lst_filenames.emplace_back(path);
            }

            // Note that path may now contain the prop_art_directory prefix
            path.Replace("_36x36.", "_48x48.");
            if (path.file_exists())
            {
                img_bundle.lst_filenames.emplace_back(path);
            }
        }
        else
        {
            ttlib::cstr path;
            for (auto& iter: suffixes)
            {
                path = parts[IndexImage];
                path.insert(pos, iter);
                if (!path.file_exists())
                {
                    if (GetProject()->HasValue(prop_art_directory))
                    {
                        ttlib::cstr tmp_path = GetProject()->prop_as_string(prop_art_directory);
                        tmp_path.append_filename(path);
                        if (tmp_path.file_exists())
                        {
                            img_bundle.lst_filenames.emplace_back(tmp_path);
                        }
                    }
                }
                else
                {
                    img_bundle.lst_filenames.emplace_back(path);
                }
            }
        }
    }

    ASSERT_MSG(img_bundle.lst_filenames.size() > 0, "image_first must always have it's filename added.")

    if (img_bundle.lst_filenames.size() == 1)
    {
        img_bundle.bundle = wxBitmapBundle::FromBitmap(image_first);
    }
    else
    {
        wxVector<wxBitmap> bitmaps;
        bitmaps.push_back(image_first);
        ttlib::cstr new_description;
        new_description << parts[IndexType] << ';';
        new_description << img_bundle.lst_filenames[1];
        auto image_second = GetPropertyBitmap(new_description, false);
        if (image_second.IsOk())
        {
            bitmaps.push_back(image_second);
        }

        if (img_bundle.lst_filenames.size() > 2)
        {
            new_description.clear();
            new_description << parts[IndexType] << ';';
            new_description << img_bundle.lst_filenames[1];
            auto image_third = GetPropertyBitmap(new_description, false);
            if (image_third.IsOk())
            {
                bitmaps.push_back(image_third);
            }
        }

        img_bundle.bundle = wxBitmapBundle::FromBitmaps(bitmaps);
    }

    m_bundles[lookup_str] = std::move(img_bundle);
    return &m_bundles[lookup_str];
}

bool Project::AddSvgBundleImage(ttlib::cstr path, Node* form)
{
    // Run the file through an XML parser so that we can remove content that isn't used, as well as removing line breaks,
    // leading spaces, etc.
    pugi::xml_document doc;
    auto result = doc.load_file(path.c_str());
    if (!result)
    {
        return false;
    }

    auto root = doc.first_child();  // this should be the <svg> element.
    root.remove_attributes();       // we don't need any of the attributes

    // Remove some inkscape nodes that we don't need
    root.remove_child("sodipodi:namedview");
    root.remove_child("metadata");

    std::ostringstream xml_stream;
    doc.save(xml_stream, "", pugi::format_raw | pugi::format_no_declaration);
    std::string str = xml_stream.str();

    // Include the trailing zero -- we need to read this back as a string, not a data array
    wxMemoryInputStream stream(str.c_str(), str.size() + 1);

    wxMemoryOutputStream memory_stream;
    wxZlibOutputStream save_strem(memory_stream, wxZ_BEST_COMPRESSION);
    m_map_embedded[path.filename().as_str()] = std::make_unique<EmbeddedImage>();
    auto embed = m_map_embedded[path.filename().as_str()].get();
    InitializeArrayName(embed, path.filename());
    embed->form = form;

    size_t org_size = (stream.GetLength() & 0xFFFFFFFF);

    if (!CopyStreamData(&stream, &save_strem, stream.GetLength()))
    {
        // TODO: [KeyWorks - 03-16-2022] This would be really bad, though it should be impossible
        return false;
    }
    save_strem.Close();
    auto compressed_size = memory_stream.TellO();

    auto read_stream = memory_stream.GetOutputStreamBuffer();
    embed->type = wxBITMAP_TYPE_INVALID;
    embed->array_size = (compressed_size | (org_size << 32));
    embed->array_data = std::make_unique<unsigned char[]>(compressed_size);
    memcpy(embed->array_data.get(), read_stream->GetBufferStart(), compressed_size);

#if defined(_DEBUG) || defined(INTERNAL_TESTING)
    wxFile file_original(path.wx_str(), wxFile::read);
    if (file_original.IsOpened())
    {
        auto file_size = file_original.Length();
        ttlib::cstr size_comparison;
        int percent = static_cast<int>(100 - (100 / (file_size / compressed_size)));
        size_comparison.Format("%v -- Original: %ku, compressed: %ku, %u percent", path.filename(), file_size,
                               compressed_size, percent);
        // Enable line below to show results for every file
        // MSG_INFO(size_comparison)
    }
#endif

    return true;
}
