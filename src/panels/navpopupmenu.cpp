/////////////////////////////////////////////////////////////////////////////
// Purpose:   Context-menu for Navigation Panel
// Author:    Ralph Walden
// Copyright: Copyright (c) 2020-2022 KeyWorks Software (Ralph Walden)
// License:   Apache License -- see ../../LICENSE
/////////////////////////////////////////////////////////////////////////////

#include <wx/artprov.h>   // wxArtProvider class
#include <wx/wupdlock.h>  // wxWindowUpdateLocker prevents window redrawing

#include "navpopupmenu.h"  // NavPopupMenu

#include "base_generator.h"  // BaseGenerator -- Base widget generator class
#include "bitmaps.h"         // Contains various images handling functions
#include "clipboard.h"       // Handles reading and writing OS clipboard data
#include "mainframe.h"       // MainFrame -- Main window frame
#include "mainframe_base.h"  // contains all the wxue_img namespace embedded images
#include "nav_panel.h"       // NavigationPanel -- Navigation Panel
#include "node.h"            // Node class
#include "node_creator.h"    // NodeCreator class
#include "undo_cmds.h"       // InsertNodeAction -- Undoable command classes derived from UndoAction
#include "utils.h"           // Utility functions that work with properties

#include "newdialog_base.h"  // NewDialog -- Dialog for creating a new project dialog
#include "newframe_base.h"   // NewFrame -- Dialog for creating a new project wxFrame
#include "newwizard_base.h"  // NewWizard -- Dialog for creating a new wizard

NavPopupMenu::NavPopupMenu(Node* node) : m_node(node)
{
    if (!node)
    {
        ASSERT(node);
        return;  // theoretically impossible, but don't crash if it happens
    }

    if (node->GetParent() && (node->GetParent()->isGen(gen_wxToolBar) || node->GetParent()->isGen(gen_wxAuiToolBar)))
    {
        m_is_parent_toolbar = true;
    }

    if (node->IsSizer())
    {
        CreateSizerMenu(node);
    }
    else
    {
        CreateCommonMenu(node);
    }
}

