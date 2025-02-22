/////////////////////////////////////////////////////////////////////////////
// Purpose:   Common component functions
// Author:    Ralph Walden
// Copyright: Copyright (c) 2020-2022 KeyWorks Software (Ralph Walden)
// License:   Apache License -- see ../../LICENSE
/////////////////////////////////////////////////////////////////////////////

#include <charconv>  // for std::to_chars

#include "gen_common.h"

#include "gen_base.h"       // BaseCodeGenerator -- Generate Src and Hdr files for Base Class
#include "image_bundle.h"   // Functions for working with wxBitmapBundle
#include "lambdas.h"        // Functions for formatting and storage of lamda events
#include "node.h"           // Node class
#include "project_class.h"  // Project class
#include "utils.h"          // Utility functions that work with properties
#include "write_code.h"     // WriteCode -- Write code to Scintilla or file

ttlib::cstr GenerateSizerFlags(Node* node)
{
    ttlib::cstr code("wxSizerFlags");

    if (auto& prop = node->prop_as_string(prop_proportion); prop != "0")
    {
        code << '(' << prop << ')';
    }
    else
    {
        code << "()";
    }

    if (auto& prop = node->prop_as_string(prop_alignment); prop.size())
    {
        if (prop.contains("wxALIGN_CENTER"))
        {
            // Note that CenterHorizontal() and CenterVertical() require wxWidgets 3.1 or higher. Their advantage is
            // generating an assert if you try to use one that is invalid if the sizer parent's orientation doesn't support
            // it. Center() just works without the assertion check.
            code << ".Center()";
        }

        if (prop.contains("wxALIGN_LEFT"))
        {
            code << ".Left()";
        }
        else if (prop.contains("wxALIGN_RIGHT"))
        {
            code << ".Right()";
        }

        if (prop.contains("wxALIGN_TOP"))
        {
            code << ".Top()";
        }
        else if (prop.contains("wxALIGN_BOTTOM"))
        {
            code << ".Bottom()";
        }
    }

    if (auto& prop = node->prop_as_string(prop_flags); prop.size())
    {
        if (prop.contains("wxEXPAND"))
        {
            code << ".Expand()";
        }
        if (prop.contains("wxSHAPED"))
        {
            code << ".Shaped()";
        }
        if (prop.contains("wxFIXED_MINSIZE"))
        {
            code << ".FixedMinSize()";
        }
        if (prop.contains("wxRESERVE_SPACE_EVEN_IF_HIDDEN"))
        {
            code << ".ReserveSpaceEvenIfHidden()";
        }
    }

    if (auto& prop = node->prop_as_string(prop_borders); prop.size())
    {
        auto border_size = node->prop_as_string(prop_border_size);
        if (prop.contains("wxALL"))
        {
            if (border_size == "5")
                code << ".Border(wxALL)";
            else if (border_size == "10")
                code << ".DoubleBorder(wxALL)";
            else if (border_size == "15")
                code << ".TripleBorder(wxALL)";
            else
            {
                code << ".Border(wxALL, " << border_size << ')';
            }
        }
        else
        {
            code << ".Border(";
            ttlib::cstr border_flags;

            if (prop.contains("wxLEFT"))
            {
                if (border_flags.size())
                    border_flags << '|';
                border_flags << "wxLEFT";
            }
            if (prop.contains("wxRIGHT"))
            {
                if (border_flags.size())
                    border_flags << '|';
                border_flags << "wxRIGHT";
            }
            if (prop.contains("wxTOP"))
            {
                if (border_flags.size())
                    border_flags << '|';
                border_flags << "wxTOP";
            }
            if (prop.contains("wxBOTTOM"))
            {
                if (border_flags.size())
                    border_flags << '|';
                border_flags << "wxBOTTOM";
            }
            if (border_flags.empty())
                border_flags = "0";

            code << border_flags << ", ";
            if (border_size == "5")
            {
                code << "wxSizerFlags::GetDefaultBorder())";
            }
            else
            {
                code << border_size << ')';
            }
        }
    }

    return code;
}

void InsertGeneratorInclude(Node* node, const std::string& include, std::set<std::string>& set_src,
                            std::set<std::string>& set_hdr)
{
    if (node->isPropValue(prop_class_access, "none"))
    {
        set_src.insert(include);
    }
    else
    {
        set_hdr.insert(include);
    }
}

ttlib::cstr GenerateColourCode(Node* node, GenEnum::PropName prop_name)
{
    ttlib::cstr code;

    if (!node->HasValue(prop_name))
    {
        code = "wxNullColour";
    }
    else
    {
        if (node->prop_as_string(prop_name).contains("wx"))
        {
            code << "wxSystemSettings::GetColour(" << node->prop_as_string(prop_name) << ")";
        }
        else
        {
            auto colour = node->prop_as_wxColour(prop_name);
            code << ttlib::cstr().Format("wxColour(%i, %i, %i)", colour.Red(), colour.Green(), colour.Blue());
        }
    }

    return code;
}

ttlib::cstr GenerateQuotedString(const ttlib::cstr& str)
{
    ttlib::cstr code;

    if (str.size())
    {
        auto str_with_escapes = ConvertToCodeString(str);

        bool has_utf_char = false;
        for (auto iter: str_with_escapes)
        {
            if (iter < 0)
            {
                has_utf_char = true;
                break;
            }
        }

        if (has_utf_char)
        {
            // While this may not be necessary for non-Windows systems, it does ensure the code compiles on all platforms.
            if (GetProject()->prop_as_bool(prop_internationalize))
                code << "_(wxString::FromUTF8(\"" << str_with_escapes << "\"))";
            else
                code << "wxString::FromUTF8(\"" << str_with_escapes << "\")";
        }
        else
        {
            if (GetProject()->prop_as_bool(prop_internationalize))
                code << "_(\"" << str_with_escapes << "\")";
            else
                code << "\"" << str_with_escapes << "\"";
        }
    }
    else
    {
        code << "wxEmptyString";
    }

    return code;
}

ttlib::cstr GenerateQuotedString(Node* node, GenEnum::PropName prop_name)
{
    if (node->HasValue(prop_name))
    {
        return GenerateQuotedString(node->prop_as_string(prop_name));
    }
    else
    {
        return ttlib::cstr("wxEmptyString");
    }
}

// clang-format off

// List of valid component parent types
static constexpr GenType s_GenParentTypes[] = {

    type_auinotebook,
    type_bookpage,
    type_choicebook,
    type_container,
    type_listbook,
    type_notebook,
    type_ribbonpanel,
    type_simplebook,
    type_splitter,
    type_wizardpagesimple,

};

// clang-format on

