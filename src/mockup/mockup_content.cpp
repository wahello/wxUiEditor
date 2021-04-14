/////////////////////////////////////////////////////////////////////////////
// Purpose:   Mockup of a form's contents
// Author:    Ralph Walden
// Copyright: Copyright (c) 2020-2021 KeyWorks Software (Ralph Walden)
// License:   Apache License -- see ../../LICENSE
/////////////////////////////////////////////////////////////////////////////

// Note that for most forms, this is the top level wxPanel and we create the contents of the form as if we were the form. The
// notable exception is a MockupWizard -- in this case we create a MockupWizard child which itself is a wxPanel that
// substitutes for the wxWizard form.

#include "pch.h"

#include <wx/bookctrl.h>    // wxBookCtrlBase: common base class for wxList/Tree/Notebook
#include <wx/choicebk.h>    // wxChoicebook: wxChoice and wxNotebook combination
#include <wx/collpane.h>    // wxCollapsiblePane
#include <wx/gbsizer.h>     // wxGridBagSizer:  A sizer that can lay out items in a grid,
#include <wx/ribbon/bar.h>  // Top-level component of the ribbon-bar-style interface
#include <wx/sizer.h>       // provide wxSizer class for layout
#include <wx/statbox.h>     // wxStaticBox base header
#include <wx/statline.h>    // wxStaticLine class interface

#include "mockup_content.h"

#include "base_generator.h"  // BaseGenerator -- Base Generator class
#include "mainframe.h"       // MainFrame -- Main window frame
#include "mockup_parent.h"   // Top-level MockUp Parent window
#include "mockup_wizard.h"   // MockupWizard Mock Up class
#include "node.h"            // Node class
#include "node_creator.h"    // NodeCreator class
#include "node_decl.h"       // NodeDeclaration class
#include "utils.h"           // Utility functions that work with properties

MockupContent::MockupContent(wxWindow* parent, MockupParent* mockupParent) : wxPanel(parent)
{
    m_mockupParent = mockupParent;
}

void MockupContent::RemoveNodes()
{
    if (m_wizard)
    {
        m_wizard->Destroy();
        m_wizard = nullptr;
    }

    m_obj_node_pair.clear();
    m_node_obj_pair.clear();

    DestroyChildren();
    SetSizer(nullptr);

    m_parent_sizer = nullptr;
}

// This is called by MockupParent in order to create all child components
void MockupContent::CreateAllGenerators()
{
    auto form = m_mockupParent->GetSelectedForm();
    if (form->isGen(gen_MenuBar) || form->isGen(gen_ToolBar))
    {
        m_parent_sizer = new wxBoxSizer(wxVERTICAL);
        CreateChildren(form, this, this, m_parent_sizer);
        SetSizerAndFit(m_parent_sizer);
        return;
    }

    if (form->isGen(gen_wxFrame))
        m_parent_sizer = new wxBoxSizer(wxVERTICAL);

    for (size_t i = 0; i < form->GetChildCount(); i++)
    {
        auto child = form->GetChild(i);

        if (m_wizard)
        {
            CreateChildren(child, m_wizard, m_wizard);
        }

        else
        {
            CreateChildren(child, this, this, m_parent_sizer);
        }
    }

    if (m_wizard)
    {
        m_wizard->SetSelection(0);

        // A MockupWizard is added directly as a panel without an intervening sizer, so we need to calculate the size of the
        // window that we need to display it.
        auto sizer = m_wizard->GetSizer();
        if (sizer)
        {
            auto min_size = sizer->GetMinSize();
            SetMinClientSize(min_size);
        }
    }

    if (m_parent_sizer)
        SetSizerAndFit(m_parent_sizer);
    else
        Layout();
}