void NavPopupMenu::OnMenuEvent(wxCommandEvent& event)
{
    switch (event.GetId())
    {
        case MenuNEW_ITEM:
            if (m_tool_name < gen_name_array_size)
            {
                if (m_node->isType(type_bookpage) || m_node->isType(type_wizardpagesimple))
                {
                    if (m_child && m_child->IsSizer())
                    {
                        m_child = m_child->GetParent();
                    }
                }

                if (m_child)
                    m_child->CreateToolNode(m_tool_name);
                else
                    wxGetFrame().CreateToolNode(m_tool_name);
            }
            break;

        case MenuNEW_COLUMN:
            if (m_tool_name == gen_wxTreeListCtrl)
            {
                if (m_child)
                    m_child->CreateToolNode(gen_TreeListCtrlColumn);
                else
                    wxGetFrame().CreateToolNode(gen_TreeListCtrlColumn);
            }
            break;

        case MenuNEW_SIBLING_BOX_SIZER:
            m_node->GetParent()->CreateToolNode(gen_wxBoxSizer);
            break;

        case MenuNEW_SIBLING_STATIC_SIZER:
            m_node->GetParent()->CreateToolNode(gen_wxStaticBoxSizer);
            break;

        case MenuNEW_SIBLING_WRAP_SIZER:
            m_node->GetParent()->CreateToolNode(gen_wxWrapSizer);
            break;

        case MenuNEW_SIBLING_GRID_SIZER:
            m_node->GetParent()->CreateToolNode(gen_wxGridSizer);
            break;

        case MenuNEW_SIBLING_FLEX_GRID_SIZER:
            m_node->GetParent()->CreateToolNode(gen_wxFlexGridSizer);
            break;

        case MenuNEW_SIBLING_GRIDBAG_SIZER:
            m_node->GetParent()->CreateToolNode(gen_wxGridBagSizer);
            break;

        case MenuNEW_SIBLING_STD_DIALG_BTNS:
            m_node->GetParent()->CreateToolNode(gen_wxStdDialogButtonSizer);
            break;

        case MenuNEW_SIBLING_SPACER:
            m_node->GetParent()->CreateToolNode(gen_spacer);
            break;

        case MenuNEW_CHILD_BOX_SIZER:
            m_sizer_node->CreateToolNode(gen_wxBoxSizer);
            break;

        case MenuNEW_CHILD_STATIC_SIZER:
            m_sizer_node->CreateToolNode(gen_wxStaticBoxSizer);
            break;

        case MenuNEW_CHILD_WRAP_SIZER:
            m_sizer_node->CreateToolNode(gen_wxWrapSizer);
            break;

        case MenuNEW_CHILD_GRID_SIZER:
            m_sizer_node->CreateToolNode(gen_wxGridSizer);
            break;

        case MenuNEW_CHILD_FLEX_GRID_SIZER:
            m_sizer_node->CreateToolNode(gen_wxFlexGridSizer);
            break;

        case MenuNEW_CHILD_GRIDBAG_SIZER:
            m_sizer_node->CreateToolNode(gen_wxGridBagSizer);
            break;

        case MenuNEW_CHILD_STD_DIALG_BTNS:
            wxGetFrame().CreateToolNode(gen_wxStdDialogButtonSizer);
            break;

        case MenuNEW_CHILD_SPACER:
            wxGetFrame().CreateToolNode(gen_spacer);
            break;

        case MenuNEW_TOOLBAR:
            wxGetFrame().CreateToolNode(gen_wxToolBar);
            break;

        case MenuNEW_INFOBAR:
            wxGetFrame().CreateToolNode(gen_wxInfoBar);
            break;

        case MenuADD_MENU:
            wxGetFrame().CreateToolNode(gen_wxMenu);
            break;

        case MenuADD_TOOL_SEPARATOR:
            if (m_child)
                m_child->CreateToolNode(gen_toolSeparator);
            else
                wxGetFrame().CreateToolNode(gen_toolSeparator);
            break;

        case MenuEXPAND_ALL:
            wxGetFrame().GetNavigationPanel()->ExpandCollapse(m_node);
            wxGetFrame().SelectNode(m_node);
            break;

        case wxID_CUT:
            wxGetFrame().CutNode(m_node);
            break;

        case wxID_COPY:
            wxGetFrame().CopyNode(m_node);
            break;

        case wxID_PASTE:
            wxGetFrame().PasteNode(m_node);
            break;

        case MenuDUPLICATE:
            wxGetFrame().DuplicateNode(m_node);
            break;

        case wxID_DELETE:
            wxGetFrame().DeleteNode(m_node);
            break;

        case MenuMOVE_UP:
            wxGetFrame().MoveNode(m_node, MoveDirection::Up);
            break;

        case MenuMOVE_DOWN:
            wxGetFrame().MoveNode(m_node, MoveDirection::Down);
            break;

        case MenuMOVE_RIGHT:
            wxGetFrame().MoveNode(m_node, MoveDirection::Right);
            break;

        case MenuMOVE_LEFT:
            wxGetFrame().MoveNode(m_node, MoveDirection::Left);
            break;

        case MenuNEW_PARENT_BOX_SIZER:
            CreateSizerParent(m_node, "wxBoxSizer");
            break;

        case MenuNEW_PARENT_STATIC_SIZER:
            CreateSizerParent(m_node, "wxStaticBoxSizer");
            break;

        case MenuNEW_PARENT_WRAP_SIZER:
            CreateSizerParent(m_node, "wxWrapSizer");
            break;

        case MenuNEW_PARENT_GRID_SIZER:
            CreateSizerParent(m_node, "wxGridSizer");
            break;

        case MenuNEW_PARENT_FLEX_GRID_SIZER:
            CreateSizerParent(m_node, "wxFlexGridSizer");
            break;

        case MenuNEW_PARENT_GRIDBAG_SIZER:
            CreateSizerParent(m_node, "wxGridBagSizer");
            break;

        case MenuNEW_PARENT_FOLDER:
            CreateSizerParent(m_node, "folder");
            break;

        case MenuChangeTo_FLEX_GRID_SIZER:
            ChangeSizer(gen_wxFlexGridSizer);
            break;

        case MenuChangeTo_AUI_BOOK:
            ChangeNode(gen_wxAuiNotebook);
            break;

        case MenuChangeTo_CHOICE_BOOK:
            ChangeNode(gen_wxChoicebook);
            break;

        case MenuChangeTo_LIST_BOOK:
            ChangeNode(gen_wxListbook);
            break;

        case MenuChangeTo_NOTE_BOOK:
            ChangeNode(gen_wxNotebook);
            break;

        case MenuChangeTo_SIMPLE_BOOK:
            ChangeNode(gen_wxSimplebook);
            break;

        case MenuChangeTo_2STATE_CHECKBOX:
            ChangeNode(gen_wxCheckBox);
            break;

        case MenuChangeTo_3STATE_CHECKBOX:
            ChangeNode(gen_Check3State);
            break;

        case MenuChangeTo_RADIO_BUTTON:
            ChangeNode(gen_wxRadioButton);
            break;

        case MenuChangeTo_CHOICE_BOX:
            ChangeNode(gen_wxChoice);
            break;

        case MenuChangeTo_COMBO_BOX:
            ChangeNode(gen_wxComboBox);
            break;

        case MenuChangeTo_LIST_BOX:
            ChangeNode(gen_wxListBox);
            break;

        case MenuChangeTo_GRID_SIZER:
            ChangeSizer(gen_wxGridSizer);
            break;

        case MenuChangeTo_STATIC_SIZER:
            ChangeSizer(gen_wxStaticBoxSizer);
            break;

        case MenuChangeTo_WRAP_SIZER:
            ChangeSizer(gen_wxWrapSizer);
            break;

        case MenuADD_PAGE:
            if (m_node->isGen(gen_BookPage))
            {
                m_node->GetParent()->CreateToolNode(gen_BookPage);
            }
            else if (m_node->isGen(gen_wxWizardPageSimple))
            {
                m_node->GetParent()->CreateToolNode(gen_wxWizardPageSimple);
            }
            else
            {
                wxGetFrame().CreateToolNode(gen_wxPanel);
            }
            break;

        default:
            break;
    }
}

void NavPopupMenu::OnUpdateEvent(wxUpdateUIEvent& event)
{
    switch (event.GetId())
    {
        case wxID_CUT:
        case wxID_COPY:
            if (m_node && !m_node->isGen(gen_Project))
                event.Enable(true);
            else
                event.Enable(wxGetFrame().CanCopyNode());
            break;

        case MenuMOVE_UP:
            Enable(MenuMOVE_UP, wxGetFrame().MoveNode(m_node, MoveDirection::Up, true));
            break;

        case MenuMOVE_DOWN:
            Enable(MenuMOVE_DOWN, wxGetFrame().MoveNode(m_node, MoveDirection::Down, true));
            break;

        case MenuMOVE_LEFT:
            Enable(MenuMOVE_LEFT, wxGetFrame().MoveNode(m_node, MoveDirection::Left, true));
            break;

        case MenuMOVE_RIGHT:
            Enable(MenuMOVE_RIGHT, wxGetFrame().MoveNode(m_node, MoveDirection::Right, true));
            break;
    }
}

