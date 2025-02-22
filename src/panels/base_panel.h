/////////////////////////////////////////////////////////////////////////////
// Purpose:   Code generation panel
// Author:    Ralph Walden
// Copyright: Copyright (c) 2020-2022 KeyWorks Software (Ralph Walden)
// License:   Apache License -- see ../../LICENSE
/////////////////////////////////////////////////////////////////////////////

#pragma once

#include <wx/panel.h>  // Base header for wxPanel

#include "node_classes.h"  // Forward defintions of Node classes

class CodeDisplay;
class CustomEvent;
class MainFrame;

class wxAuiNotebook;
class wxFindDialogEvent;
class wxStyledTextCtrl;

class BasePanel : public wxPanel
{
public:
    BasePanel(wxWindow* parent, MainFrame* frame, int GenerateDerivedCode);
    ~BasePanel() override;

    void GenerateBaseClass();

    void OnFind(wxFindDialogEvent& event);

    void OnNodeSelected(CustomEvent& event);

protected:
private:
    CodeDisplay* m_cppPanel;
    CodeDisplay* m_hPanel;
    CodeDisplay* m_inherit_src_panel { nullptr };
    CodeDisplay* m_inherit_hdr_panel { nullptr };
    wxAuiNotebook* m_notebook;
    Node* m_cur_form { nullptr };

    int m_panel_type;
};
