/////////////////////////////////////////////////////////////////////////////
// Purpose:   Displays node creation tools in a wxRibbonBar
// Author:    Ralph Walden
// Copyright: Copyright (c) 2020-2022 KeyWorks Software (Ralph Walden)
// License:   Apache License -- see ../../LICENSE
/////////////////////////////////////////////////////////////////////////////

#include <wx/menu.h>          // wxMenu and wxMenuBar classes
#include <wx/ribbon/page.h>   // Container for ribbon-bar-style interface panels
#include <wx/ribbon/panel.h>  // Ribbon-style container for a group of related tools / controls
#include <wx/sizer.h>         // provide wxSizer class for layout

#include "ribbon_tools.h"  // auto-generated: wxui/ribbonpanel_base.h and wxui/ribbonpanel_base.cpp

#include "gen_enums.h"     // Enumerations for generators
#include "mainframe.h"     // MainFrame -- Main window frame
#include "node.h"          // Node class
#include "node_creator.h"  // NodeCreator class

#include "newdialog_base.h"  // NewDialog -- Dialog for creating a new project dialog
#include "newframe_base.h"   // NewFrame -- Dialog for creating a new project wxFrame
#include "newpanel_base.h"   // NewPanel -- Dialog for creating a new form panel
#include "newribbon_base.h"  // NewRibbon -- Dialog for creating a new wxRibbonBar
#include "newwizard_base.h"  // NewWizard -- Dialog for creating a new wizard

#include "ribbon_ids.h"

#include "menu_auibar_base.h"
#include "menu_bartools_base.h"
#include "menubutton_base.h"
#include "menucheckbox_base.h"
#include "menucombobox_base.h"
#include "menudatactrl_base.h"
#include "menulistbox_base.h"
#include "menuribbontype_base.h"
#include "menuspin_base.h"
#include "menustaticsizer_base.h"

// The base class specifies a larger size for the panel to make it easier to work with in the Mockup window. We switch that
// to a default size here.
RibbonPanel::RibbonPanel(wxWindow* parent) : RibbonPanelBase(parent) {}

void RibbonPanel::OnToolClick(wxRibbonToolBarEvent& event)
{
    size_t id = event.GetId();

    if (id == CreateNewRibbon && (!wxGetFrame().GetSelectedNode() || wxGetFrame().GetSelectedNode()->isGen(gen_Project)))
    {
        id = CreateNewFormRibbon;
    }

    if (id < gen_name_array_size)
    {
        wxGetFrame().CreateToolNode(static_cast<GenName>(event.GetId()));
    }
    else
    {
        switch (id)
        {
            case CreateNewDialog:
                {
                    NewDialog dlg(wxGetFrame().GetWindow());
                    if (dlg.ShowModal() == wxID_OK)
                    {
                        dlg.CreateNode();
                    }
                    return;
                }
                break;

            case CreateNewFrame:
                {
                    NewFrame dlg(wxGetFrame().GetWindow());
                    if (dlg.ShowModal() == wxID_OK)
                    {
                        dlg.CreateNode();
                    }
                    return;
                }
                break;

            case CreateNewPanel:
                {
                    NewPanel dlg(wxGetFrame().GetWindow());
                    dlg.WantFormVersion();
                    if (dlg.ShowModal() == wxID_OK)
                    {
                        dlg.CreateNode();
                    }
                    return;
                }
                break;

            case CreateNewRibbon:
                {
                    NewRibbon dlg(wxGetFrame().GetWindow());
                    if (dlg.IsCreatable())
                    {
                        if (dlg.ShowModal() == wxID_OK)
                        {
                            dlg.CreateNode();
                        }
                    }
                    return;
                }
                break;

            case CreateNewFormRibbon:
                {
                    NewRibbon dlg(wxGetFrame().GetWindow());
                    dlg.WantFormVersion();
                    if (dlg.ShowModal() == wxID_OK)
                    {
                        dlg.CreateNode();
                    }
                    return;
                }
                break;

            case CreateNewWizard:
                {
                    NewWizard dlg(wxGetFrame().GetWindow());
                    if (dlg.ShowModal() == wxID_OK)
                    {
                        dlg.CreateNode();
                    }
                    return;
                }
                break;
        }

        FAIL_MSG("This will only happen if the tool is a) not a dropdown, or b) doesn't have a valid id.");

        // For release build, we'll at least attempt to create it in case the help string specifies a widget.
        auto name = event.GetBar()->GetToolHelpString(event.GetId());
        if (auto result = rmap_GenNames.find(name.utf8_string()); result != rmap_GenNames.end())
        {
            wxGetFrame().CreateToolNode(result->second);
        }
    }
}