ttlib::cstr GetParentName(Node* node)
{
    auto parent = node->GetParent();
    while (parent)
    {
        if (parent->IsSizer())
        {
            if (parent->IsStaticBoxSizer())
            {
                return (ttlib::cstr() << parent->get_node_name() << "->GetStaticBox()");
            }
        }
        if (parent->IsForm())
        {
            return ttlib::cstr("this");
        }

        for (auto iter: s_GenParentTypes)
        {
            if (parent->isType(iter))
            {
                ttlib::cstr name = parent->get_node_name();
                if (parent->isGen(gen_wxCollapsiblePane))
                {
                    name << "->GetPane()";
                }
                return name;
            }
        }
        parent = parent->GetParent();
    }

    ASSERT_MSG(parent, ttlib::cstr() << node->get_node_name() << " has no parent!");
    return ttlib::cstr("internal error");
}

void GenPos(Node* node, ttlib::cstr& code)
{
    auto point = node->prop_as_wxPoint(prop_pos);
    if (point.x != -1 || point.y != -1)
    {
        if (node->prop_as_string(prop_pos).contains("d", tt::CASE::either))
        {
            code << "ConvertDialogToPixels(wxPoint(" << point.x << ", " << point.y << "))";
        }
        else
        {
            code << "wxPoint(" << point.x << ", " << point.y << ")";
        }
    }
    else
        code << "wxDefaultPosition";
}

void GenSize(Node* node, ttlib::cstr& code)
{
    if (node->as_wxSize(prop_size) != wxDefaultSize)
        code << GenerateWxSize(node, prop_size);
    else
        code << "wxDefaultSize";
}

void GenStyle(Node* node, ttlib::cstr& code, const char* prefix)
{
    ttlib::cstr all_styles;

    if (node->HasValue(prop_tab_position) && !node->prop_as_string(prop_tab_position).is_sameas("wxBK_DEFAULT"))
    {
        if (all_styles.size())
            all_styles << '|';
        all_styles << node->prop_as_string(prop_tab_position);
    }

    if (node->HasValue(prop_orientation) && !node->prop_as_string(prop_orientation).is_sameas("wxGA_HORIZONTAL"))
    {
        if (all_styles.size())
            all_styles << '|';
        all_styles << node->prop_as_string(prop_orientation);
    }

    if (node->isGen(gen_wxRichTextCtrl))
    {
        if (all_styles.size())
            all_styles << '|';
        all_styles << "wxRE_MULTILINE";
    }

    if (node->HasValue(prop_style))
    {
        if (all_styles.size())
        {
            all_styles << '|';
        }
        if (prefix)
        {
            all_styles << node->prop_as_constant(prop_style, prefix);
        }
        else
        {
            all_styles << node->prop_as_string(prop_style);
        }
    }

    if (node->HasValue(prop_window_style))
    {
        if (all_styles.size())
            all_styles << '|';
        all_styles << node->prop_as_string(prop_window_style);
    }

    if (node->isGen(gen_wxListView))
    {
        if (all_styles.size())
            all_styles << '|';
        all_styles << node->prop_as_string(prop_mode);
    }

    if (all_styles.empty())
    {
        code << "0";
    }
    else
    {
        code << all_styles;
    }
}

void GeneratePosSizeFlags(Node* node, ttlib::cstr& code, bool uses_def_validator, ttlib::sview def_style)
{
    if (node->HasValue(prop_window_name))
    {
        // Window name is always the last parameter, so if it is specified, everything has to be generated.
        if (code.size() < 80)
            code << ", ";
        else
            code << ",\n\t\t";

        GenPos(node, code);
        code << ", ";
        GenSize(node, code);
        code << ", ";
        GenStyle(node, code);
        if (uses_def_validator)
            code << ", wxDefaultValidator";
        code << ", " << node->prop_as_string(prop_window_name) << ");";
        return;
    }

    ttlib::cstr all_styles;
    GenStyle(node, all_styles);
    if (all_styles.is_sameas("0") || all_styles.is_sameas(def_style))
        all_styles.clear();

    bool isPosSet { false };
    auto pos = node->prop_as_wxPoint(prop_pos);
    if (pos.x != -1 || pos.y != -1)
    {
        if (node->prop_as_string(prop_pos).contains("d", tt::CASE::either))
        {
            code << ", ConvertDialogToPixels(wxPoint(" << pos.x << ", " << pos.y << "))";
        }
        else
        {
            code << ", wxPoint(" << pos.x << ", " << pos.y << ")";
        }

        isPosSet = true;
    }

    bool isSizeSet { false };
    if (node->as_wxSize(prop_size) != wxDefaultSize)
    {
        if (!isPosSet)
        {
            code << ", wxDefaultPosition";
            isPosSet = true;
        }
        code << ", " << GenerateWxSize(node, prop_size);

        isSizeSet = true;
    }

    if (node->HasValue(prop_window_style) && !node->prop_as_string(prop_window_style).is_sameas("wxTAB_TRAVERSAL"))
    {
        if (!isPosSet)
            code << ", wxDefaultPosition";
        if (!isSizeSet)
            code << ", wxDefaultSize";

        code << ", " << all_styles << ");";
        return;
    }

    if (all_styles.size())
    {
        if (!isPosSet)
            code << ", wxDefaultPosition";
        if (!isSizeSet)
            code << ", wxDefaultSize";

        if (code.size() < 100)
            code << ", ";
        else
        {
            code << ",\n\t\t";
            if (code.starts_with("    "))
            {
                code.insert(0, 4, ' ');
            }
            else if (code.at(0) != '\t')
            {
                code.insert(0, 1, '\t');
            }
        }

        code << all_styles << ");";
        return;
    }

    code << ");";
}

int GetStyleInt(Node* node, const char* prefix)
{
    ttlib::cstr styles;

    // If prefix is non-null, this will convert friendly names to wxWidgets constants
    GenStyle(node, styles, prefix);

    int result = 0;
    // Can't use multiview because GetConstantAsInt() searches an unordered_map which requires a std::string to pass to it
    ttlib::multistr mstr(styles, '|');
    for (auto& iter: mstr)
    {
        // Friendly names will have already been converted, so normal lookup works fine.
        result |= g_NodeCreator.GetConstantAsInt(iter);
    }
    return result;
}

struct BTN_BMP_TYPES
{
    GenEnum::PropName prop_name;
    const char* function_name;
};

const BTN_BMP_TYPES btn_bmp_types[] = {
    { prop_bitmap, "SetBitmap" },
    { prop_disabled_bmp, "SetBitmapDisabled" },
    { prop_pressed_bmp, "SetBitmapPressed" },
    { prop_focus_bmp, "SetBitmapFocus" },
    { prop_current, "SetBitmapCurrent" },
};