void NavPopupMenu::CreateSizerMenu(Node* node)
{
    // This needs to be added first to cover all menu ids that aren't specically bound to an id.
    Bind(wxEVT_MENU, &NavPopupMenu::OnMenuEvent, this, wxID_ANY);
    Bind(wxEVT_UPDATE_UI, &NavPopupMenu::OnUpdateEvent, this);

    m_sizer_node = node;

    bool isTopSizer = (node->GetParent()->IsForm() || node->GetParent()->IsContainer());
    wxMenuItem* menu_item;
    wxMenu* sub_menu;

    sub_menu = new wxMenu;
    menu_item = sub_menu->Append(MenuNEW_CHILD_BOX_SIZER, "wxBoxSizer");
    menu_item->SetBitmap(GetInternalImage("sizer_horizontal"));
    menu_item = sub_menu->Append(MenuNEW_CHILD_STATIC_SIZER, "wxStaticBoxSizer");
    menu_item->SetBitmap(GetInternalImage("wxStaticBoxSizer"));
    menu_item = sub_menu->Append(MenuNEW_CHILD_WRAP_SIZER, "wxWrapSizer");
    menu_item->SetBitmap(GetInternalImage("wrap_sizer"));
    menu_item = sub_menu->Append(MenuNEW_CHILD_GRID_SIZER, "wxGridSizer");
    menu_item->SetBitmap(GetInternalImage("grid_sizer"));
    menu_item = sub_menu->Append(MenuNEW_CHILD_FLEX_GRID_SIZER, "wxFlexGridSizer");
    menu_item->SetBitmap(GetInternalImage("flex_grid_sizer"));
    menu_item = sub_menu->Append(MenuNEW_CHILD_GRIDBAG_SIZER, "wxGridBagSizer");
    menu_item->SetBitmap(GetInternalImage("grid_bag_sizer"));

    AppendSubMenu(sub_menu, "Add child sizer");

    if (!isTopSizer)
    {
        sub_menu = new wxMenu;
        menu_item = sub_menu->Append(MenuNEW_SIBLING_BOX_SIZER, "wxBoxSizer");
        menu_item->SetBitmap(GetInternalImage("sizer_horizontal"));
        menu_item = sub_menu->Append(MenuNEW_SIBLING_STATIC_SIZER, "wxStaticBoxSizer");
        menu_item->SetBitmap(GetInternalImage("wxStaticBoxSizer"));
        menu_item = sub_menu->Append(MenuNEW_SIBLING_WRAP_SIZER, "wxWrapSizer");
        menu_item->SetBitmap(GetInternalImage("wrap_sizer"));
        menu_item = sub_menu->Append(MenuNEW_SIBLING_GRID_SIZER, "wxGridSizer");
        menu_item->SetBitmap(GetInternalImage("grid_sizer"));
        menu_item = sub_menu->Append(MenuNEW_SIBLING_FLEX_GRID_SIZER, "wxFlexGridSizer");
        menu_item->SetBitmap(GetInternalImage("flex_grid_sizer"));
        menu_item = sub_menu->Append(MenuNEW_SIBLING_GRIDBAG_SIZER, "wxGridBagSizer");
        menu_item->SetBitmap(GetInternalImage("grid_bag_sizer"));
        AppendSubMenu(sub_menu, "Add sibling sizer");

        AppendSeparator();
        MenuAddMoveCommands(node);
    }

    if (node->isGen(gen_wxBoxSizer))
    {
        sub_menu = new wxMenu;
        menu_item = sub_menu->Append(MenuChangeTo_FLEX_GRID_SIZER, "wxFlexGridSizer");
        menu_item->SetBitmap(GetInternalImage("flex_grid_sizer"));
        menu_item = sub_menu->Append(MenuChangeTo_GRID_SIZER, "wxGridSizer");
        menu_item->SetBitmap(GetInternalImage("grid_sizer"));
        menu_item = sub_menu->Append(MenuChangeTo_STATIC_SIZER, "wxStaticBoxSizer");
        menu_item->SetBitmap(GetInternalImage("wxStaticBoxSizer"));
        menu_item = sub_menu->Append(MenuChangeTo_WRAP_SIZER, "wxWrapSizer");
        menu_item->SetBitmap(GetInternalImage("wrap_sizer"));

        AppendSubMenu(sub_menu, "Change Sizer To");
    }
    else if (node->isGen(gen_wxGridSizer))
    {
        sub_menu = new wxMenu;
        menu_item = sub_menu->Append(MenuChangeTo_FLEX_GRID_SIZER, "wxFlexGridSizer");
        menu_item->SetBitmap(GetInternalImage("flex_grid_sizer"));
        menu_item = sub_menu->Append(MenuChangeTo_WRAP_SIZER, "wxWrapSizer");
        menu_item->SetBitmap(GetInternalImage("wrap_sizer"));

        AppendSubMenu(sub_menu, "Change Sizer To");
    }
    else if (node->isGen(gen_wxFlexGridSizer))
    {
        sub_menu = new wxMenu;
        menu_item = sub_menu->Append(MenuChangeTo_GRID_SIZER, "wxGridSizer");
        menu_item->SetBitmap(GetInternalImage("grid_sizer"));
        menu_item = sub_menu->Append(MenuChangeTo_WRAP_SIZER, "wxWrapSizer");
        menu_item->SetBitmap(GetInternalImage("wrap_sizer"));

        AppendSubMenu(sub_menu, "Change Sizer To");
    }
    else if (node->isGen(gen_wxWrapSizer))
    {
        sub_menu = new wxMenu;
        menu_item = sub_menu->Append(MenuChangeTo_FLEX_GRID_SIZER, "wxFlexGridSizer");
        menu_item->SetBitmap(GetInternalImage("flex_grid_sizer"));
        menu_item = sub_menu->Append(MenuChangeTo_GRID_SIZER, "wxGridSizer");
        menu_item->SetBitmap(GetInternalImage("grid_sizer"));

        AppendSubMenu(sub_menu, "Change Sizer To");
    }

    AppendSeparator();
    MenuAddStandardCommands(node);
}