void MockupContent::CreateChildren(Node* node, wxWindow* parent, wxObject* parentNode, wxBoxSizer* parent_sizer)
{
    auto comp = node->GetGenerator();
    ASSERT_MSG(comp, ttlib::cstr() << "Missing component for " << node->DeclName());
    if (!comp)
        return;

    auto created_object = comp->Create(node, parent);
    if (!created_object)
    {
        if (node->IsSpacer() && parentNode)
        {
            if (node->prop_as_int(prop_proportion) != 0)
            {
                wxStaticCast(parentNode, wxSizer)->AddStretchSpacer(node->prop_as_int(prop_proportion));
            }
            else
            {
                auto width = node->prop_as_int(prop_width);
                auto height = node->prop_as_int(prop_height);
                if (node->prop_as_bool(prop_add_default_border))
                {
                    width += wxSizerFlags::GetDefaultBorder();
                    height += wxSizerFlags::GetDefaultBorder();
                }
                wxStaticCast(parentNode, wxSizer)->Add(width, height);
            }
        }
        return;  // means the component doesn't create any UI element, and cannot have children
    }

    wxWindow* created_window = nullptr;
    wxSizer* created_sizer = nullptr;

    if (node->isGen(gen_wxMenuBar) || node->isGen(gen_MenuBar))
    {
        // Store the wxObject/Node pair both ways
        m_obj_node_pair[created_object] = node;
        m_node_obj_pair[node] = created_object;

        if (parent_sizer)
        {
            parent_sizer->Add((wxWindow*) created_object, wxSizerFlags().Expand().Border(0));
            parent_sizer->Add(new wxStaticLine(this, wxID_ANY), wxSizerFlags().Border(0));
        }

        // We don't create any children because the only thing visible is the mock menu
        return;
    }
    else if (node->IsSizer() || node->isGen(gen_wxStdDialogButtonSizer) || node->isGen(gen_TextSizer))
    {
        if (node->IsStaticBoxSizer())
        {
            auto staticBoxSizer = wxStaticCast(created_object, wxStaticBoxSizer);
            created_window = staticBoxSizer->GetStaticBox();
            created_sizer = staticBoxSizer;
        }
        else
        {
            created_sizer = wxStaticCast(created_object, wxSizer);
        }

        if (auto minsize = node->prop_as_wxSize(prop_minimum_size); minsize != wxDefaultSize)
        {
            created_sizer->SetMinSize(minsize);
            created_sizer->Layout();
        }
    }
    else
    {
        created_window = wxStaticCast(created_object, wxWindow);
        SetWindowProperties(node, created_window);
    }

    // Store the wxObject/Node pair both ways.
    m_obj_node_pair[created_object] = node;
    m_node_obj_pair[node] = created_object;

    wxWindow* new_wxparent = (created_window ? created_window : parent);

    if (node->isGen(gen_wxCollapsiblePane))
    {
        auto collpane = wxStaticCast(created_object, wxCollapsiblePane);
        new_wxparent = collpane->GetPane();
    }

    for (const auto& child: node->GetChildNodePtrs())
    {
        CreateChildren(child.get(), new_wxparent, created_object);
    }

    if (node->GetParent()->isType(type_wizard))
    {
        m_wizard->AddPage(wxStaticCast(created_window, wxPanel));
        return;
    }

    if (parent && (created_window || created_sizer))
    {
        auto obj_parent = GetNode(parentNode);
        if (obj_parent && obj_parent->isGen(gen_wxChoicebook) && node->isType(type_widget))
        {
            wxStaticCast(parentNode, wxChoicebook)
                ->GetControlSizer()
                ->Add(created_window, wxSizerFlags().Expand().Border(wxALL));
        }
        else if (obj_parent && obj_parent->IsSizer())
        {
            auto child_obj = GetNode(created_object);
            auto sizer_flags = child_obj->GetSizerFlags();
            if (obj_parent->isGen(gen_wxGridBagSizer))
            {
                auto sizer = wxStaticCast(parentNode, wxGridBagSizer);
                wxGBPosition position(child_obj->prop_as_int(prop_row), child_obj->prop_as_int(prop_column));
                wxGBSpan span(child_obj->prop_as_int(prop_rowspan), child_obj->prop_as_int(prop_colspan));

                if (created_window)
                    sizer->Add(created_window, position, span, sizer_flags.GetFlags(), sizer_flags.GetBorderInPixels());
                else
                    sizer->Add(created_sizer, position, span, sizer_flags.GetFlags(), sizer_flags.GetBorderInPixels());
            }
            else
            {
                auto sizer = wxStaticCast(parentNode, wxSizer);
                if (created_window && !child_obj->IsStaticBoxSizer())
                {
                    sizer->Add(created_window, sizer_flags.GetProportion(), sizer_flags.GetFlags(),
                               sizer_flags.GetBorderInPixels());
                }
                else
                {
                    sizer->Add(created_sizer, sizer_flags.GetProportion(), sizer_flags.GetFlags(),
                               sizer_flags.GetBorderInPixels());
                }
            }
        }
    }
    comp->AfterCreation(created_object, parent);

    if (parent_sizer)
    {
        if (created_window)
            parent_sizer->Add(created_window, wxSizerFlags().Expand().Border(0));
        else if (created_sizer)
            parent_sizer->Add(created_sizer, wxSizerFlags(1).Expand().Border(0));
    }

    else if ((created_sizer && wxDynamicCast(parentNode, wxWindow)) || (!parentNode && created_sizer))
    {
        parent->SetSizer(created_sizer);
        if (parentNode)
            created_sizer->SetSizeHints(parent);

        parent->SetAutoLayout(true);
        parent->Layout();
    }
}