bool GenBtnBimapCode(Node* node, ttlib::cstr& code, bool is_single)
{
    bool has_additional_bitmaps = (node->HasValue(prop_disabled_bmp) || node->HasValue(prop_pressed_bmp) ||
                                   node->HasValue(prop_focus_bmp) || node->HasValue(prop_current));
    if (code.size())
        code << '\n';

    bool is_old_widgets = (wxGetProject().value(prop_wxWidgets_version) == "3.1");
    if (is_old_widgets)
    {
        if (code.size() && !(code.back() == '\n'))
            code << '\n';
        code << "#if wxCHECK_VERSION(3, 1, 6)\n";
    }

    if (has_additional_bitmaps)
    {
        if (code.size() && !(code.back() == '\n'))
            code << '\n';
        code << "{\n";
    }

    ttlib::cstr bundle_code;
    bool is_vector_generated = false;

    for (auto& iter: btn_bmp_types)
    {
        if (node->HasValue(iter.prop_name))
        {
            bundle_code.clear();
            bool is_code_block = GenerateBundleCode(node->prop_as_string(iter.prop_name), bundle_code);
            if (is_code_block)
            {
                if (is_vector_generated)
                {
                    code << "\n\n\tbitmaps.clear();";
                    // find end of wxVector<wxBitmap> bitmaps;
                    bundle_code.erase(0, bundle_code.find(';'));
                }
                else
                {
                    is_vector_generated = true;
                }

                // GenerateBundleCode assumes an indent within an indent
                bundle_code.Replace("\t\t\t", "\t", true);
                // if has_additional_bitmaps is true, we already have an opening brace
                code << bundle_code.c_str() + (has_additional_bitmaps ? 1 : 0);
                code << "\t" << node->get_node_name() << "->" << iter.function_name;
                code << "(wxBitmapBundle::FromBitmaps(bitmaps));";

                if (!has_additional_bitmaps)
                {
                    code << "\n}";
                }
            }
            else
            {
                if (code.size() && !(code.back() == '\n'))
                    code << '\n';
                code << "\t" << node->get_node_name() << "->" << iter.function_name << "(" << bundle_code << ");";
            }
        }
        if (is_single)
        {
            // Means the caller only wants prop_bitmap
            break;
        }
    }

    if (has_additional_bitmaps)
    {
        code << "\n}";
    }

    /////////// wxWidgets 3.1 code ///////////

    if (is_old_widgets)
    {
        code << "\n#else\n";
        code << node->get_node_name() << "->SetBitmap(" << GenerateBitmapCode(node->prop_as_string(prop_bitmap)) << ");";

        if (node->HasValue(prop_disabled_bmp))
        {
            if (code.size())
                code << '\n';
            code << node->get_node_name() << "->SetBitmapDisabled("
                 << GenerateBitmapCode(node->prop_as_string(prop_disabled_bmp)) << ");";
        }

        if (node->HasValue(prop_pressed_bmp))
        {
            if (code.size())
                code << '\n';
            code << node->get_node_name() << "->SetBitmapPressed("
                 << GenerateBitmapCode(node->prop_as_string(prop_pressed_bmp)) << ");";
        }

        if (node->HasValue(prop_focus_bmp))
        {
            if (code.size())
                code << '\n';
            code << node->get_node_name() << "->SetBitmapFocus(" << GenerateBitmapCode(node->prop_as_string(prop_focus_bmp))
                 << ");";
        }

        if (node->HasValue(prop_current))
        {
            if (code.size())
                code << '\n';
            code << node->get_node_name() << "->SetBitmapCurrent(" << GenerateBitmapCode(node->prop_as_string(prop_current))
                 << ");";
        }

        code << "\n#endif  // wxCHECK_VERSION(3, 1, 6)";
    }

    return is_vector_generated;
}

ttlib::cstr GenerateBitmapCode(const ttlib::cstr& description)
{
    ttlib::cstr code;

    if (description.empty())
    {
        code << "wxNullBitmap";
        return code;
    }

    ttlib::multiview parts(description, BMP_PROP_SEPARATOR, tt::TRIM::both);

    if (parts[IndexType].starts_with("SVG"))
    {
        code << "wxNullBitmap /* SVG images require wxWidgets 3.1.6 */";
        return code;
    }
    else if (parts[IndexImage].empty())
    {
        code << "wxNullBitmap";
        return code;
    }

    if (parts[IndexType].contains("Art"))
    {
        ttlib::cstr art_id(parts[IndexArtID]);
        ttlib::cstr art_client;
        if (auto pos = art_id.find('|'); ttlib::is_found(pos))
        {
            art_client = art_id.subview(pos + 1);
            art_id.erase(pos);
        }

        code << "wxArtProvider::GetBitmap(" << art_id;
        if (art_client.size())
            code << ", " << art_client;
        code << ')';

        return code;
    }

    ttlib::cstr result;
    if (parts[IndexType].is_sameas("XPM"))
    {
        code << "wxImage(";

        ttlib::cstr name(parts[IndexImage].filename());
        name.remove_extension();
        code << name << "_xpm)";
    }
    else
    {
        code << "wxueImage(";

        ttlib::cstr name(parts[1].filename());
        name.remove_extension();
        name.Replace(".", "_", true);  // wxFormBuilder writes files with the extra dots that have to be converted to '_'

        if (parts[IndexType].starts_with("Embed"))
        {
            auto embed = GetProject()->GetEmbeddedImage(parts[IndexImage]);
            if (embed)
            {
                name = "wxue_img::" + embed->array_name;
            }
        }

        code << name << ", sizeof(" << name << "))";
    }
    return code;
}