void NavPopupMenu::CreateCommonMenu(Node* node)
{
    // This needs to be added first to cover all menu ids that aren't specically bound to an id.
    Bind(wxEVT_MENU, &NavPopupMenu::OnMenuEvent, this, wxID_ANY);
    Bind(wxEVT_UPDATE_UI, &NavPopupMenu::OnUpdateEvent, this);

    MenuAddCommands(node);
    MenuAddMoveCommands(node);
    AppendSeparator();
    MenuAddStandardCommands(node);
}

void NavPopupMenu::MenuAddCommands(Node* node)
{
    if (node->IsForm())
    {
        if (!node->isGen(gen_wxWizard))
        {
            return;
        }
    }

    if (node->isGen(gen_wxStatusBar) || node->isGen(gen_embedded_image))
    {
        return;
    }

    if (auto gen = node->GetGenerator(); gen)
    {
        if (gen->PopupMenuAddCommands(this, node))
        {
            AppendSeparator();
            return;
        }
    }

    bool add_sizer = true;

    switch (node->gen_name())
    {
        case gen_wxAuiNotebook:
        case gen_wxChoicebook:
        case gen_wxListbook:
        case gen_wxNotebook:
        case gen_wxSimplebook:
        case gen_wxToolbook:
        case gen_wxTreebook:
            add_sizer = false;
            Append(MenuADD_PAGE, "Add Page");
            Bind(
                wxEVT_MENU,
                [](wxCommandEvent&)
                {
                    wxGetFrame().CreateToolNode(gen_BookPage);
                },
                MenuADD_PAGE);
            break;

        case gen_wxRibbonBar:
        case gen_RibbonBar:
            add_sizer = false;
            Append(MenuADD_RIBBON_PAGE, "Add Page");
            Bind(
                wxEVT_MENU,
                [](wxCommandEvent&)
                {
                    wxGetFrame().CreateToolNode(gen_wxRibbonPage);
                },
                MenuADD_RIBBON_PAGE);
            break;

        case gen_wxRibbonPage:
            add_sizer = false;
            Append(MenuADD_RIBBON_PANEL, "Add Panel");
            Bind(
                wxEVT_MENU,
                [](wxCommandEvent&)
                {
                    wxGetFrame().CreateToolNode(gen_wxRibbonPanel);
                },
                MenuADD_RIBBON_PANEL);
            break;

        case gen_wxRibbonPanel:
            if (node->GetChildCount())
            {
                return;
            }
            else
            {
                Append(MenuADD_RIBBON_BUTTONBAR, "Add Button Bar");
                Bind(
                    wxEVT_MENU,
                    [](wxCommandEvent&)
                    {
                        wxGetFrame().CreateToolNode(gen_wxRibbonButtonBar);
                    },
                    MenuADD_RIBBON_BUTTONBAR);

                Append(MenuADD_RIBBON_TOOLBAR, "Add Tool Bar");
                Bind(
                    wxEVT_MENU,
                    [](wxCommandEvent&)
                    {
                        wxGetFrame().CreateToolNode(gen_wxRibbonToolBar);
                    },
                    MenuADD_RIBBON_TOOLBAR);

                Append(MenuADD_RIBBON_GALLERY, "Add Gallery");
                Bind(
                    wxEVT_MENU,
                    [](wxCommandEvent&)
                    {
                        wxGetFrame().CreateToolNode(gen_wxRibbonGallery);
                    },
                    MenuADD_RIBBON_GALLERY);
            }
            break;

        case gen_wxAuiToolBar:
        case gen_auitool:
            add_sizer = false;
            AddToolbarCommands(node);
            break;

        case gen_wxToolBar:
        case gen_ToolBar:
        case gen_tool:
        case gen_toolSeparator:
        case gen_toolStretchable:
            add_sizer = false;
            AddToolbarCommands(node);
            break;

        case gen_wxMenuBar:
        case gen_MenuBar:
            add_sizer = false;
            Append(MenuADD_MENU, "Add Menu");
            Bind(
                wxEVT_MENU,
                [](wxCommandEvent&)
                {
                    wxGetFrame().CreateToolNode(gen_wxMenu);
                },
                MenuADD_MENU);
            break;

        case gen_wxMenu:
        case gen_wxMenuItem:
        case gen_submenu:
        case gen_separator:
        case gen_tool_dropdown:
            add_sizer = false;
            Append(MenuADD_MENUITEM, "Add Menu Item");
            Bind(
                wxEVT_MENU,
                [](wxCommandEvent&)
                {
                    wxGetFrame().CreateToolNode(gen_wxMenuItem);
                },
                MenuADD_MENUITEM);

            Append(MenuADD_SUBMENU, "Add Submenu");
            Bind(
                wxEVT_MENU,
                [](wxCommandEvent&)
                {
                    wxGetFrame().CreateToolNode(gen_submenu);
                },
                MenuADD_SUBMENU);

            Append(MenuADD_MENU_SEPARATOR, "Add Separator");
            Bind(
                wxEVT_MENU,
                [](wxCommandEvent&)
                {
                    wxGetFrame().CreateToolNode(gen_separator);
                },
                MenuADD_MENU_SEPARATOR);

            break;

        default:
            if (m_is_parent_toolbar)
            {
                add_sizer = false;
                AddToolbarCommands(node);
            }
            else
            {
                Append(MenuNEW_CHILD_SPACER, "Add spacer");
            }
            break;
    }

    if (add_sizer)
    {
        MenuAddChildSizerCommands(node);
    }

    if (!node->isGen(gen_Project))
    {
        AppendSeparator();
    }
}