void RibbonPanel::OnDropDown(wxRibbonToolBarEvent& event)
{
    switch (event.GetId())
    {
        case NewCheckbox:
            {
                MenuCheckbox popup_menu;
                popup_menu.Bind(wxEVT_MENU, &RibbonPanel::OnMenuEvent, this, wxID_ANY);
                event.PopupMenu(&popup_menu);
                return;
            }

        case BarTools:
            {
                const auto* cur_sel = wxGetFrame().GetSelectedNode();
                if (cur_sel && (cur_sel->isGen(gen_wxAuiToolBar) || cur_sel->GetParent()->isGen(gen_wxAuiToolBar)))
                {
                    MenuAuiBar popup_menu;
                    popup_menu.Bind(wxEVT_MENU, &RibbonPanel::OnMenuEvent, this, wxID_ANY);
                    event.PopupMenu(&popup_menu);
                }
                else
                {
                    MenuBarTools popup_menu;
                    popup_menu.Bind(wxEVT_MENU, &RibbonPanel::OnMenuEvent, this, wxID_ANY);
                    event.PopupMenu(&popup_menu);
                }
                return;
            }

        case AuiBarTools:
            {
                const auto* cur_sel = wxGetFrame().GetSelectedNode();
                if (cur_sel && (cur_sel->isGen(gen_wxToolBar) || cur_sel->isGen(gen_ToolBar) ||
                                cur_sel->GetParent()->isGen(gen_wxToolBar) || cur_sel->GetParent()->isGen(gen_ToolBar)))
                {
                    MenuBarTools popup_menu;
                    popup_menu.Bind(wxEVT_MENU, &RibbonPanel::OnMenuEvent, this, wxID_ANY);
                    event.PopupMenu(&popup_menu);
                }
                else
                {
                    MenuAuiBar popup_menu;
                    popup_menu.Bind(wxEVT_MENU, &RibbonPanel::OnMenuEvent, this, wxID_ANY);
                    event.PopupMenu(&popup_menu);
                }
                return;
            }

        case NewCombobox:
            {
                MenuCombobox popup_menu;
                popup_menu.Bind(wxEVT_MENU, &RibbonPanel::OnMenuEvent, this, wxID_ANY);
                event.PopupMenu(&popup_menu);
                return;
            }

        case NewListbox:
            {
                MenuListbox popup_menu;
                popup_menu.Bind(wxEVT_MENU, &RibbonPanel::OnMenuEvent, this, wxID_ANY);
                event.PopupMenu(&popup_menu);
                return;
            }

        case NewButton:
            {
                MenuButton popup_menu;
                popup_menu.Bind(wxEVT_MENU, &RibbonPanel::OnMenuEvent, this, wxID_ANY);
                event.PopupMenu(&popup_menu);
                return;
            }

        case NewSpin:
            {
                MenuSpin popup_menu;
                popup_menu.Bind(wxEVT_MENU, &RibbonPanel::OnMenuEvent, this, wxID_ANY);
                event.PopupMenu(&popup_menu);
                return;
            }

        case NewDataCtrl:
            {
                MenuDataCtrl popup_menu;
                popup_menu.Bind(wxEVT_MENU, &RibbonPanel::OnMenuEvent, this, wxID_ANY);
                event.PopupMenu(&popup_menu);
                return;
            }

        case NewRibbonType:
            {
                MenuRibbonType popup_menu;
                popup_menu.Bind(wxEVT_MENU, &RibbonPanel::OnMenuEvent, this, wxID_ANY);
                event.PopupMenu(&popup_menu);
                return;
            }

        case NewStaticSizer:
            {
                MenuStaticSizer popup_menu;
                popup_menu.Bind(wxEVT_MENU, &RibbonPanel::OnMenuEvent, this, wxID_ANY);
                event.PopupMenu(&popup_menu);
                return;
            }

        case NewScrolled:
            {
                wxMenu menu;
                menu.Append(gen_wxScrolledWindow, "Insert wxScrolled<wxPanel> (wxScrolledWindow)");
                menu.Append(gen_wxScrolledCanvas, "Insert wxScrolled<wxWindow> (wxScrolledCanvas)");

                menu.Bind(wxEVT_MENU, &RibbonPanel::OnMenuEvent, this, wxID_ANY);
                event.PopupMenu(&menu);
                return;
            }

        default:
            return;
    }
}

void RibbonPanel::OnMenuEvent(wxCommandEvent& event)
{
    ASSERT_MSG(static_cast<GenName>(event.GetId()) < gen_name_array_size, "Invalid gen_ id!")

    wxGetFrame().CreateToolNode(static_cast<GenName>(event.GetId()));
}