bool GenerateBundleCode(const ttlib::cstr& description, ttlib::cstr& code)
{
    if (description.empty())
    {
        code << "wxNullBitmap";
        return false;
    }

    ttlib::multiview parts(description, BMP_PROP_SEPARATOR, tt::TRIM::both);

    if (parts[IndexImage].empty())
    {
        code << "wxNullBitmap";
        return false;
    }

    if (parts[IndexType].contains("Art"))
    {
        ttlib::cstr art_id(parts[IndexArtID]);
        ttlib::cstr art_client;
        if (auto pos = art_id.find('|'); ttlib::is_found(pos))
        {
            art_client = art_id.subview(pos + 1);
            art_id.erase(pos);
        }

        code << "wxArtProvider::GetBitmapBundle(" << art_id << ", ";

        // Note that current documentation states that the client is required, but the header file says otherwise
        if (art_client.size())
            code << art_client;
        code << ')';
    }

    else if (parts[IndexType].is_sameas("XPM"))
    {
        if (auto function_name = GetProject()->GetBundleFuncName(description); function_name.size())
        {
            // We get here if there is an Image form that contains the function to retrieve this bundle.
            code = function_name;
            return false;
        }

        if (auto bundle = GetProject()->GetPropertyImageBundle(description); bundle)
        {
            if (bundle->lst_filenames.size() == 1)
            {
                code << "wxBitmapBundle::FromBitmap(wxImage(";
                ttlib::cstr name(bundle->lst_filenames[0].filename());
                name.remove_extension();
                code << name << "_xpm))";
            }
            else if (bundle->lst_filenames.size() == 2)
            {
                code << "wxBitmapBundle::FromBitmaps(wxImage(";
                ttlib::cstr name(bundle->lst_filenames[0].filename());
                name.remove_extension();
                code << name << "_xpm), ";
                name = bundle->lst_filenames[1].filename();
                name.remove_extension();
                code << "wxImage(" << name << "_xpm))";
            }
            else
            {
                code << "{\n\t\t\twxVector<wxBitmap> bitmaps;\n";
                for (auto& iter: bundle->lst_filenames)
                {
                    ttlib::cstr name(iter.filename());
                    name.remove_extension();
                    code << "\t\t\tbitmaps.push_back(wxImage(" << name << "_xpm));\n";
                }

                // Return true to indicate a code block was generated
                return true;
            }
        }
        else
        {
            FAIL_MSG(ttlib::cstr(description) << " was not converted to a bundle ahead of time!")

            // This should never happen, but if it does, at least generate something that will compiler
            code << "wxImage(";
            ttlib::cstr name(parts[IndexImage].filename());
            name.remove_extension();
            code << name << "_xpm)";
        }
    }
    else if (description.starts_with("SVG"))
    {
        if (auto function_name = GetProject()->GetBundleFuncName(description); function_name.size())
        {
            // We get here if there is an Image form that contains the function to retrieve this bundle.
            code = function_name;
            return false;
        }

        auto embed = GetProject()->GetEmbeddedImage(parts[IndexImage]);
        if (!embed)
        {
            FAIL_MSG(ttlib::cstr() << description << " not embedded!")
            code << "wxNullBitmap";
            return false;
        }

        wxSize svg_size { -1, -1 };
        if (parts[IndexSize].size())
        {
            GetSizeInfo(svg_size, parts[IndexSize]);
        }

        ttlib::cstr name = "wxue_img::" + embed->array_name;
        code << "wxueBundleSVG(" << name << ", " << (embed->array_size & 0xFFFFFFFF) << ", ";
        code << (embed->array_size >> 32) << ", wxSize(" << svg_size.x << ", " << svg_size.y << "))";
    }
    else
    {
        if (auto function_name = GetProject()->GetBundleFuncName(description); function_name.size())
        {
            // We get here if there is an Image form that contains the function to retrieve this bundle.
            code = function_name;
            return false;
        }

        if (auto bundle = GetProject()->GetPropertyImageBundle(description); bundle)
        {
            if (bundle->lst_filenames.size() == 1)
            {
                code << "wxBitmapBundle::FromBitmap(wxueImage(";
                ttlib::cstr name(bundle->lst_filenames[0].filename());
                name.remove_extension();
                name.Replace(".", "_", true);  // fix wxFormBuilder header files

                if (parts[IndexType].starts_with("Embed"))
                {
                    auto embed = GetProject()->GetEmbeddedImage(bundle->lst_filenames[0]);
                    if (embed)
                    {
                        name = "wxue_img::" + embed->array_name;
                    }
                }

                code << name << ", sizeof(" << name << ")))";
            }
            else if (bundle->lst_filenames.size() == 2)
            {
                code << "wxBitmapBundle::FromBitmaps(wxueImage(";
                ttlib::cstr name(bundle->lst_filenames[0].filename());
                name.remove_extension();
                name.Replace(".", "_", true);  // fix wxFormBuilder header files

                if (parts[IndexType].starts_with("Embed"))
                {
                    auto embed = GetProject()->GetEmbeddedImage(bundle->lst_filenames[0]);
                    if (embed)
                    {
                        name = "wxue_img::" + embed->array_name;
                    }
                }
                code << name << ", sizeof(" << name << ")), wxueImage(";

                name = bundle->lst_filenames[1].filename();
                name.remove_extension();
                name.Replace(".", "_", true);

                if (parts[IndexType].starts_with("Embed"))
                {
                    auto embed = GetProject()->GetEmbeddedImage(bundle->lst_filenames[1]);
                    if (embed)
                    {
                        name = "wxue_img::" + embed->array_name;
                    }
                }
                code << name << ", sizeof(" << name << ")))";
            }
            else
            {
                code << "{\n\t\t\twxVector<wxBitmap> bitmaps;\n";
                for (auto& iter: bundle->lst_filenames)
                {
                    ttlib::cstr name(iter.filename());
                    name.remove_extension();
                    name.Replace(".", "_", true);  // fix wxFormBuilder header files
                    if (parts[IndexType].starts_with("Embed"))
                    {
                        auto embed = GetProject()->GetEmbeddedImage(iter);
                        if (embed)
                        {
                            name = "wxue_img::" + embed->array_name;
                        }
                    }
                    code << "\t\t\tbitmaps.push_back(wxueImage(" << name << ", sizeof(" << name << ")));\n";
                }

                // Return true to indicate a code block was generated
                return true;
            }
        }
        else
        {
            FAIL_MSG(ttlib::cstr(description) << " was not converted to a bundle ahead of time!")

            // This should never happen, but if it does, at least generate something that will compiler
            code << "wxNullBitmsap";
        }
    }

    return false;
}

bool GenerateVectorCode(const ttlib::cstr& description, ttlib::cstr& code)
{
    if (description.empty())
    {
        return false;
    }

    ttlib::multiview parts(description, BMP_PROP_SEPARATOR, tt::TRIM::both);

    if (parts[IndexImage].empty() || parts[IndexType].contains("Art") || parts[IndexType].contains("SVG"))
    {
        return false;
    }

    auto bundle = GetProject()->GetPropertyImageBundle(description);

    if (!bundle || bundle->lst_filenames.size() < 3)
    {
        return false;
    }

    // If we get here, then we need to first put the bitmaps into a wxVector in order for wxBitmapBundle to load them.

    code << "\t{\n";
    if (wxGetProject().value(prop_wxWidgets_version) == "3.1")
    {
        code << "#if wxCHECK_VERSION(3, 1, 6)\n";
    }
    code << "\t\twxVector<wxBitmap> bitmaps;\n";

    bool is_xpm = (parts[IndexType].is_sameas("XPM"));

    for (auto& iter: bundle->lst_filenames)
    {
        ttlib::cstr name(iter.filename());
        name.remove_extension();
        if (is_xpm)
        {
            code << "\t\tbitmaps.push_back(wxImage(" << name << "_xpm));\n";
        }
        else
        {
            name.Replace(".", "_", true);  // fix wxFormBuilder header files
            if (parts[IndexType].starts_with("Embed"))
            {
                auto embed = GetProject()->GetEmbeddedImage(iter);
                if (embed)
                {
                    name = "wxue_img::" + embed->array_name;
                }
            }
            code << "\t\tbitmaps.push_back(wxueImage(" << name << ", sizeof(" << name << ")));\n";
        }
    }
    if (wxGetProject().value(prop_wxWidgets_version) == "3.1")
    {
        code << "#endif\n";
    }

    // Note that the opening brace is *not* closed. That will have to be handled by BaseCodeGenerator::GenConstruction()

    return true;
}