void NavPopupMenu::MenuAddChildSizerCommands(Node* child)
{
    m_sizer_node = child;
    auto sub_menu = new wxMenu;
    auto menu_item = sub_menu->Append(MenuNEW_CHILD_BOX_SIZER, "wxBoxSizer");
    menu_item->SetBitmap(GetInternalImage("sizer_horizontal"));
    menu_item = sub_menu->Append(MenuNEW_CHILD_STATIC_SIZER, "wxStaticBoxSizer");
    menu_item->SetBitmap(GetInternalImage("wxStaticBoxSizer"));
    menu_item = sub_menu->Append(MenuNEW_CHILD_WRAP_SIZER, "wxWrapSizer");
    menu_item->SetBitmap(GetInternalImage("wrap_sizer"));
    menu_item = sub_menu->Append(MenuNEW_CHILD_GRID_SIZER, "wxGridSizer");
    menu_item->SetBitmap(GetInternalImage("grid_sizer"));
    menu_item = sub_menu->Append(MenuNEW_CHILD_FLEX_GRID_SIZER, "wxFlexGridSizer");
    menu_item->SetBitmap(GetInternalImage("flex_grid_sizer"));
    menu_item = sub_menu->Append(MenuNEW_CHILD_GRIDBAG_SIZER, "wxGridBagSizer");
    menu_item->SetBitmap(GetInternalImage("grid_bag_sizer"));

    if (child->isParent(gen_wxDialog))
    {
        sub_menu->AppendSeparator();
        menu_item = sub_menu->Append(MenuNEW_CHILD_STD_DIALG_BTNS, "wxStdDialogButtonSizer");
        menu_item->SetBitmap(GetInternalImage("stddialogbuttonsizer"));
    }

    AppendSubMenu(sub_menu, "Add sizer");
}

