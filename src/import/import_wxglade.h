/////////////////////////////////////////////////////////////////////////////
// Purpose:   Import a wxGlade file
// Author:    Ralph Walden
// Copyright: Copyright (c) 2021 KeyWorks Software (Ralph Walden)
// License:   Apache License -- see ../../LICENSE
/////////////////////////////////////////////////////////////////////////////

#pragma once

#include "node_classes.h"

#include "import_xml.h"  // ImportXML -- Base class for XML importing

class WxGlade : public ImportXML
{
public:
    WxGlade();

    bool Import(const ttString& filename, bool write_doc = true) override;

protected:
    NodeSharedPtr CreateGladeNode(pugi::xml_node& xml_obj, Node* parent, Node* sizeritem = nullptr);

private:
};