ttlib::cstr GenFormCode(GenEnum::GenCodeType command, Node* node)
{
    ttlib::cstr code;

    switch (command)
    {
        case code_base_class:
            if (node->HasValue(prop_derived_class))
            {
                code << node->prop_as_string(prop_derived_class);
            }
            else
            {
                code << node->DeclName();
            }
            break;

        case code_after_children:
            if (node->isGen(gen_wxToolBar))
            {
                if (code.size())
                    code << "\n";
                code << "\tRealize();";
            }
            else if (node->isGen(gen_RibbonBar))
            {
                if (code.size())
                    code << "\n";
                code << "\tRealize();";
            }
            else
            {
                auto& center = node->prop_as_string(prop_center);
                if (center.size() && !center.is_sameas("no"))
                {
                    code << "\tCentre(" << center << ");";
                }
            }
            break;

        case code_header:
            code << node->get_node_name() << "(wxWindow* parent, wxWindowID id = " << node->prop_as_string(prop_id);
            if (!node->isGen(gen_wxPanel) && !node->isGen(gen_wxToolBar) && !node->isGen(gen_ToolBar) &&
                !node->isGen(gen_wxAuiToolBar) && !node->isGen(gen_wxPopupTransientWindow) && !node->isGen(gen_PanelForm))
            {
                code << ",\n\tconst wxString& title = ";
                auto& title = node->prop_as_string(prop_title);
                if (title.size())
                {
                    code << GenerateQuotedString(title) << ",\n\t";
                }
                else
                {
                    code << "wxEmptyString,\n\t";
                }
            }
            else
            {
                code << ", ";
            }
            code << "const wxPoint& pos = ";
            auto point = node->prop_as_wxPoint(prop_pos);
            if (point.x != -1 || point.y != -1)
                code << "wxPoint(" << point.x << ", " << point.y << ")";
            else
                code << "wxDefaultPosition";

            // BUGBUG: [KeyWorks - 05-20-2021] This doesn't make sense in a wxDialog because the generated code calls SetSize
            // with this value -- and without that call, SetSizerAndFit() will ignore this setting.
            code << ", const wxSize& size = ";
            auto size = node->prop_as_wxPoint(prop_size);
            if (size.x != -1 || size.y != -1)
                code << "wxSize(" << size.x << ", " << size.y << ")";
            else
                code << "wxDefaultSize";

            code << ",\n\tlong style = ";
            auto& style = node->prop_as_string(prop_style);
            auto& win_style = node->prop_as_string(prop_window_style);
            if (style.empty() && win_style.empty())
                code << "0";
            else
            {
                if (style.size())
                {
                    code << style;
                    if (win_style.size())
                    {
                        code << '|' << win_style;
                    }
                }
                else if (win_style.size())
                {
                    code << win_style;
                }
            }

            if (node->prop_as_string(prop_window_name).size())
            {
                code << ", const wxString& name = " << node->prop_as_string(prop_window_name);
            }

            code << ");\n\n";
            break;
    }

    return code;
}

ttlib::cstr GenFormSettings(Node* node)
{
    ttlib::cstr code;

    if (!node->isGen(gen_PanelForm) && !node->isGen(gen_wxToolBar))
    {
        auto max_size = node->prop_as_wxSize(prop_maximum_size);
        auto min_size = node->prop_as_wxSize(prop_minimum_size);
        if (min_size != wxDefaultSize || max_size != wxDefaultSize)
        {
            code << "SetSizeHints(";
            if (min_size.GetX() != -1 || min_size.GetY() != -1)
                code << "wxSize(" << min_size.x << ", " << min_size.y << ")";
            else
                code << "wxDefaultSize";

            if (max_size.GetX() != -1 || max_size.GetY() != -1)
                code << ", wxSize(" << max_size.x << ", " << max_size.y << ")";

            code << ");";
        }
    }
    if (node->prop_as_string(prop_window_extra_style).size())
        code << "\nSetExtraStyle(GetExtraStyle() | " << node->prop_as_string(prop_window_extra_style) << ");";

    code << GenFontColourSettings(node);

    if (node->prop_as_bool(prop_disabled))
        code << "\nDisable();";

    if (node->prop_as_bool(prop_hidden))
        code << "\nHide();";

    return code;
}