void NavPopupMenu::MenuAddMoveCommands(Node* node)
{
    if (node->isGen(gen_Project))
    {
        return;
    }

    wxMenuItem* menu_item;
    wxMenu* sub_menu;

    sub_menu = new wxMenu;
    menu_item = sub_menu->Append(MenuMOVE_UP, "Up\tAlt+Up", "Moves selected item up");
    menu_item->SetBitmap(wxArtProvider::GetBitmapBundle(wxART_GO_UP, wxART_MENU));
    menu_item = sub_menu->Append(MenuMOVE_DOWN, "Down\tAlt+Down", "Moves selected item down");
    menu_item->SetBitmap(wxArtProvider::GetBitmapBundle(wxART_GO_DOWN, wxART_MENU));

    auto gen = node->GetGenerator();
    if (!m_is_parent_toolbar && gen && gen->CanChangeParent(node))
    {
        menu_item = sub_menu->Append(MenuMOVE_LEFT, "Left\tAlt+Left", "Moves selected item left");
        menu_item->SetBitmap(wxArtProvider::GetBitmapBundle(wxART_GO_BACK, wxART_MENU));
        menu_item = sub_menu->Append(MenuMOVE_RIGHT, "Right\tAlt+Right", "Moves selected item right");
        menu_item->SetBitmap(wxArtProvider::GetBitmapBundle(wxART_GO_FORWARD, wxART_MENU));
    }
    AppendSubMenu(sub_menu, "Move");

    if (!m_is_parent_toolbar && gen && gen->CanChangeParent(node))
    {
        sub_menu = new wxMenu;
        menu_item = sub_menu->Append(MenuNEW_PARENT_BOX_SIZER, "wxBoxSizer");
        menu_item->SetBitmap(GetInternalImage("sizer_horizontal"));
        menu_item = sub_menu->Append(MenuNEW_PARENT_STATIC_SIZER, "wxStaticBoxSizer");
        menu_item->SetBitmap(GetInternalImage("wxStaticBoxSizer"));
        menu_item = sub_menu->Append(MenuNEW_PARENT_WRAP_SIZER, "wxWrapSizer");
        menu_item->SetBitmap(GetInternalImage("wrap_sizer"));
        menu_item = sub_menu->Append(MenuNEW_PARENT_GRID_SIZER, "wxGridSizer");
        menu_item->SetBitmap(GetInternalImage("grid_sizer"));
        menu_item = sub_menu->Append(MenuNEW_PARENT_FLEX_GRID_SIZER, "wxFlexGridSizer");
        menu_item->SetBitmap(GetInternalImage("flex_grid_sizer"));
        menu_item = sub_menu->Append(MenuNEW_PARENT_GRIDBAG_SIZER, "wxGridBagSizer");
        menu_item->SetBitmap(GetInternalImage("grid_bag_sizer"));

        AppendSubMenu(sub_menu, "&Move into new sizer");
    }
    else if (node->IsForm())
    {
        menu_item = sub_menu->Append(MenuNEW_PARENT_FOLDER, "Move into new folder");
        menu_item->SetBitmap(GetInternalImage("folder"));
    }

    if (node->isGen(gen_wxRadioButton))
    {
        sub_menu = new wxMenu;
        menu_item = sub_menu->Append(MenuChangeTo_2STATE_CHECKBOX, "2-state wxCheckBox");
        menu_item->SetBitmap(GetInternalImage("wxCheckBox"));
        menu_item = sub_menu->Append(MenuChangeTo_3STATE_CHECKBOX, "3-state wxCheckBox");
        menu_item->SetBitmap(GetInternalImage("check3state"));
        AppendSubMenu(sub_menu, "&Change widget to");
    }
    else if (node->isGen(gen_wxCheckBox))
    {
        sub_menu = new wxMenu;
        menu_item = sub_menu->Append(MenuChangeTo_3STATE_CHECKBOX, "3-state wxCheckBox");
        menu_item->SetBitmap(GetInternalImage("check3state"));
        menu_item = sub_menu->Append(MenuChangeTo_RADIO_BUTTON, "wxRadioButton");
        menu_item->SetBitmap(GetInternalImage("wxRadioButton"));
        AppendSubMenu(sub_menu, "&Change widget to");
    }
    else if (node->isGen(gen_Check3State))
    {
        sub_menu = new wxMenu;
        menu_item = sub_menu->Append(MenuChangeTo_2STATE_CHECKBOX, "2-state wxCheckBox");
        menu_item->SetBitmap(GetInternalImage("wxCheckBox"));
        menu_item = sub_menu->Append(MenuChangeTo_RADIO_BUTTON, "wxRadioButton");
        menu_item->SetBitmap(GetInternalImage("wxRadioButton"));
        AppendSubMenu(sub_menu, "&Change widget to");
    }
    else if (node->isGen(gen_wxChoice))
    {
        sub_menu = new wxMenu;
        menu_item = sub_menu->Append(MenuChangeTo_COMBO_BOX, "wxComboBox");
        menu_item->SetBitmap(GetInternalImage("wxComboBox"));
        menu_item = sub_menu->Append(MenuChangeTo_LIST_BOX, "wxListBox");
        menu_item->SetBitmap(GetInternalImage("wxListBox"));
        AppendSubMenu(sub_menu, "&Change widget to");
    }
    else if (node->isGen(gen_wxComboBox))
    {
        sub_menu = new wxMenu;
        menu_item = sub_menu->Append(MenuChangeTo_CHOICE_BOX, "wxChoice");
        menu_item->SetBitmap(GetInternalImage("wxChoice"));
        menu_item = sub_menu->Append(MenuChangeTo_LIST_BOX, "wxListBox");
        menu_item->SetBitmap(GetInternalImage("wxListBox"));
        AppendSubMenu(sub_menu, "&Change widget to");
    }
    else if (node->isGen(gen_wxListBox))
    {
        sub_menu = new wxMenu;
        menu_item = sub_menu->Append(MenuChangeTo_CHOICE_BOX, "wxChoice");
        menu_item->SetBitmap(GetInternalImage("wxChoice"));
        menu_item = sub_menu->Append(MenuChangeTo_COMBO_BOX, "wxComboBox");
        menu_item->SetBitmap(GetInternalImage("wxComboBox"));
        AppendSubMenu(sub_menu, "&Change widget to");
    }
    else if (node->isGen(gen_wxAuiNotebook))
    {
        sub_menu = new wxMenu;
        menu_item = sub_menu->Append(MenuChangeTo_CHOICE_BOOK, "wxChoicebook");
        menu_item->SetBitmap(GetInternalImage("wxChoicebook"));
        menu_item = sub_menu->Append(MenuChangeTo_LIST_BOOK, "wxListbook");
        menu_item->SetBitmap(GetInternalImage("wxListbook"));
        menu_item = sub_menu->Append(MenuChangeTo_NOTE_BOOK, "wxNotebook");
        menu_item->SetBitmap(GetInternalImage("wxNotebook"));
        menu_item = sub_menu->Append(MenuChangeTo_SIMPLE_BOOK, "wxSimplebook");
        menu_item->SetBitmap(GetInternalImage("wxSimplebook"));
        AppendSubMenu(sub_menu, "&Change widget to");
    }
    else if (node->isGen(gen_wxChoicebook))
    {
        sub_menu = new wxMenu;
        menu_item = sub_menu->Append(MenuChangeTo_AUI_BOOK, "wxAuiNotebook");
        menu_item->SetBitmap(GetInternalImage("auinotebook"));
        menu_item = sub_menu->Append(MenuChangeTo_LIST_BOOK, "wxListbook");
        menu_item->SetBitmap(GetInternalImage("wxListbook"));
        menu_item = sub_menu->Append(MenuChangeTo_NOTE_BOOK, "wxNotebook");
        menu_item->SetBitmap(GetInternalImage("wxNotebook"));
        menu_item = sub_menu->Append(MenuChangeTo_SIMPLE_BOOK, "wxSimplebook");
        menu_item->SetBitmap(GetInternalImage("wxSimplebook"));
        AppendSubMenu(sub_menu, "&Change widget to");
    }
    else if (node->isGen(gen_wxListbook))
    {
        sub_menu = new wxMenu;
        menu_item = sub_menu->Append(MenuChangeTo_AUI_BOOK, "wxAuiNotebook");
        menu_item->SetBitmap(GetInternalImage("auinotebook"));
        menu_item = sub_menu->Append(MenuChangeTo_CHOICE_BOOK, "wxChoicebook");
        menu_item->SetBitmap(GetInternalImage("wxChoicebook"));
        menu_item = sub_menu->Append(MenuChangeTo_NOTE_BOOK, "wxNotebook");
        menu_item->SetBitmap(GetInternalImage("wxNotebook"));
        menu_item = sub_menu->Append(MenuChangeTo_SIMPLE_BOOK, "wxSimplebook");
        menu_item->SetBitmap(GetInternalImage("wxSimplebook"));
        AppendSubMenu(sub_menu, "&Change widget to");
    }
    else if (node->isGen(gen_wxNotebook))
    {
        sub_menu = new wxMenu;
        menu_item = sub_menu->Append(MenuChangeTo_AUI_BOOK, "wxAuiNotebook");
        menu_item->SetBitmap(GetInternalImage("auinotebook"));
        menu_item = sub_menu->Append(MenuChangeTo_CHOICE_BOOK, "wxChoicebook");
        menu_item->SetBitmap(GetInternalImage("wxChoicebook"));
        menu_item = sub_menu->Append(MenuChangeTo_LIST_BOOK, "wxListbook");
        menu_item->SetBitmap(GetInternalImage("wxListbook"));
        menu_item = sub_menu->Append(MenuChangeTo_SIMPLE_BOOK, "wxSimplebook");
        menu_item->SetBitmap(GetInternalImage("wxSimplebook"));
        AppendSubMenu(sub_menu, "&Change widget to");
    }
    else if (node->isGen(gen_wxSimplebook))
    {
        sub_menu = new wxMenu;
        menu_item = sub_menu->Append(MenuChangeTo_AUI_BOOK, "wxAuiNotebook");
        menu_item->SetBitmap(GetInternalImage("auinotebook"));
        menu_item = sub_menu->Append(MenuChangeTo_CHOICE_BOOK, "wxChoicebook");
        menu_item->SetBitmap(GetInternalImage("wxChoicebook"));
        menu_item = sub_menu->Append(MenuChangeTo_LIST_BOOK, "wxListbook");
        menu_item->SetBitmap(GetInternalImage("wxListbook"));
        menu_item = sub_menu->Append(MenuChangeTo_NOTE_BOOK, "wxNotebook");
        menu_item->SetBitmap(GetInternalImage("wxNotebook"));
        AppendSubMenu(sub_menu, "&Change widget to");
    }
}