void MockupContent::SetWindowProperties(Node* node, wxWindow* window)
{
    if (auto size = node->prop_as_wxSize(prop_size); size != wxDefaultSize)
    {
        window->SetSize(size);
    }

    if (auto minsize = node->prop_as_wxSize(prop_minimum_size); minsize != wxDefaultSize)
    {
        window->SetMinSize(minsize);
    }

    if (auto maxsize = node->prop_as_wxSize(prop_maximum_size); maxsize != wxDefaultSize)
    {
        window->SetMaxSize(maxsize);
    }

    if (!node->isPropValue(prop_variant, "normal"))
    {
        if (node->isPropValue(prop_variant, "small"))
            window->SetWindowVariant(wxWINDOW_VARIANT_SMALL);
        else if (node->isPropValue(prop_variant, "mini"))
            window->SetWindowVariant(wxWINDOW_VARIANT_MINI);
        else
            window->SetWindowVariant(wxWINDOW_VARIANT_LARGE);
    }

    if (node->HasValue(prop_font))
    {
        window->SetFont(node->prop_as_font(prop_font));
    }

    if (auto fg_colour = node->get_prop_ptr(prop_foreground_colour); fg_colour && fg_colour->HasValue())
    {
        window->SetForegroundColour(ConvertToColour(fg_colour->GetValue()));
    }

    if (auto bg_colour = node->get_prop_ptr(prop_background_colour); bg_colour && bg_colour->HasValue())
    {
        window->SetBackgroundColour(ConvertToColour(bg_colour->GetValue()));
    }

    if (auto extra_style = node->get_prop_ptr(prop_window_extra_style); extra_style && extra_style->as_int() != 0)
    {
        window->SetExtraStyle(extra_style->as_int());
    }

    if (node->isPropValue(prop_disabled, true))
    {
        window->Disable();
    }

    if (node->isPropValue(prop_hidden, true) && !m_mockupParent->IsShowingHidden())
    {
        window->Show(false);
    }

    if (auto tooltip = node->get_prop_ptr(prop_tooltip); tooltip && tooltip->as_string().size())
    {
        window->SetToolTip(tooltip->as_wxString());
    }
}

void MockupContent::CreateWizard()
{
    auto form = m_mockupParent->GetSelectedForm();
    m_wizard = new MockupWizard(this, form);
}

Node* MockupContent::GetNode(wxObject* wxobject)
{
    if (auto node = m_obj_node_pair.find(wxobject); node != m_obj_node_pair.end())
        return node->second;
    else
        return nullptr;
}

wxObject* MockupContent::Get_wxObject(Node* node)
{
    if (auto wxobject = m_node_obj_pair.find(node); wxobject != m_node_obj_pair.end())
        return wxobject->second;
    else
        return nullptr;
}

void MockupContent::OnNodeSelected(Node* node)
{
    if (m_wizard && node->isGen(gen_wxWizardPageSimple))
    {
        auto parent = node->GetParent();
        ASSERT(parent->isGen(gen_wxWizard));
        m_wizard->SetSelection(parent->GetChildPosition(node));
        return;
    }

    else if (node->isGen(gen_BookPage))
    {
        auto parent = node->GetParent();
        auto book = wxStaticCast(Get_wxObject(parent), wxBookCtrlBase);
        ASSERT(book);
        if (book)
        {
            // If this is a wxChoicebook, then some of the children might be a widget rather then a book page
            size_t sel_pos = 0;
            for (size_t child = 0; child < parent->GetChildCount(); ++child)
            {
                if (parent->GetChildNodePtrs()[child].get() == node)
                    break;
                if (parent->GetChildNodePtrs()[child].get()->GetNodeType()->get_name() == "widget")
                    continue;
                ++sel_pos;
            }
            book->SetSelection(sel_pos);
            m_mockupParent->ClearIgnoreSelection();
        }
        return;
    }

    else if (node->isGen(gen_wxRibbonPage))
    {
        auto parent = node->GetParent();
        ASSERT(parent->isGen(gen_wxRibbonBar));

        auto bar = wxStaticCast(Get_wxObject(parent), wxRibbonBar);
        auto page = wxStaticCast(Get_wxObject(node), wxRibbonPage);
        bar->SetActivePage(page);

        return;
    }
    else if (node->isGen(gen_wxRibbonPanel))
    {
        auto parent = node->GetParent();
        ASSERT(parent->isGen(gen_wxRibbonPage));

        auto bar = wxStaticCast(Get_wxObject(parent->GetParent()), wxRibbonBar);
        auto page = wxStaticCast(Get_wxObject(parent), wxRibbonPage);
        bar->SetActivePage(page);

        return;
    }
    else if (node->isGen(gen_wxRibbonButtonBar) || node->isGen(gen_wxRibbonToolBar))
    {
        auto parent = node->GetParent()->GetParent();
        ASSERT(parent->isGen(gen_wxRibbonPage));

        auto bar = wxStaticCast(Get_wxObject(parent->GetParent()), wxRibbonBar);
        auto page = wxStaticCast(Get_wxObject(parent), wxRibbonPage);
        bar->SetActivePage(page);

        return;
    }
    else if (node->isGen(gen_ribbonButton) || node->isGen(gen_ribbonTool))
    {
        auto parent = node->GetParent()->GetParent()->GetParent();
        ASSERT(parent->isGen(gen_wxRibbonPage));

        auto bar = wxStaticCast(Get_wxObject(parent->GetParent()), wxRibbonBar);
        auto page = wxStaticCast(Get_wxObject(parent), wxRibbonPage);
        bar->SetActivePage(page);

        return;
    }

    // If the project is selected, then there will be no component.
    auto component = node->GetGenerator();
    if (component && component->OnSelected(node))
        Refresh();
}

void MockupContent::SelectNode(wxObject* wxobject)
{
    if (auto result = m_obj_node_pair.find(wxobject); result != m_obj_node_pair.end())
    {
        wxGetFrame().SelectNode(result->second);
    }
}