ttlib::cstr GenFontColourSettings(Node* node)
{
    ttlib::cstr code;

    if (node->prop_as_string(prop_font).size())
    {
        FontProperty fontprop(node->get_prop_ptr(prop_font));
        if (fontprop.isDefGuiFont())
        {
            code << "{\n\twxFont font(wxSystemSettings::GetFont(wxSYS_DEFAULT_GUI_FONT));";
            if (fontprop.GetSymbolSize() != wxFONTSIZE_MEDIUM)
                code << "\n\tfont.SetSymbolicSize(" << font_symbol_pairs.GetValue(fontprop.GetSymbolSize()) << ");";
            if (fontprop.GetStyle() != wxFONTSTYLE_NORMAL)
                code << "\n\tfont.SetStyle(" << font_style_pairs.GetValue(fontprop.GetStyle()) << ");";
            if (fontprop.GetWeight() != wxFONTWEIGHT_NORMAL)
                code << "\n\tfont.SetWeight(" << font_weight_pairs.GetValue(fontprop.GetWeight()) << ");";
            if (fontprop.IsUnderlined())
                code << "\n\tfont.SetUnderlined(true);";
            if (fontprop.IsStrikethrough())
                code << "\n\tfont.SetStrikethrough(true);";

            if (node->IsForm())
            {
                code << "\n\t"
                     << "SetFont(font);\n}";
            }
            else if (node->isGen(gen_wxStyledTextCtrl))
            {
                code << "\n\t" << node->get_node_name() << "->StyleSetFont(wxSTC_STYLE_DEFAULT, font);\n}";
            }
            else
            {
                code << "\n\t" << node->get_node_name() << "->SetFont(font);\n}";
            }
        }
        else
        {
            auto point_size = fontprop.GetFractionalPointSize();
            code << "{\n\twxFontInfo font_info(";
            if (point_size != static_cast<int>(point_size))
            {
                code << "\n#if !wxCHECK_VERSION(3, 1, 2)\n\t";
                if (point_size <= 0)
                {
                    code << "wxSystemSettings::GetFont()->GetPointSize());";
                }
                else
                {
                    // GetPointSize() will round the result rather than truncating the decimal
                    code << fontprop.GetPointSize() << ");";
                }
                code << "\n#else  // fractional point sizes are new to wxWidgets 3.1.2\n\t";
                {
                    std::array<char, 10> float_str;
                    if (auto [ptr, ec] = std::to_chars(float_str.data(), float_str.data() + float_str.size(), point_size);
                        ec == std::errc())
                    {
                        code << std::string_view(float_str.data(), ptr - float_str.data()) << ");";
                    }
                }

                // leave a trailing dot -- we'll remove the final dot after all the params are added
                code << "\n#endif";
            }
            else
            {
                if (point_size <= 0)
                {
                    code << "wxSystemSettings::GetFont()->GetPointSize());";
                }
                else
                {
                    // GetPointSize() will round the result rather than truncating the decimal
                    code << fontprop.GetPointSize() << ");";
                }
            }

            ttlib::cstr info_code("\n\tfont_info.");

            if (fontprop.GetFaceName().size() && fontprop.GetFaceName() != "default")
                info_code << "FaceName(" << GenerateQuotedString(ttlib::cstr() << fontprop.GetFaceName().wx_str()) << ").";
            if (fontprop.GetFamily() != wxFONTFAMILY_DEFAULT)
                info_code << "Family(" << font_family_pairs.GetValue(fontprop.GetFamily()) << ").";
            if (fontprop.GetStyle() != wxFONTSTYLE_NORMAL)
                info_code << "Style(" << font_style_pairs.GetValue(fontprop.GetStyle()) << ").";
            if (fontprop.GetWeight() != wxFONTWEIGHT_NORMAL)
                info_code << "Weight(" << font_weight_pairs.GetValue(fontprop.GetWeight()) << ").";
            if (fontprop.IsUnderlined())
                info_code << "Underlined().";
            if (fontprop.IsStrikethrough())
                info_code << "Strikethrough()";

            if (info_code.back() == '.')
                info_code.pop_back();

            if (!info_code.is_sameas("font_info"))
                code << info_code << ';';

            if (node->IsForm())
            {
                code << "\n\t"
                        "SetFont(wxFont(font_info));\n}";
            }
            else
            {
                code << "\n\t" << node->get_node_name() << "->SetFont(wxFont(font_info));\n}";
            }
        }
    }
    auto& fg_clr = node->prop_as_string(prop_foreground_colour);
    if (fg_clr.size())
    {
        if (code.size())
            code << '\n';

        if (node->IsForm())
        {
            code << "SetForegroundColour(";
        }
        else
        {
            code << node->get_node_name() << "->SetForegroundColour(";
        }
        if (fg_clr.contains("wx"))
            code << "wxSystemSettings::GetColour(" << fg_clr << ")";
        else
        {
            wxColour colour = ConvertToColour(fg_clr);
            code << ttlib::cstr().Format("wxColour(%i, %i, %i)", colour.Red(), colour.Green(), colour.Blue());
        }
        code << ");";
    }

    auto& bg_clr = node->prop_as_string(prop_background_colour);
    if (bg_clr.size())
    {
        if (node->IsForm())
        {
            code << "SetBackgroundColour(";
        }
        else
        {
            code << node->get_node_name() << "->SetBackgroundColour(";
        }
        if (bg_clr.contains("wx"))
            code << "wxSystemSettings::GetColour(" << bg_clr << ")";
        else
        {
            wxColour colour = ConvertToColour(bg_clr);
            code << ttlib::cstr().Format("wxColour(%i, %i, %i)", colour.Red(), colour.Green(), colour.Blue());
        }
        code << ");";
    }

    return code;
}

// Add C++ escapes around any characters the compiler wouldn't accept as a normal part of a string. Used when generating
// code.
ttlib::cstr ConvertToCodeString(const ttlib::cstr& text)
{
    ttlib::cstr result;

    for (auto c: text)
    {
        switch (c)
        {
            case '"':
                result += "\\\"";
                break;

            // This generally isn't needed for C++, but is needed for Python
            case '\'':
                result += "\\'";
                break;

            case '\\':
                result += "\\\\";
                break;

            case '\t':
                result += "\\t";
                break;

            case '\n':
                result += "\\n";
                break;

            case '\r':
                result += "\\r";
                break;

            default:
                result += c;
                break;
        }
    }
    return result;
}

ttlib::cstr GenerateNewAssignment(Node* node, bool use_generic)
{
    ttlib::cstr code(" = new ");
    if (node->HasValue(prop_derived_class))
    {
        code << node->prop_as_string(prop_derived_class);
    }
    else if (use_generic)
    {
        ttlib::cstr class_name = node->DeclName();
        class_name.Replace("wx", "wxGeneric");
        code << class_name;
    }
    else if (node->isGen(gen_BookPage))
    {
        code << "wxPanel";
    }
    else
    {
        code << node->DeclName();
    }
    code << '(';
    return code;
}

std::optional<ttlib::cstr> GenGetSetCode(Node* node)
{
    auto& get_name = node->prop_as_string(prop_get_function);
    auto& set_name = node->prop_as_string(prop_set_function);
    if (get_name.empty() && set_name.empty())
        return {};

    if (auto& var_name = node->prop_as_string(prop_validator_variable); var_name.size())
    {
        auto& val_data_type = node->prop_as_string(prop_validator_data_type);
        if (val_data_type.empty())
            return {};
        ttlib::cstr code;
        if (val_data_type == "wxString" || val_data_type == "wxFileName" || val_data_type == "wxArrayInt")
        {
            if (get_name.size())
                code << "\tconst " << val_data_type << "& " << get_name << "() const { return " << var_name << "; }";
            if (set_name.size())
            {
                if (code.size())
                    code << "\n";
                code << "\tvoid " << set_name << "(const " << val_data_type << "& value) { " << var_name << " = value; }";
            }
            if (code.empty())
                return {};
            return code;
        }
        else
        {
            if (get_name.size())
                code << '\t' << val_data_type << ' ' << get_name << "() const { return " << var_name << "; }";
            if (set_name.size())
            {
                if (code.size())
                    code << "\n";
                code << "\tvoid " << set_name << "(" << val_data_type << " value) { " << var_name << " = value; }";
            }
            if (code.empty())
                return {};
            return code;
        }
    }

    return {};
}

std::optional<ttlib::cstr> GenValidatorSettings(Node* node)
{
    if (auto& var_name = node->prop_as_string(prop_validator_variable); var_name.size())
    {
        auto& val_data_type = node->prop_as_string(prop_validator_data_type);
        if (val_data_type.empty())
            return {};

        ttlib::cstr code;
        auto& validator_type = node->prop_as_string(prop_validator_type);
        if (validator_type.is_sameas("wxTextValidator"))
        {
            code << node->get_node_name() << "->SetValidator(wxTextValidator(" << node->prop_as_string(prop_validator_style)
                 << ", &" << var_name << "));";
        }
        else
        {
            if (node->isGen(gen_StaticCheckboxBoxSizer))
                code << node->prop_as_string(prop_checkbox_var_name);
            else if (node->isGen(gen_StaticRadioBtnBoxSizer))
                code << node->prop_as_string(prop_radiobtn_var_name);
            else
                code << node->get_node_name();

            if (node->isGen(gen_wxRearrangeCtrl))
                code << "->GetList()";
            code << "->SetValidator(wxGenericValidator(&" << var_name << "));";
        }

        return code;
    }

    return {};
}