void NavPopupMenu::MenuAddStandardCommands(Node* node)
{
    m_isPasteAllowed = false;

    if (!node->isGen(gen_wxStatusBar))
    {
        auto clip_node = GetClipboardNode(false);
        if (!clip_node)
        {
            clip_node = wxGetFrame().GetClipboardPtr();
        }

        if (node->isGen(gen_Project))
        {
            auto paste_menu_item = Append(wxID_PASTE);
            paste_menu_item->SetBitmap(wxArtProvider::GetBitmapBundle(wxART_PASTE, wxART_MENU));
            if (!clip_node || !clip_node->IsForm())
            {
                paste_menu_item->Enable(false);
                m_isPasteAllowed = false;
            }

            // There are no other standard commands for a project
            return;
        }
        m_isPasteAllowed = (clip_node ? true : false);
    }

    wxMenuItem* menu_item;
    menu_item = Append(wxID_CUT);
    menu_item->SetBitmap(wxArtProvider::GetBitmapBundle(wxART_CUT, wxART_MENU));
    menu_item = Append(wxID_COPY);
    menu_item->SetBitmap(wxArtProvider::GetBitmapBundle(wxART_COPY, wxART_MENU));
    if (!node->isGen(gen_wxStatusBar))
    {
        auto paste_menu_item = Append(wxID_PASTE);
        paste_menu_item->SetBitmap(wxArtProvider::GetBitmapBundle(wxART_PASTE, wxART_MENU));
        if (!m_isPasteAllowed)
        {
            paste_menu_item->Enable(false);
        }
    }
    menu_item = Append(wxID_DELETE);
    menu_item->SetBitmap(wxArtProvider::GetBitmapBundle(wxART_DELETE, wxART_MENU));
    if (!node->isGen(gen_wxStatusBar))
    {
        Append(MenuDUPLICATE, "Duplicate");
    }
}

void NavPopupMenu::CreateSizerParent(Node* node, ttlib::sview widget)
{
    auto parent = node->GetParent();
    if (!parent)
    {
        // If this actually happens, then we silently do nothing leaving the user no idea of why it didn't work
        FAIL_MSG("If this occurs, we need to figure out why and then add a message to let the user know why.")
        return;
    }

    auto childPos = parent->GetChildPosition(node);

    if (!parent->IsFormParent())
    {
        while (parent && !parent->IsSizer())
        {
            parent = parent->GetParent();
        }
    }

    if (!parent)
    {
        // If this actually happens, then we silently do nothing leaving the user no idea of why it didn't work
        FAIL_MSG("If this occurs, we need to figure out why and then add a message to let the user know why.")
        return;
    }

    if (parent->isGen(gen_folder) || parent->isGen(gen_sub_folder))
        widget = "sub_folder";

    // Avoid the temptation to set new_parent to the raw pointer so that .get() doesn't have to be called below. Doing so
    // will result in the reference count being decremented before we are done hooking it up, and you end up crashing.

    auto new_parent = g_NodeCreator.CreateNode(widget, parent);
    if (new_parent)
    {
        wxGetFrame().Freeze();
        ttlib::cstr undo_string("Insert new ");
        if (widget == "folder" || widget == "sub_folder")
            undo_string << "folder";
        else
            undo_string << "sizer";
        wxGetFrame().PushUndoAction(std::make_shared<InsertNodeAction>(new_parent.get(), parent, undo_string, childPos));

        // InsertNodeAction does not fire the creation event since that's usually handled by the caller as needed. We don't
        // want to fire an event because we don't want the Mockup or Code panels to update until we have changed the parent.
        // However we *do* need to let the navigation panel know that a new node has been added.

        wxGetFrame().GetNavigationPanel()->InsertNode(new_parent.get());

        wxGetFrame().PushUndoAction(std::make_shared<ChangeParentAction>(node, new_parent.get()));
        wxGetFrame().SelectNode(node, evt_flags::fire_event | evt_flags::force_selection);
        wxGetFrame().Thaw();
    }
}

