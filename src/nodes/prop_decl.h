/////////////////////////////////////////////////////////////////////////////
// Purpose:   PropChildDeclaration and PropDeclaration classes
// Author:    Ralph Walden
// Copyright: Copyright (c) 2020-2021 KeyWorks Software (Ralph Walden)
// License:   Apache License -- see ../../LICENSE
/////////////////////////////////////////////////////////////////////////////

#pragma once

#include "gen_enums.h"  // Enumerations for generators
using namespace GenEnum;

struct PropChildDeclaration
{
    ttlib::cstr m_def_value;
    ttlib::cstr m_help;

    // BUGBUG: [KeyWorks - 04-09-2021] NodeCreator::ParseProperties does not initialize the following for parent properties

    const char* name_str() const noexcept { return m_name_str; }
    PropName get_name() const noexcept { return m_name_enum; }
    PropType get_type() const noexcept { return m_prop_type; }

    bool isType(PropType type) const noexcept { return (type == m_prop_type); }
    bool isProp(PropName name) const noexcept { return (name == m_name_enum); }

    GenEnum::PropType m_prop_type;
    GenEnum::PropName m_name_enum;  // enumeration value for the name
    const char* m_name_str;         // const char* pointer to the name
};

class PropDeclaration : public PropChildDeclaration
{
public:
    PropDeclaration(GenEnum::PropName prop_name, GenEnum::PropType prop_type, std::string_view def_value,
                    std::string_view help, std::string_view customEditor)
    {
        m_def_value = def_value;
        m_help = help;
        m_customEditor = customEditor;

        m_prop_type = prop_type;
        m_name_enum = prop_name;
        m_name_str = GenEnum::map_PropNames[m_name_enum];
    }

    // Returns a char pointer to the name. Use get_name() if you want the enum value.
    ttlib::sview DeclName() const noexcept { return m_name_str; }

    const ttlib::cstr& GetDefaultValue() const noexcept { return m_def_value; }
    const ttlib::cstr& GetDescription() const noexcept { return m_help; }
    const ttlib::cstr& GetCustomEditor() const noexcept { return m_customEditor; }

    // These get used to setup wxPGProperty, so both key and value need to be a wxString
    struct Options
    {
        ttString name;
        ttString help;
    };

    std::vector<Options>& GetOptions() { return m_options; }

private:
    ttlib::cstr m_customEditor;  // an optional custom editor for the property grid

    // This gets used to setup wxPGProperty, so both key and value need to be a wxString
    std::vector<Options> m_options;
};