static void AddPropIfUsed(PropName prop_name, ttlib::sview func_name, Node* node, ttlib::cstr& code)
{
    if (prop_name == prop_background_colour)
    {
        auto& color = node->prop_as_string(prop_background_colour);
        if (color.size())
        {
            if (code.size())
                code << "\n\t";
            if (!node->IsForm())
                code << node->get_node_name() << "->";
            code << "SetBackgroundColour(";
            if (color.contains("wx"))
                code << "wxSystemSettings::GetColour(" << color << "));";
            else
            {
                wxColour colour = ConvertToColour(color);
                code << ttlib::cstr().Format("wxColour(%i, %i, %i);", colour.Red(), colour.Green(), colour.Blue());
            }
        }
    }
    else if (prop_name == prop_foreground_colour)
    {
        auto& color = node->prop_as_string(prop_foreground_colour);
        if (color.size())
        {
            if (code.size())
                code << "\n\t";
            if (!node->IsForm())
                code << node->get_node_name() << "->";
            code << "SetForegroundColour(";
            if (color.contains("wx"))
                code << "wxSystemSettings::GetColour(" << color << "));";
            else
            {
                wxColour colour = ConvertToColour(color);
                code << ttlib::cstr().Format("wxColour(%i, %i, %i);", colour.Red(), colour.Green(), colour.Blue());
            }
        }
    }
    else if (node->prop_as_string(prop_name).size())
    {
        if (code.size())
            code << "\n\t";
        if (!node->IsForm())
            code << node->get_node_name() << "->";
        code << func_name << node->prop_as_string(prop_name) << ");";
    }
}

void GenerateWindowSettings(Node* node, ttlib::cstr& code)
{
    AddPropIfUsed(prop_window_extra_style, "SetExtraStyle(", node, code);
    code << GenFontColourSettings(node);

    // Note that \t is added after the \n in case the caller needs to keep indents

    if (node->prop_as_bool(prop_disabled))
    {
        if (code.size())
            code << "\n";
        if (!node->IsForm())
            code << node->get_node_name() << "->";
        code << "Enable(false);";
    }

    if (node->prop_as_bool(prop_hidden))
    {
        if (code.size())
            code << "\n";
        if (!node->IsForm())
            code << node->get_node_name() << "->";
        code << "Hide();";
    }

    bool allow_minmax { true };
    bool is_smart_size { false };

    // REVIEW: [KeyWorks - 05-20-2021] Because of issue #242, this was shut off for all forms.

    // The following needs to match GenFormSettings() in gen_common.cpp. If these conditions are met, then GenFormSettings()
    // will generate SetSizeHints(min, max) so there is no reason to generate SetMinSize()/SetMaxSize()
    if (node->IsForm() && !node->isGen(gen_PanelForm) && !node->isGen(gen_wxToolBar))
        allow_minmax = false;

    auto size = node->prop_as_wxSize(prop_smart_size);
    if (size != wxDefaultSize)
    {
        is_smart_size = true;  // Set to true means prop_size and prop_minimum_size will be ignored

        // REVIEW: [KeyWorks - 12-07-2021] Do we need to block if allow_minmax is false?

        if (code.size())
            code << "\n";
        code << node->get_node_name() << "->SetInitialSize(";
        if (node->prop_as_string(prop_smart_size).contains("d", tt::CASE::either))
            code << "ConvertDialogToPixels(";

        if (size.IsFullySpecified())
            code << "\n\t";
        code << "wxSize(";

        if (size.x != -1)
            code << size.x << " > GetBestSize().x ? " << size.x << " : -1, ";
        else
            code << "-1, ";
        if (size.y != -1)
            code << size.y << " > GetBestSize().y ? " << size.y << " : -1";
        else
            code << "-1";
        if (node->prop_as_string(prop_smart_size).contains("d", tt::CASE::either))
            code << ')';  // close the ConvertDialogToPixels function call
        code << "));";
    }

    if (!is_smart_size && allow_minmax)
    {
        if (node->prop_as_wxSize(prop_minimum_size) != wxDefaultSize)
        {
            if (code.size())
                code << "\n";
            code << node->get_node_name() << "->";
            code << "SetMinSize(" << GenerateWxSize(node, prop_minimum_size) << ");";
        }
    }

    if (node->prop_as_wxSize(prop_maximum_size) != wxDefaultSize)
    {
        if (allow_minmax)
        {
            if (code.size())
                code << "\n";
            code << node->get_node_name() << "->";
            code << "SetMaxSize(" << GenerateWxSize(node, prop_maximum_size) << ");";
        }
    }

    if (!node->IsForm() && !node->isPropValue(prop_variant, "normal"))
    {
        if (code.size())
            code << "\n";
        code << node->get_node_name() << "->SetWindowVariant(";

        if (node->isPropValue(prop_variant, "small"))
            code << "wxWINDOW_VARIANT_SMALL);";
        else if (node->isPropValue(prop_variant, "mini"))
            code << "wxWINDOW_VARIANT_MINI);";
        else
            code << "wxWINDOW_VARIANT_LARGE);";
    }

    if (node->prop_as_string(prop_tooltip).size())
    {
        if (code.size())
            code << "\n";
        if (!node->IsForm())
            code << node->get_node_name() << "->";
        code << "SetToolTip(" << GenerateQuotedString(node->prop_as_string(prop_tooltip)) << ");";
    }

    if (node->prop_as_string(prop_context_help).size())
    {
        if (code.size())
            code << "\n";
        if (!node->IsForm())
            code << node->get_node_name() << "->";
        code << "SetHelpText(" << GenerateQuotedString(node->prop_as_string(prop_context_help)) << ");";
    }
}

// Generates code for any class inheriting from wxTopLevelWindow -- this will generate everything needed to set the
// window's icon.