void NavPopupMenu::ChangeSizer(GenEnum::GenName new_sizer_gen)
{
    wxWindowUpdateLocker freeze(wxGetFrame().GetWindow());
    wxGetFrame().PushUndoAction(std::make_shared<ChangeSizerType>(m_node, new_sizer_gen));
}

void NavPopupMenu::ChangeNode(GenEnum::GenName new_node_gen)
{
    wxWindowUpdateLocker freeze(wxGetFrame().GetWindow());
    wxGetFrame().PushUndoAction(std::make_shared<ChangeNodeType>(m_node, new_node_gen));
}

void NavPopupMenu::AddToolbarCommands(Node* node)
{
    auto sub_menu = new wxMenu;
    wxMenuItem* menu_item;
    AppendSubMenu(sub_menu, "Tools");

    bool is_aui_toolbar = (node->gen_name() == gen_wxAuiToolBar || node->GetParent()->gen_name() == gen_wxAuiToolBar);

    menu_item = sub_menu->Append(MenuADD_TOOL, "Tool (normal, check, radio)");
    menu_item->SetBitmap(GetInternalImage("tool"));
    if (!is_aui_toolbar)
    {
        menu_item = sub_menu->Append(MenuADD_TOOL_DROPDOWN, "Dropdown");
        menu_item->SetBitmap(GetInternalImage("tool_dropdown"));
    }

    if (is_aui_toolbar)
    {
        menu_item = sub_menu->Append(MenuADD_TOOL_LABEL, "Label");
        menu_item->SetBitmap(GetInternalImage("wxStaticText"));
    }

    sub_menu->AppendSeparator();
    menu_item = sub_menu->Append(MenuADD_TOOL_COMBOBOX, "Combobox");
    menu_item->SetBitmap(GetInternalImage("wxComboBox"));
    menu_item = sub_menu->Append(MenuADD_TOOL_SLIDER, "Slider");
    menu_item->SetBitmap(GetInternalImage("slider"));
    menu_item = sub_menu->Append(MenuADD_TOOL_SPINCTRL, "Spin control");
    menu_item->SetBitmap(GetInternalImage("spin_ctrl"));
    sub_menu->AppendSeparator();

    menu_item = sub_menu->Append(MenuADD_TOOL_SEPARATOR, "Separator");
    menu_item->SetBitmap(GetInternalImage("toolseparator"));
    if (!is_aui_toolbar)
    {
        menu_item = sub_menu->Append(MenuADD_TOOL_STRETCHABLE_SPACE, "Stretchable space");
        menu_item->SetBitmap(GetInternalImage("toolStretchable"));
    }
    else
    {
        menu_item = sub_menu->Append(MenuADD_TOOL_SPACER, "Spacer");
        menu_item->SetBitmap(GetInternalImage("toolspacer"));
        menu_item = sub_menu->Append(MenuADD_TOOL_STRETCHABLE_SPACER, "Stretchable spacer");
        menu_item->SetBitmap(GetInternalImage("toolStretchable"));
    }

    Bind(
        wxEVT_MENU,
        [](wxCommandEvent&)
        {
            wxGetFrame().CreateToolNode(gen_wxComboBox);
        },
        MenuADD_TOOL_COMBOBOX);
    Bind(
        wxEVT_MENU,
        [](wxCommandEvent&)
        {
            wxGetFrame().CreateToolNode(gen_wxSlider);
        },
        MenuADD_TOOL_SLIDER);
    Bind(
        wxEVT_MENU,
        [](wxCommandEvent&)
        {
            wxGetFrame().CreateToolNode(gen_wxSpinCtrl);
        },
        MenuADD_TOOL_SPINCTRL);
    Bind(
        wxEVT_MENU,
        [](wxCommandEvent&)
        {
            wxGetFrame().CreateToolNode(gen_toolSeparator);
        },
        MenuADD_TOOL_SEPARATOR);

    if (!is_aui_toolbar)
    {
        Bind(
            wxEVT_MENU,
            [](wxCommandEvent&)
            {
                wxGetFrame().CreateToolNode(gen_tool);
            },
            MenuADD_TOOL);
        Bind(
            wxEVT_MENU,
            [](wxCommandEvent&)
            {
                wxGetFrame().CreateToolNode(gen_tool_dropdown);
            },
            MenuADD_TOOL_DROPDOWN);
        Bind(
            wxEVT_MENU,
            [](wxCommandEvent&)
            {
                wxGetFrame().CreateToolNode(gen_toolStretchable);
            },
            MenuADD_TOOL_STRETCHABLE_SPACE);
    }
    else
    {
        Bind(
            wxEVT_MENU,
            [](wxCommandEvent&)
            {
                wxGetFrame().CreateToolNode(gen_auitool);
            },
            MenuADD_TOOL);
        Bind(
            wxEVT_MENU,
            [](wxCommandEvent&)
            {
                wxGetFrame().CreateToolNode(gen_auitool_label);
            },
            MenuADD_TOOL_LABEL);
        Bind(
            wxEVT_MENU,
            [](wxCommandEvent&)
            {
                wxGetFrame().CreateToolNode(gen_auitool_spacer);
            },
            MenuADD_TOOL_SPACER);
        Bind(
            wxEVT_MENU,
            [](wxCommandEvent&)
            {
                wxGetFrame().CreateToolNode(gen_auitool_stretchable);
            },
            MenuADD_TOOL_STRETCHABLE_SPACER);
    }
}