ttlib::cstr GenerateIconCode(const ttlib::cstr& description)
{
    ttlib::cstr code;

    if (description.empty())
    {
        return code;
    }

    ttlib::multiview parts(description, BMP_PROP_SEPARATOR, tt::TRIM::both);

    if (parts.size() < 2 || parts[IndexImage].empty())
    {
        return code;
    }

    if (parts[IndexType].is_sameas("XPM"))
    {
        // In theory, we could create an alpha channel using black as the transparency, but it just doesn't make sense
        // for the user to be using XPM files as an icon.
        code << "// XPM files do not contain an alpha channel and cannot be used as an icon.";
        return code;
    }

    if (wxGetProject().value(prop_wxWidgets_version) == "3.1" && !parts[IndexType].is_sameas("SVG"))
    {
        code << "#if wxCHECK_VERSION(3, 1, 6)\n";
    }

    if (parts[IndexType].contains("Art"))
    {
        ttlib::cstr art_id(parts[IndexArtID]);
        ttlib::cstr art_client;
        if (auto pos = art_id.find('|'); ttlib::is_found(pos))
        {
            art_client = art_id.subview(pos + 1);
            art_id.erase(pos);
        }

        code << "SetIcon(wxArtProvider::GetBitmapBundle(" << art_id << ", ";
        // Note that current documentation states that the client is required, but the header file says otherwise
        if (art_client.size())
            code << art_client;
        code << ").GetIconFor(this));\n";
    }
    else if (description.starts_with("SVG"))
    {
        auto embed = GetProject()->GetEmbeddedImage(parts[IndexImage]);
        if (!embed)
        {
            FAIL_MSG(ttlib::cstr() << description << " not embedded!")
            return code;
        }

        auto svg_size = get_image_prop_size(parts[IndexSize]);

        ttlib::cstr name = "wxue_img::" + embed->array_name;
        code << "SetIcon(wxueBundleSVG(" << name << ", " << (embed->array_size & 0xFFFFFFFF) << ", ";
        code << (embed->array_size >> 32) << ", wxSize(" << svg_size.x << ", " << svg_size.y << "))";
        code << ".GetIconFor(this));\n";
        return code;
    }
    else
    {
        if (auto bundle = GetProject()->GetPropertyImageBundle(description); bundle)
        {
            if (bundle->lst_filenames.size() == 1)
            {
                code << "SetIcon(wxBitmapBundle::FromBitmap(wxueImage(";
                ttlib::cstr name(bundle->lst_filenames[0].filename());
                name.remove_extension();
                name.Replace(".", "_", true);  // fix wxFormBuilder header files

                if (parts[IndexType].starts_with("Embed"))
                {
                    auto embed = GetProject()->GetEmbeddedImage(bundle->lst_filenames[0]);
                    if (embed)
                    {
                        name = "wxue_img::" + embed->array_name;
                    }
                }

                code << name << ", sizeof(" << name << ")))";
                code << ".GetIconFor(this));\n";
            }
            else
            {
                code << "{\n\twxIconBundle icon_bundle;\n\twxIcon icon;\n";
                for (auto& iter: bundle->lst_filenames)
                {
                    ttlib::cstr name(iter.filename());
                    name.remove_extension();
                    name.Replace(".", "_", true);  // fix wxFormBuilder header files
                    if (parts[IndexType].starts_with("Embed"))
                    {
                        auto embed = GetProject()->GetEmbeddedImage(iter);
                        if (embed)
                        {
                            name = "wxue_img::" + embed->array_name;
                        }
                    }
                    code << "\ticon.CopyFromBitmap(wxueImage(" << name << ", sizeof(" << name << ")));\n";
                    code << "\ticon_bundle.AddIcon(icon);\n";
                }
                code << "\tSetIcons(icon_bundle);\n}\n";
            }
        }
        else
        {
            FAIL_MSG(ttlib::cstr(description) << " was not converted to a bundle ahead of time!")
            return code;
        }
    }

    if (wxGetProject().value(prop_wxWidgets_version) == "3.1")
    {
        code << "#else\n";
        auto image_code = GenerateBitmapCode(description);
        if (!image_code.contains(".Scale") && image_code.starts_with("wxImage("))
        {
            code << "SetIcon(wxIcon(" << image_code.subview(sizeof("wxImage")) << ");\n";
        }
        else
        {
            code << "{\n";
            code << "\twxIcon icon;\n";
            code << "\ticon.CopyFromBitmap(" << GenerateBitmapCode(description) << ");\n";
            code << "\tSetIcon(wxIcon(icon));\n";
            code << "}\n";
        }
        code << "#endif\n";
    }

    return code;
}

// This is called to add a tool to either wxToolBar or wxAuiToolBar
ttlib::cstr GenToolCode(Node* node, ttlib::sview BitmapCode)
{
    ttlib::cstr code;
    code << '\t';

    if (node->prop_as_bool(prop_disabled) || (node->prop_as_string(prop_id) == "wxID_ANY" && node->GetInUseEventCount()))
    {
        if (node->IsLocal())
            code << "auto* ";
        code << node->get_node_name() << " = ";
    }

    // If the user doesn't want access, then we have no use for the return value.
    if (node->IsLocal())
    {
        if (node->isParent(gen_wxToolBar) || node->isParent(gen_wxAuiToolBar))
            code << node->get_parent_name() << "->AddTool(" << node->prop_as_string(prop_id) << ", ";
        else
            code << "AddTool(" << node->prop_as_string(prop_id) << ", ";
    }
    else
    {
        if (node->isParent(gen_wxToolBar) || node->isParent(gen_wxAuiToolBar))
            code << node->get_node_name() << " = " << node->get_parent_name() << "->AddTool("
                 << node->prop_as_string(prop_id) << ", ";
        else
            code << node->get_node_name() << " = AddTool(" << node->prop_as_string(prop_id) << ", ";
    }

    auto& label = node->prop_as_string(prop_label);
    if (label.size())
    {
        code << GenerateQuotedString(label);
    }
    else
    {
        code << "wxEmptyString";
    }

    if (BitmapCode.size())
    {
        code << ", " << BitmapCode;
    }
    else
    {
        code << ", " << GenerateBitmapCode(node->prop_as_string(prop_bitmap));
    }

    if (!node->HasValue(prop_tooltip) && !node->HasValue(prop_statusbar))
    {
        if (node->isGen(gen_tool_dropdown))
        {
            code << ", wxEmptyString, wxITEM_DROPDOWN";
        }
        else if (node->prop_as_string(prop_kind) != "wxITEM_NORMAL")
        {
            code << ", wxEmptyString, " << node->prop_as_string(prop_kind);
        }
    }

    else if (node->HasValue(prop_tooltip) && !node->HasValue(prop_statusbar))
    {
        code << ",\n\t\t\t" << GenerateQuotedString(node->prop_as_string(prop_tooltip));
        if (node->isGen(gen_tool_dropdown))
        {
            code << ", wxITEM_DROPDOWN";
        }
        else if (node->prop_as_string(prop_kind) != "wxITEM_NORMAL")
        {
            code << ", " << node->prop_as_string(prop_kind);
        }
    }

    else if (node->HasValue(prop_statusbar))
    {
        code << ", wxNullBitmap, ";
        code << node->prop_as_string(prop_kind) << ", \n\t\t\t";

        if (node->HasValue(prop_tooltip))
        {
            code << GenerateQuotedString(node->prop_as_string(prop_tooltip));
        }
        else
        {
            code << "wxEmptyString";
        }

        code << ", " << GenerateQuotedString(node->prop_as_string(prop_statusbar));
    }

    code << ");";

    if (node->prop_as_bool(prop_disabled))
    {
        code << "\n\t" << node->get_node_name() << "->Enable(false);";
    }

    return code;
}

ttlib::cstr GenerateWxSize(Node* node, PropName prop)
{
    ttlib::cstr code;
    auto size = node->prop_as_wxSize(prop);
    if (node->value(prop).contains("d", tt::CASE::either))
    {
        code << "ConvertDialogToPixels(wxSize(" << size.x << ", " << size.y << "))";
    }
    else
    {
        code << "wxSize(" << size.x << ", " << size.y << ")";
    }
    return code;
}
