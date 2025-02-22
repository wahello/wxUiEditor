/////////////////////////////////////////////////////////////////////////////
// Purpose:   Main window frame
// Author:    Ralph Walden
// Copyright: Copyright (c) 2020-2022 KeyWorks Software (Ralph Walden)
// License:   Apache License -- see ../LICENSE
/////////////////////////////////////////////////////////////////////////////

#include <sstream>

#include <wx/aboutdlg.h>          // declaration of wxAboutDialog class
#include <wx/aui/auibook.h>       // wxaui: wx advanced user interface - notebook
#include <wx/clipbrd.h>           // wxClipboad class and clipboard functions
#include <wx/config.h>            // wxConfig base header
#include <wx/fdrepdlg.h>          // wxFindReplaceDialog class
#include <wx/filedlg.h>           // wxFileDialog base header
#include <wx/filehistory.h>       // wxFileHistory class
#include <wx/frame.h>             // wxFrame class interface
#include <wx/infobar.h>           // declaration of wxInfoBarBase defining common API of wxInfoBar
#include <wx/persist/splitter.h>  // persistence support for wxTLW
#include <wx/persist/toplevel.h>  // persistence support for wxTLW
#include <wx/splitter.h>          // Base header for wxSplitterWindow
#include <wx/stc/stc.h>           // Scintilla
#include <wx/toolbar.h>           // wxToolBar interface declaration
#include <wx/utils.h>             // Miscellaneous utilities
#include <wx/wupdlock.h>          // wxWindowUpdateLocker prevents window redrawing

// auto-generated: wxui/mainframe_base.h and wxui/mainframe_base.cpp

#include "mainframe.h"

#include "base_generator.h"  // BaseGenerator -- Base widget generator class
#include "bitmaps.h"         // Map of bitmaps accessed by name
#include "clipboard.h"       // wxUiEditorData -- Handles reading and writing OS clipboard data
#include "cstm_event.h"      // CustomEvent -- Custom Event class
#include "gen_base.h"        // Generate Base class
#include "gen_enums.h"       // Enumerations for generators
#include "node.h"            // Node class
#include "node_creator.h"    // NodeCreator class
#include "node_gridbag.h"    // GridBag -- Create and modify a node containing a wxGridBagSizer
#include "node_prop.h"       // NodeProperty -- NodeProperty class
#include "project_class.h"   // Project class
#include "undo_cmds.h"       // InsertNodeAction -- Undoable command classes derived from UndoAction
#include "utils.h"           // Utility functions that work with properties
#include "wakatime.h"        // WakaTime -- Updates WakaTime metrics
#include "write_code.h"      // Write code to Scintilla or file

#include "panels/base_panel.h"      // BasePanel -- C++ panel
#include "panels/nav_panel.h"       // NavigationPanel -- Node tree class
#include "panels/propgrid_panel.h"  // PropGridPanel -- Node inspector class
#include "panels/ribbon_tools.h"    // RibbonPanel -- Displays component tools in a wxRibbonBar

#include "wxui/importwinres_base.h"  // ImportWinResDlg -- Dialog for Importing a Windows resource file
#include "wxui/insertwidget_base.h"  // InsertWidget -- Dialog to lookup and insert a widget
#include "wxui/ui_images.h"          // This is generated from the Images form

#if defined(INTERNAL_TESTING)
    #include "internal/code_compare_base.h"  // CodeCompare
    #include "internal/import_panel.h"       // ImportPanel -- Panel to display original imported file
    #include "internal/node_info.h"          // NodeInfo
#endif

#if defined(_DEBUG) || defined(INTERNAL_TESTING)
    #include "internal/debugsettings.h"  // DebugSettings -- Settings while running the Debug version of wxUiEditor
    #include "internal/xrcpreview.h"     // XrcPreview
#endif

#include "mockup/mockup_parent.h"  // MockupParent -- Top-level MockUp Parent window

#include "pugixml.hpp"

using namespace wxue_img;
using namespace GenEnum;

enum
{
    IDM_IMPORT_WINRES = wxID_HIGHEST + 500,

    id_DebugCurrentTest,
    id_DebugXrcImport,
    id_DebugXrcDuplicate,
    id_DebugPreferences,
    id_ConvertImage,
    id_ShowLogger,
    id_NodeMemory,
    id_CodeDiffDlg,
    id_XrcPreviewDlg,
    id_CompareXrcDlg,
    id_MockupPreview,
    id_FindWidget
};

const char* txtEmptyProject = "Empty Project";

MainFrame::MainFrame() :
    MainFrameBase(nullptr), m_findData(wxFR_DOWN)
#if defined(_DEBUG) || defined(INTERNAL_TESTING)
    ,
    m_ImportHistory(9, wxID_FILE1 + 1000)
#endif  // _DEBUG
{
    wxIconBundle bundle;

    bundle.AddIcon(GetIconImage("logo16"));
    bundle.AddIcon(GetIconImage("logo32"));
    SetIcons(bundle);

    SetTitle("wxUiEditor");

    if (WakaTime::IsWakaTimeAvailable())
    {
        m_wakatime = std::make_unique<WakaTime>();
    }

    auto config = wxConfig::Get();

    // Normally, wxPersistentRegisterAndRestore(this, "MainFrame"); could be called to save/restore the size and position.
    // That works fine on Windows 10, but on Windows 11, a user can maximize the height of a window by dragging the frame to
    // the bottom of the screen. This does not generate the normal size event, and therefore the main windows doesn't save or
    // restore the correct size and position. It's worth noting that even Windows apps like Notepad don't handle this
    // correctly either. However, by retrieving the dimensions when the app is closed, the exact size and position can be
    // saved and restored without relying on event messages.

#if defined(_DEBUG)
    config->SetPath("/debug_mainframe");
#else
    config->SetPath("/mainframe");
#endif

    if (auto isMaximixed = config->ReadBool("IsMaximized", false); isMaximixed)
    {
        Maximize();
    }
    else
    {
        if (auto isIconized = config->ReadBool("IsIconized", false); isIconized)
        {
            Iconize();
        }
        else
        {
            wxPoint pt;
            pt.x = config->ReadLong("PosX", -1);
            pt.y = config->ReadLong("PosY", -1);
            SetPosition(pt);

            wxSize config_size;
            config_size.x = config->ReadLong("SizeW", 500);
            config_size.y = config->ReadLong("SizeH", 400);
            SetSize(config_size);
        }
    }

    config->SetPath(txt_main_window_config);
    m_FileHistory.Load(*config);
    m_FileHistory.UseMenu(m_submenu_recent);
    m_FileHistory.AddFilesToMenu();

#if defined(_DEBUG) || defined(INTERNAL_TESTING)
    auto menuInternal = new wxMenu;

    menuInternal->Append(id_CodeDiffDlg, "Compare Code &Generation...",
                         "Dialog showing what class have changed, and optional viewing in WinMerge");
    menuInternal->Append(id_NodeMemory, "Node &Information...", "Show node memory usage");
    menuInternal->Append(id_FindWidget, "&Find Widget...", "Search for a widget starting with the current selected node");

    // We want these available in internal Release builds

    menuInternal->AppendSeparator();
    menuInternal->Append(id_ShowLogger, "Show &Log Window", "Show window containing debug messages");
    menuInternal->Append(id_DebugPreferences, "Test &Settings...", "Settings to use in testing builds");
    menuInternal->AppendSeparator();
    menuInternal->Append(id_DebugCurrentTest, "&Current Test", "Current debugging test");

    ////////////////////// Debug-only menu items //////////////////////
    #if defined(_DEBUG)

    wxMenuItem* item;
    item = menuInternal->Append(id_DebugXrcImport, "&Text XRC import", "Export the current form, then verify importing it");
    item->SetBitmap(bundle_import_svg(16, 16));
    menuInternal->Append(id_DebugXrcDuplicate, "&Text XRC duplication",
                         "Duplicate the current form via Export and Import XRC");

    #endif
    ////////////////////// End Debug-only menu items //////////////////////

    menuInternal->Append(id_ConvertImage, "&Convert Image...", "Image conversion testing...");

    m_submenu_import_recent = new wxMenu();
    m_menuFile->AppendSeparator();
    m_menuFile->AppendSubMenu(m_submenu_import_recent, "Import &Recent");

    config = wxConfig::Get();
    config->SetPath("/debug_history");
    m_ImportHistory.Load(*config);
    m_ImportHistory.UseMenu(m_submenu_import_recent);
    m_ImportHistory.AddFilesToMenu();
    config->SetPath("/");

    Bind(wxEVT_MENU, &MainFrame::OnImportRecent, this, wxID_FILE1 + 1000, wxID_FILE9 + 1000);

    m_menubar->Append(menuInternal, "&Internal");

    #if defined(_DEBUG)
    m_toolbar->AddTool(id_DebugXrcImport, "Test XRC import", bundle_import_svg(24, 24), "Test XRC import");
    #endif

    m_toolbar->Realize();

#endif

    CreateStatusBar(StatusPanels);
    SetStatusBarPane(1);  // specifies where menu and toolbar help content is displayed
    CreateSplitters();

    m_SecondarySplitter->Bind(wxEVT_COMMAND_SPLITTER_SASH_POS_CHANGED,
                              [this](wxSplitterEvent&)
                              {
                                  UpdateStatusWidths();
                              });
    m_MainSplitter->Bind(wxEVT_COMMAND_SPLITTER_SASH_POS_CHANGED,
                         [this](wxSplitterEvent&)
                         {
                             UpdateStatusWidths();
                         });

    Bind(wxEVT_MENU, &MainFrame::OnImportWindowsResource, this, IDM_IMPORT_WINRES);
    Bind(wxEVT_MENU, &MainFrame::OnOpenRecentProject, this, wxID_FILE1, wxID_FILE9);

    Bind(wxEVT_FIND, &MainFrame::OnFind, this);
    Bind(wxEVT_FIND_NEXT, &MainFrame::OnFind, this);
    Bind(wxEVT_FIND_CLOSE, &MainFrame::OnFindClose, this);

    Bind(EVT_NodeSelected, &MainFrame::OnNodeSelected, this);

    Bind(EVT_EventHandlerChanged,
         [this](CustomEvent&)
         {
             UpdateFrame();
         });
    Bind(EVT_GridBagAction,
         [this](CustomEvent&)
         {
             UpdateFrame();
         });
    Bind(EVT_NodeCreated,
         [this](CustomEvent&)
         {
             UpdateFrame();
         });
    Bind(EVT_NodeDeleted,
         [this](CustomEvent&)
         {
             UpdateFrame();
         });
    Bind(EVT_NodePropChange,
         [this](CustomEvent&)
         {
             UpdateFrame();
         });
    Bind(EVT_ParentChanged,
         [this](CustomEvent&)
         {
             UpdateFrame();
         });
    Bind(EVT_PositionChanged,
         [this](CustomEvent&)
         {
             UpdateFrame();
         });
    Bind(EVT_ProjectUpdated,
         [this](CustomEvent&)
         {
             UpdateFrame();
         });

    Bind(
        wxEVT_MENU,
        [this](wxCommandEvent&)
        {
            Close();
        },
        wxID_EXIT);

    Bind(
        wxEVT_MENU,
        [this](wxCommandEvent& event)
        {
            m_mockupPanel->ShowHiddenControls(event.IsChecked());
        },
        id_ShowHidden);

    Bind(
        wxEVT_MENU,
        [this](wxCommandEvent& event)
        {
            m_mockupPanel->MagnifyWindow(event.IsChecked());
        },
        id_Magnify);

#if defined(INTERNAL_TESTING)
    Bind(
        wxEVT_MENU,
        [this](wxCommandEvent&)
        {
            CodeCompare dlg(this);
            dlg.ShowModal();
        },
        id_CodeDiffDlg);

    Bind(
        wxEVT_MENU,
        [this](wxCommandEvent&)
        {
            NodeInfo dlg(this);
            dlg.ShowModal();
        },
        id_NodeMemory);
#endif

#if defined(_DEBUG) || defined(INTERNAL_TESTING)
    Bind(wxEVT_MENU, &MainFrame::OnFindWidget, this, id_FindWidget);
    Bind(wxEVT_MENU, &MainFrame::OnConvertImageDlg, this, id_ConvertImage);

    Bind(
        wxEVT_MENU,
        [](wxCommandEvent&)
        {
            g_pMsgLogging->ShowLogger();
        },
        id_ShowLogger);
    Bind(
        wxEVT_MENU,
        [this](wxCommandEvent&)
        {
            DebugSettings dlg(this);
            dlg.ShowModal();
        },
        id_DebugPreferences);

    Bind(wxEVT_MENU, &App::DbgCurrentTest, &wxGetApp(), id_DebugCurrentTest);
#endif

#if defined(_DEBUG)
    Bind(wxEVT_MENU, &MainFrame::OnTestXrcImport, this, id_DebugXrcImport);
    Bind(wxEVT_MENU, &MainFrame::OnTestXrcDuplicate, this, id_DebugXrcDuplicate);
#endif

    AddCustomEventHandler(GetEventHandler());

    m_notebook->Bind(wxEVT_AUINOTEBOOK_PAGE_CHANGED, &MainFrame::OnAuiNotebookPageChanged, this);
}

MainFrame::~MainFrame()
{
    delete m_findDialog;
}

void MainFrame::OnSaveProject(wxCommandEvent& event)
{
    if (m_isImported || GetProject()->getProjectFile().empty() ||
        GetProject()->getProjectFile().filename().is_sameas(txtEmptyProject))
        OnSaveAsProject(event);
    else
    {
        pugi::xml_document doc;
        GetProject()->CreateDoc(doc);
        if (doc.save_file(GetProject()->getProjectFile().c_str(), "  ", pugi::format_indent_attributes))
        {
            m_isProject_modified = false;
            ProjectSaved();
        }
        else
        {
            wxMessageBox(wxString("Unable to save the project: ") << GetProject()->GetProjectFile(), "Save Project");
        }
    }
}

void MainFrame::OnSaveAsProject(wxCommandEvent&)
{
    auto filename = GetProject()->GetProjectFile().filename();
    if (filename.is_sameas(txtEmptyProject))
    {
        filename = "MyProject";
    }

    // The ".wxue" extension is only used for testing -- all normal projects should have a .wxui extension
    wxFileDialog dialog(this, "Save Project As", GetProject()->GetProjectPath(), filename,
                        "wxUiEditor Project File (*.wxui)|*.wxui;*.wxue", wxFD_SAVE | wxFD_OVERWRITE_PROMPT);

    if (dialog.ShowModal() == wxID_OK)
    {
        filename = dialog.GetPath();
        if (filename.extension().empty())
        {
            filename.replace_extension(".wxui");
        }

        // Don't allow the user to walk over existing project file types that are probably associated with another designer
        // tool

        else if (filename.extension().is_sameas(".fbp", tt::CASE::either))
        {
            wxMessageBox("You cannot save the project as a wxFormBuilder project file", "Save Project As");
            return;
        }
        else if (filename.extension().is_sameas(".wxg", tt::CASE::either))
        {
            wxMessageBox("You cannot save the project as a wxGlade file", "Save Project As");
            return;
        }
        else if (filename.extension().is_sameas(".wxs", tt::CASE::either))
        {
            wxMessageBox("You cannot save the project as a wxSmith file", "Save Project As");
            return;
        }
        else if (filename.extension().is_sameas(".xrc", tt::CASE::either))
        {
            wxMessageBox("You cannot save the project as a XRC file", "Save Project As");
            return;
        }
        else if (filename.extension().is_sameas(".rc", tt::CASE::either) ||
                 filename.extension().is_sameas(".dlg", tt::CASE::either))
        {
            wxMessageBox("You cannot save the project as a Windows Resource file", "Save Project As");
            return;
        }

        pugi::xml_document doc;
        GetProject()->CreateDoc(doc);
        if (doc.save_file(filename.sub_cstr().c_str(), "  ", pugi::format_indent_attributes))
        {
            m_isProject_modified = false;
            m_isImported = false;
            m_FileHistory.AddFileToHistory(filename);
            GetProject()->SetProjectFile(filename);
            GetProject()->SetProjectPath(filename);
            ProjectSaved();
            FireProjectLoadedEvent();
        }
        else
        {
            wxMessageBox(wxString("Unable to save the project: ") << filename, "Save Project As");
        }
    };
}

void MainFrame::OnOpenProject(wxCommandEvent&)
{
    if (!SaveWarning())
        return;

    // The ".wxue" extension is only used for testing -- all normal projects should have a .wxui extension
    wxFileDialog dialog(this, "Open or Import Project", wxEmptyString, wxEmptyString,
                        "wxUiEditor Project File (*.wxui)|*.wxui;*.wxue"
                        "|Windows Resource File (*.rc)|*.rc"
                        "|wxCrafter Project File (*.wxcp)|*.wxcp"
                        "|wxFormBuilder Project File (*.fbp)|*.fbp"
                        "|wxGlade File (*.wxg)|*.wxg"
                        "|wxSmith File (*.wxs)|*.wxs"
                        "|XRC File (*.xrc)|*.xrc||",
                        wxFD_OPEN);

    if (dialog.ShowModal() == wxID_OK)
    {
        ttString filename = dialog.GetPath();
        // The ".wxue" extension is only used for testing -- all normal projects should have a .wxui extension
        if (filename.extension().is_sameas(".wxui", tt::CASE::either) ||
            filename.extension().is_sameas(".wxue", tt::CASE::either))
        {
            wxGetApp().LoadProject(filename);
        }
        else
        {
            wxGetApp().ImportProject(filename);
        }
    };
}

void MainFrame::OnAppendCrafter(wxCommandEvent&)
{
    ttSaveCwd cwd;
    wxFileDialog dlg(this, "Open or Import Project", cwd, wxEmptyString, "wxCrafter Project File (*.wxcp)|*.wxcp||",
                     wxFD_OPEN | wxFD_FILE_MUST_EXIST | wxFD_MULTIPLE);
    if (dlg.ShowModal() == wxID_OK)
    {
        wxArrayString files;
        dlg.GetPaths(files);
        wxGetApp().AppendCrafter(files);
    }
}

void MainFrame::OnAppendFormBuilder(wxCommandEvent&)
{
    ttSaveCwd cwd;
    wxFileDialog dlg(this, "Open or Import Project", cwd, wxEmptyString, "wxFormBuilder Project File (*.fbp)|*.fbp||",
                     wxFD_OPEN | wxFD_FILE_MUST_EXIST | wxFD_MULTIPLE);
    if (dlg.ShowModal() == wxID_OK)
    {
        wxArrayString files;
        dlg.GetPaths(files);
        wxGetApp().AppendFormBuilder(files);
    }
}

void MainFrame::OnAppendGlade(wxCommandEvent&)
{
    ttSaveCwd cwd;
    wxFileDialog dlg(this, "Open or Import Project", cwd, wxEmptyString, "wxGlade Project File (*.wxg)|*.wxg||",
                     wxFD_OPEN | wxFD_FILE_MUST_EXIST | wxFD_MULTIPLE);
    if (dlg.ShowModal() == wxID_OK)
    {
        wxArrayString files;
        dlg.GetPaths(files);
        wxGetApp().AppendGlade(files);
    }
}

void MainFrame::OnAppendSmith(wxCommandEvent&)
{
    ttSaveCwd cwd;
    wxFileDialog dlg(this, "Open or Import Project", cwd, wxEmptyString, "wxSmith File (*.wxs)|*.wxs||",
                     wxFD_OPEN | wxFD_FILE_MUST_EXIST | wxFD_MULTIPLE);
    if (dlg.ShowModal() == wxID_OK)
    {
        wxArrayString files;
        dlg.GetPaths(files);
        wxGetApp().AppendSmith(files);
    }
}

void MainFrame::OnAppendXRC(wxCommandEvent&)
{
    ttSaveCwd cwd;
    wxFileDialog dlg(this, "Open or Import Project", cwd, wxEmptyString, "XRC File (*.xrc)|*.xrc||",
                     wxFD_OPEN | wxFD_FILE_MUST_EXIST | wxFD_MULTIPLE);
    if (dlg.ShowModal() == wxID_OK)
    {
        wxArrayString files;
        dlg.GetPaths(files);
        wxGetApp().AppendXRC(files);
    }
}

void MainFrame::OnImportWindowsResource(wxCommandEvent&)
{
    ImportWinRes dlg(this);
    if (dlg.ShowModal() == wxID_OK)
    {
        wxGetApp().AppendWinRes(dlg.GetRcFilename(), dlg.GetDlgNames());
    }
}

void MainFrame::OnInsertWidget(wxCommandEvent&)
{
    InsertWidget dlg(this);
    if (dlg.ShowModal() == wxID_OK)
    {
        if (auto result = rmap_GenNames.find(dlg.GetWidget()); result != rmap_GenNames.end())
        {
            return CreateToolNode(result->second);
        }
        FAIL_MSG(ttlib::cstr() << "No property enum type exists for dlg.GetWidget()! This should be impossible...")
    }
}

void MainFrame::OnOpenRecentProject(wxCommandEvent& event)
{
    if (!SaveWarning())
        return;

    ttString file = m_FileHistory.GetHistoryFile(event.GetId() - wxID_FILE1);

    if (file.file_exists())
    {
        wxGetApp().LoadProject(file);
    }
    else if (wxMessageBox(
                 wxString().Format(
                     "The project file '%s' doesn't exist.\n\nWould you like to remove it from the recent files list?",
                     file.c_str()),
                 "Open recent project", wxICON_WARNING | wxYES_NO) == wxYES)
    {
        m_FileHistory.RemoveFileFromHistory(event.GetId() - wxID_FILE1);
    }
}

#if defined(_DEBUG) || defined(INTERNAL_TESTING)
void MainFrame::OnImportRecent(wxCommandEvent& event)
{
    ttString file = m_ImportHistory.GetHistoryFile(event.GetId() - (wxID_FILE1 + 1000));
    wxArrayString files;
    files.Add(file);
    auto extension = file.extension();
    if (extension == ".wxcp")
        wxGetApp().AppendCrafter(files);
    else if (extension == ".fbp")
        wxGetApp().AppendFormBuilder(files);
    else if (extension == ".wxg")
        wxGetApp().AppendGlade(files);
    else if (extension == ".wxs")
        wxGetApp().AppendSmith(files);
    else if (extension == ".xrc")
        wxGetApp().AppendXRC(files);
}
#endif  // defined(_DEBUG) || defined(INTERNAL_TESTING)

void MainFrame::OnNewProject(wxCommandEvent&)
{
    if (!SaveWarning())
        return;

    // true means create an empty project
    wxGetApp().NewProject(true);
}

void MainFrame::OnImportProject(wxCommandEvent&)
{
    if (!SaveWarning())
        return;

    wxGetApp().NewProject();
}

namespace wxue_img
{
    extern const unsigned char logo_svg[1943];
}

wxBitmapBundle wxueBundleSVG(const unsigned char* data, size_t size_data, size_t size_svg, wxSize def_size);

void MainFrame::OnAbout(wxCommandEvent&)
{
    wxAboutDialogInfo aboutInfo;
    aboutInfo.SetName(txtVersion);

    // Use trailing spaces to make the dialog width a bit wider
    aboutInfo.SetDescription(ttlib::cstr() << "wxWidgets GUI designer for C++/XRC applications  \n\n\tBuilt using "
                                           << wxVERSION_STRING << '\n');
    aboutInfo.SetCopyright(txtCopyRight);
    aboutInfo.SetWebSite("https://github.com/KeyWorksRW/wxUiEditor");

    aboutInfo.AddDeveloper("Ralph Walden");

    aboutInfo.SetIcon(wxueBundleSVG(wxue_img::logo_svg, 1943, 7265, wxSize(64, 64)).GetIconFor(this));

    wxAboutBox(aboutInfo);
}

void MainFrame::OnClose(wxCloseEvent& event)
{
    if (!SaveWarning())
        return;

    wxGetApp().SetMainFrameClosing();

    auto config = wxConfig::Get();
#if defined(_DEBUG)
    config->SetPath("/debug_mainframe");
#else
    config->SetPath("/mainframe");
#endif

    bool isIconized = IsIconized();
    bool isMaximized = IsMaximized();

    if (!isMaximized)
    {
        config->Write("PosX", isIconized ? -1 : GetPosition().x);
        config->Write("PosY", isIconized ? -1 : GetPosition().y);
        config->Write("SizeW", isIconized ? -1 : GetSize().GetWidth());
        config->Write("SizeH", isIconized ? -1 : GetSize().GetHeight());
    }

    config->Write("IsMaximized", isMaximized);
    config->Write("IsIconized", isIconized);

    config->SetPath(txt_main_window_config);
    m_FileHistory.Save(*config);
    m_property_panel->SaveDescBoxHeight();

    // BUGBUG: [KeyWorks - 01-24-2022] m_has_clipboard_data is never set to true

    // If we have clipboard data, ensure it persists after we exit
    if (m_has_clipboard_data)
        wxTheClipboard->Flush();

#if defined(_DEBUG) || defined(INTERNAL_TESTING)
    config->SetPath("/debug_history");
    m_ImportHistory.Save(*config);
    config->SetPath("/");

    g_pMsgLogging->CloseLogger();
#endif

    event.Skip();
}

void MainFrame::ProjectLoaded()
{
    GetProject()->GetProjectPath().ChangeDir();
    setStatusText("Project loaded");
    if (!m_isImported)
    {
        if (!GetProject()->GetProjectFile().filename().is_sameas(txtEmptyProject))
        {
            m_FileHistory.AddFileToHistory(GetProject()->GetProjectFile());
        }
        m_isProject_modified = false;
    }

    m_undo_stack.clear();
    m_undo_stack_size = 0;

    if (!m_iswakatime_bound)
    {
        m_iswakatime_bound = true;
        Bind(wxEVT_ACTIVATE,
             [this](wxActivateEvent&)
             {
                 if (m_wakatime)
                     m_wakatime->ResetHeartbeat();
             });
    }

    m_selected_node = wxGetApp().GetProjectPtr();
}

void MainFrame::ProjectSaved()
{
    setStatusText(ttlib::cstr(GetProject()->getProjectFile().filename()) << " saved");
    UpdateFrame();
}

void MainFrame::OnNodeSelected(CustomEvent& event)
{
    // This event is normally only fired if the current selection has changed. We dismiss any previous infobar message, and
    // check to see if the current selection has any kind of issue that we should warn the user about.
    m_info_bar->Dismiss();

    auto evt_flags = event.GetNode();

    if (evt_flags->isGen(gen_wxToolBar))
    {
        if (evt_flags->GetParent()->IsSizer())
        {
            auto grandparent = evt_flags->GetParent()->GetParent();
            if (grandparent->isGen(gen_wxFrame) || grandparent->isGen(gen_wxAuiMDIChildFrame))
            {
                // Caution! In wxWidgets 3.1.3 The info bar will wrap the first word if it starts with "If".
                GetPropInfoBar()->ShowMessage(
                    "For the toolbar to be owned by the frame window, it should be placed directly under the frame, "
                    "not under a sizer. Use Cut and Paste to move it.",
                    wxICON_INFORMATION);
            }
        }
    }

#if defined(_DEBUG) || defined(INTERNAL_TESTING)
    g_pMsgLogging->OnNodeSelected();
    m_imnportPanel->OnNodeSelected(evt_flags);
#endif

    UpdateFrame();
}

void MainFrame::UpdateLayoutTools()
{
    int option = -1;
    int border = 0;
    int flag = 0;
    int orient = 0;

    bool gotLayoutSettings = GetLayoutSettings(&flag, &option, &border, &orient);

    bool enableHorizontalTools = (orient != wxHORIZONTAL) && gotLayoutSettings;
    m_menuEdit->Enable(id_AlignLeft, enableHorizontalTools);
    m_toolbar->EnableTool(id_AlignLeft, enableHorizontalTools);
    m_menuEdit->Check(id_AlignLeft, ((flag & (wxALIGN_RIGHT | wxALIGN_CENTER_HORIZONTAL)) == 0) && enableHorizontalTools);
    m_toolbar->ToggleTool(id_AlignLeft,
                          ((flag & (wxALIGN_RIGHT | wxALIGN_CENTER_HORIZONTAL)) == 0) && enableHorizontalTools);

    m_menuEdit->Enable(id_AlignCenterHorizontal, enableHorizontalTools);
    m_toolbar->EnableTool(id_AlignCenterHorizontal, enableHorizontalTools);
    m_menuEdit->Check(id_AlignCenterHorizontal, (flag & wxALIGN_CENTER_HORIZONTAL) && enableHorizontalTools);
    m_toolbar->ToggleTool(id_AlignCenterHorizontal, (flag & wxALIGN_CENTER_HORIZONTAL) && enableHorizontalTools);

    m_menuEdit->Enable(id_AlignRight, enableHorizontalTools);
    m_toolbar->EnableTool(id_AlignRight, enableHorizontalTools);
    m_menuEdit->Check(id_AlignRight, (flag & wxALIGN_RIGHT) && enableHorizontalTools);
    m_toolbar->ToggleTool(id_AlignRight, (flag & wxALIGN_RIGHT) && enableHorizontalTools);

    bool enableVerticalTools = (orient != wxVERTICAL) && gotLayoutSettings;
    m_menuEdit->Enable(id_AlignTop, enableVerticalTools);
    m_toolbar->EnableTool(id_AlignTop, enableVerticalTools);
    m_menuEdit->Check(id_AlignTop, ((flag & (wxALIGN_BOTTOM | wxALIGN_CENTER_VERTICAL)) == 0) && enableVerticalTools);
    m_toolbar->ToggleTool(id_AlignTop, ((flag & (wxALIGN_BOTTOM | wxALIGN_CENTER_VERTICAL)) == 0) && enableVerticalTools);

    m_menuEdit->Enable(id_AlignCenterVertical, enableVerticalTools);
    m_toolbar->EnableTool(id_AlignCenterVertical, enableVerticalTools);
    m_menuEdit->Check(id_AlignCenterVertical, (flag & wxALIGN_CENTER_VERTICAL) && enableVerticalTools);
    m_toolbar->ToggleTool(id_AlignCenterVertical, (flag & wxALIGN_CENTER_VERTICAL) && enableVerticalTools);

    m_menuEdit->Enable(id_AlignBottom, enableVerticalTools);
    m_toolbar->EnableTool(id_AlignBottom, enableVerticalTools);
    m_menuEdit->Check(id_AlignBottom, (flag & wxALIGN_BOTTOM) && enableVerticalTools);
    m_toolbar->ToggleTool(id_AlignBottom, (flag & wxALIGN_BOTTOM) && enableVerticalTools);

    m_menuEdit->Enable(id_BorderLeft, gotLayoutSettings);
    m_menuEdit->Enable(id_BorderRight, gotLayoutSettings);
    m_menuEdit->Enable(id_BorderTop, gotLayoutSettings);
    m_menuEdit->Enable(id_BorderBottom, gotLayoutSettings);

    m_toolbar->EnableTool(id_BorderLeft, gotLayoutSettings);
    m_toolbar->EnableTool(id_BorderRight, gotLayoutSettings);
    m_toolbar->EnableTool(id_BorderTop, gotLayoutSettings);
    m_toolbar->EnableTool(id_BorderBottom, gotLayoutSettings);

    m_menuEdit->Check(id_BorderTop, ((flag & wxTOP) != 0) && gotLayoutSettings);
    m_menuEdit->Check(id_BorderRight, ((flag & wxRIGHT) != 0) && gotLayoutSettings);
    m_menuEdit->Check(id_BorderLeft, ((flag & wxLEFT) != 0) && gotLayoutSettings);
    m_menuEdit->Check(id_BorderBottom, ((flag & wxBOTTOM) != 0) && gotLayoutSettings);

    m_toolbar->ToggleTool(id_BorderTop, ((flag & wxTOP) != 0) && gotLayoutSettings);
    m_toolbar->ToggleTool(id_BorderRight, ((flag & wxRIGHT) != 0) && gotLayoutSettings);
    m_toolbar->ToggleTool(id_BorderLeft, ((flag & wxLEFT) != 0) && gotLayoutSettings);
    m_toolbar->ToggleTool(id_BorderBottom, ((flag & wxBOTTOM) != 0) && gotLayoutSettings);

    m_menuEdit->Enable(id_Expand, gotLayoutSettings);
    m_toolbar->EnableTool(id_Expand, gotLayoutSettings);
    m_menuEdit->Check(id_Expand, ((flag & wxEXPAND) != 0) && gotLayoutSettings);
    m_toolbar->ToggleTool(id_Expand, ((flag & wxEXPAND) != 0) && gotLayoutSettings);
}

void MainFrame::UpdateFrame()
{
    ttString filename = GetProject()->GetProjectFile().filename();

    if (filename.empty())
    {
        filename = "untitled";
    }
    filename.remove_extension();

    if (m_isProject_modified)
    {
        filename.insert(0, "*");
    }
    SetTitle(filename);

    wxString menu_text = "Undo";
    if (m_undo_stack.IsUndoAvailable())
    {
        if (m_undo_stack.GetUndoString().size())
        {
            menu_text << ' ' << m_undo_stack.GetUndoString();
        }
    }
    menu_text << "\tCtrl+Z";
    m_menuEdit->FindChildItem(wxID_UNDO)->SetItemLabel(menu_text);

    menu_text = "Redo";
    if (m_undo_stack.IsRedoAvailable())
    {
        if (m_undo_stack.GetRedoString().size())
        {
            menu_text << ' ' << m_undo_stack.GetRedoString();
        }
    }
    menu_text << "\tCtrl+Y";
    m_menuEdit->FindChildItem(wxID_REDO)->SetItemLabel(menu_text);

    bool isMockup = (m_notebook->GetPageText(m_notebook->GetSelection()) == "Mock Up");
    m_menuEdit->Enable(wxID_FIND, !isMockup);
#if defined(_DEBUG)
    m_menuEdit->Enable(id_insert_widget, true);
#else
    m_menuEdit->Enable(id_insert_widget, m_selected_node && !m_selected_node->IsFormParent());
#endif  // _DEBUG

    UpdateMoveMenu();
    UpdateLayoutTools();
    UpdateWakaTime();
}

void MainFrame::OnCopy(wxCommandEvent&)
{
    if (auto win = wxWindow::FindFocus(); win && win->IsKindOf(wxCLASSINFO(wxStyledTextCtrl)))
    {
        wxStaticCast(win, wxStyledTextCtrl)->Copy();
        return;
    }
    else if (m_selected_node)
    {
        CopyNode(m_selected_node.get());
        UpdateFrame();
    }
}

void MainFrame::OnCut(wxCommandEvent&)
{
    if (auto win = wxWindow::FindFocus(); win && win->IsKindOf(wxCLASSINFO(wxStyledTextCtrl)))
    {
        // This is a read-only control, so we don't allow Cut
        return;
    }
    else
    {
        RemoveNode(m_selected_node.get(), true);
        UpdateFrame();
    }
}

void MainFrame::OnDelete(wxCommandEvent&)
{
    ASSERT(m_selected_node);
    RemoveNode(m_selected_node.get(), false);
    UpdateFrame();
}

void MainFrame::OnPaste(wxCommandEvent&)
{
    if (auto win = wxWindow::FindFocus(); win && win->IsKindOf(wxCLASSINFO(wxStyledTextCtrl)))
    {
        // This is a read-only control, so we don't allow Paste
        return;  // we don't allow pasting into the code generation windows which are marked as read-only
    }
    else if (m_selected_node)
    {
        PasteNode(m_selected_node.get());
        UpdateFrame();
    }
}

void MainFrame::OnDuplicate(wxCommandEvent& WXUNUSED(event))
{
    ASSERT(m_selected_node);
    DuplicateNode(m_selected_node.get());
}

void MainFrame::OnBrowseDocs(wxCommandEvent& WXUNUSED(event))
{
    if (m_selected_node)
    {
        if (auto generator = m_selected_node->GetGenerator(); generator)
        {
            auto file = generator->GetHelpURL(m_selected_node.get());
            if (file.size())
            {
                // wxString url("https://docs.wxwidgets.org/trunk/class");
                wxString url("https://docs.wxwidgets.org/3.2.0/class");
                url << file.wx_str();
                wxLaunchDefaultBrowser(url);
                return;
            }
        }
    }
    // wxLaunchDefaultBrowser("https://docs.wxwidgets.org/trunk/");
    wxLaunchDefaultBrowser("https://docs.wxwidgets.org/3.2.0/");
}

void MainFrame::OnUpdateBrowseDocs(wxUpdateUIEvent& event)
{
    if (m_selected_node)
    {
        if (auto generator = m_selected_node->GetGenerator(); generator)
        {
            auto label = generator->GetHelpText(m_selected_node.get());
            if (label.empty())
            {
                label << "wxWidgets";
            }
            label << " Documentation";
            event.SetText(label.wx_str());
            return;
        }
    }

    event.SetText("wxWidgets Documentation");
}

void MainFrame::OnChangeAlignment(wxCommandEvent& event)
{
    int align = 0;
    bool vertical =
        (event.GetId() == id_AlignTop || event.GetId() == id_AlignBottom || event.GetId() == id_AlignCenterVertical);

    switch (event.GetId())
    {
        case id_AlignRight:
            align = wxALIGN_RIGHT;

            break;

        case id_AlignCenterHorizontal:
            align = wxALIGN_CENTER_HORIZONTAL;

            break;

        case id_AlignBottom:
            align = wxALIGN_BOTTOM;

            break;

        case id_AlignCenterVertical:
            align = wxALIGN_CENTER_VERTICAL;

            break;
    }

    ChangeAlignment(m_selected_node.get(), align, vertical);

    UpdateLayoutTools();
}

void MainFrame::OnChangeBorder(wxCommandEvent& event)
{
    int border;

    switch (event.GetId())
    {
        case id_BorderLeft:
            border = wxLEFT;
            break;

        case id_BorderRight:
            border = wxRIGHT;
            break;

        case id_BorderTop:
            border = wxTOP;
            break;

        case id_BorderBottom:
            border = wxBOTTOM;
            break;

        default:
            border = 0;
            break;
    }

    ToggleBorderFlag(m_selected_node.get(), border);

    UpdateLayoutTools();
}

bool MainFrame::SaveWarning()
{
    int result = wxYES;

    if (m_isProject_modified)
    {
        result = ::wxMessageBox("Current project file has been modified...\n"
                                "Do you want to save the changes?",
                                "Save project", wxYES | wxNO | wxCANCEL, this);

        if (result == wxYES)
        {
            wxCommandEvent dummy;
            OnSaveProject(dummy);
        }
    }

    return (result != wxCANCEL);
}

void MainFrame::OnAuiNotebookPageChanged(wxAuiNotebookEvent&)
{
    UpdateFrame();
    if (auto page = m_notebook->GetCurrentPage(); page)
    {
        if (page == m_mockupPanel)
        {
            m_mockupPanel->CreateContent();
        }
#if defined(INTERNAL_TESTING)
        else if (page != m_imnportPanel)
        {
            static_cast<BasePanel*>(page)->GenerateBaseClass();
        }
#else
        else
        {
            static_cast<BasePanel*>(page)->GenerateBaseClass();
        }
#endif
    }
}

void MainFrame::OnFindDialog(wxCommandEvent&)
{
    if (!m_findDialog)
    {
        m_findDialog = new wxFindReplaceDialog(this, &m_findData, "Find");
        m_findDialog->Centre(wxCENTRE_ON_SCREEN | wxBOTH);
    }
    m_findDialog->Show(true);
}

void MainFrame::OnFindClose(wxFindDialogEvent&)
{
    m_findDialog->Destroy();
    m_findDialog = nullptr;
}

void MainFrame::OnFind(wxFindDialogEvent& event)
{
    if (auto page = m_notebook->GetCurrentPage(); page)
    {
        event.StopPropagation();
        event.SetClientData(m_findDialog);
        page->GetEventHandler()->ProcessEvent(event);
    }
}

wxWindow* MainFrame::CreateNoteBook(wxWindow* parent)
{
    m_notebook = new wxAuiNotebook(parent, wxID_ANY, wxDefaultPosition, wxDefaultSize, wxAUI_NB_TOP);
    m_notebook->SetArtProvider(new wxAuiSimpleTabArt());

    m_mockupPanel = new MockupParent(m_notebook, this);
    m_notebook->AddPage(m_mockupPanel, "Mock Up", false, wxWithImages::NO_IMAGE);

    m_generatedPanel = new BasePanel(m_notebook, this, GEN_LANG_CPLUSPLUS);
    m_notebook->AddPage(m_generatedPanel, "C++", false, wxWithImages::NO_IMAGE);

    // Placing the Python panel first as it's the most commonly used language after C++
    m_pythonPanel = new BasePanel(m_notebook, this, GEN_LANG_PYTHON);
    m_notebook->AddPage(m_pythonPanel, "Python", false, wxWithImages::NO_IMAGE);

    m_xrcPanel = new BasePanel(m_notebook, this, GEN_LANG_XRC);
    m_notebook->AddPage(m_xrcPanel, "XRC", false, wxWithImages::NO_IMAGE);

#if defined(INTERNAL_TESTING)
    m_imnportPanel = new ImportPanel(m_notebook);
    m_notebook->AddPage(m_imnportPanel, "Import", false, wxWithImages::NO_IMAGE);
#endif

    return m_notebook;
}

void MainFrame::CreateSplitters()
{
    // The main splitter contains the navigation tree control and it's toolbar on the left. On the right is a panel
    // containing the Ribbon toolbar at the top, and a splitter window containing the property grid and notebook with
    // mockup and code windows below it.

    m_MainSplitter = new wxSplitterWindow(this, wxID_ANY, wxDefaultPosition, wxDefaultSize, wxSP_LIVE_UPDATE);

    m_nav_panel = new NavigationPanel(m_MainSplitter, this);
    auto panel_right = new wxPanel(m_MainSplitter);
    panel_right->SetWindowStyle(wxBORDER_RAISED);

    auto parent_sizer = new wxBoxSizer(wxVERTICAL);
    m_ribbon_panel = new RibbonPanel(panel_right);
    parent_sizer->Add(m_ribbon_panel, wxSizerFlags(0).Expand());

    m_SecondarySplitter = new wxSplitterWindow(panel_right, wxID_ANY, wxDefaultPosition, wxDefaultSize, wxSP_LIVE_UPDATE);
    parent_sizer->Add(m_SecondarySplitter, wxSizerFlags(1).Expand());

    m_info_bar = new wxInfoBar(panel_right);
    parent_sizer->Add(m_info_bar, wxSizerFlags().Expand());

    panel_right->SetSizer(parent_sizer);

    m_property_panel = new PropGridPanel(m_SecondarySplitter, this);
    auto notebook = CreateNoteBook(m_SecondarySplitter);

    m_SecondarySplitter->SplitVertically(m_property_panel, notebook, m_SecondarySashPosition);

    m_MainSplitter->SplitVertically(m_nav_panel, panel_right);
    m_MainSplitter->SetName("Navigation");
    wxPersistenceManager::Get().RegisterAndRestore(m_MainSplitter);

    // Need to update the left splitter so the right one is drawn correctly
    wxSizeEvent update(GetSize(), GetId());
    ProcessEvent(update);
    m_MainSplitter->UpdateSize();
    m_MainSplitter->SetMinimumPaneSize(2);

    // Set to zero because we don't need this to change relative size when the main window is resized. Fixes issue
    // #90
    m_SecondarySplitter->SetSashGravity(0);
    m_SecondarySplitter->SetMinimumPaneSize(2);

    m_SecondarySplitter->SetName("Properties");
    wxPersistenceManager::Get().RegisterAndRestore(m_SecondarySplitter);

    // UpdateStatusWidths();
    m_MainSashPosition = m_MainSplitter->GetSashPosition();
    m_SecondarySashPosition = m_SecondarySplitter->GetSashPosition();
    int widths[StatusPanels] = { 1, (m_MainSashPosition + m_SecondarySashPosition) - 12, -1 };
    SetStatusWidths(sizeof(widths) / sizeof(int), widths);

    // SetMinSize(wxSize(700, 380));
}

void MainFrame::SetStatusField(const ttlib::cstr text, int position)
{
    if (position == -1)
        position = m_posPropGridStatusField;
    setStatusText(text, position);
}

void MainFrame::UpdateStatusWidths()
{
    if (m_MainSplitter)
    {
        m_MainSashPosition = m_MainSplitter->GetSashPosition();
    }
    if (m_SecondarySplitter)
    {
        m_SecondarySashPosition = m_SecondarySplitter->GetSashPosition();
    }

    int widths[StatusPanels] = { 1, (m_MainSashPosition + m_SecondarySashPosition - 16), -1 };
    SetStatusWidths(sizeof(widths) / sizeof(int), widths);
}

void MainFrame::UpdateMoveMenu()
{
    auto node = m_selected_node.get();
    Node* parent = nullptr;
    if (node)
        parent = node->GetParent();
    if (!node || !parent)
    {
        m_menuEdit->Enable(id_MoveUp, false);
        m_menuEdit->Enable(id_MoveDown, false);
        m_menuEdit->Enable(id_MoveLeft, false);
        m_menuEdit->Enable(id_MoveRight, false);
        return;
    }

    m_menuEdit->Enable(id_MoveUp, MoveNode(node, MoveDirection::Up, true));
    m_menuEdit->Enable(id_MoveDown, MoveNode(node, MoveDirection::Down, true));
    m_menuEdit->Enable(id_MoveLeft, MoveNode(node, MoveDirection::Left, true));
    m_menuEdit->Enable(id_MoveRight, MoveNode(node, MoveDirection::Right, true));
}

Node* MainFrame::GetSelectedForm()
{
    if (!m_selected_node || m_selected_node->isGen(gen_Project))
        return nullptr;

    return m_selected_node->get_form();
}

bool MainFrame::SelectNode(Node* node, size_t flags)
{
    if (!node)
    {
        m_selected_node = NodeSharedPtr();
        return false;
    }

    if (node == m_selected_node.get() && !(flags & evt_flags::force_selection))
    {
        return false;  // already selected
    }

    m_selected_node = node->GetSharedPtr();

    if (flags & evt_flags::queue_event)
    {
        CustomEvent node_event(EVT_NodeSelected, m_selected_node.get());
        for (auto handler: m_custom_event_handlers)
        {
            handler->QueueEvent(node_event.Clone());
        }
    }
    else if (flags & evt_flags::fire_event)
    {
        FireSelectedEvent(node);
    }

    return true;
}

void MainFrame::CreateToolNode(GenName name)
{
    if (!m_selected_node)
    {
        wxMessageBox("You need to select something first in order to properly place this widget.");
        return;
    }

    if (name == gen_tool && (m_selected_node->isType(type_aui_toolbar) || m_selected_node->isType(type_aui_tool)))
    {
        name = gen_auitool;
    }

    if (!m_selected_node->CreateToolNode(name))
    {
        if (m_selected_node->isGen(gen_wxSplitterWindow))
        {
            return;  // The user has already been notified of the problem
        }

        wxMessageBox(ttlib::cstr() << "Unable to create " << map_GenNames[name] << " as a child of "
                                   << m_selected_node->DeclName());
    }
}

void MainFrame::CopyNode(Node* node)
{
    ASSERT(node);
    m_clipboard = g_NodeCreator.MakeCopy(node);
    if (m_clipboard)
    {
        SmartClipboard clip;
        if (clip.IsOpened())
        {
            pugi::xml_document doc;
            auto clip_node = doc.append_child("node");
            int project_version = minRequiredVer;
            m_clipboard->AddNodeToDoc(clip_node, project_version);
            // REVIEW: [Randalphwa - 08-24-2022] project_version is ignored, assuming that the same version of
            // wxClipboard will be used to paste the clipboard node.
            auto* u8_data = new wxUtf8DataObject();
            std::stringstream strm;
            doc.save(strm, "", pugi::format_raw);

            // Skip over the XML header
            auto begin = strm.str().find("<node");
            if (ttlib::is_found(begin))
            {
                u8_data->GetText() = strm.str().c_str() + begin;
                auto hash_data = new wxUEDataObject();

                m_clip_hash = 0;
                m_clipboard->CalcNodeHash(m_clip_hash);
                hash_data->GetHash() = m_clip_hash;

                wxDataObjectComposite* data = new wxDataObjectComposite();
                data->Add(u8_data, true);
                data->Add(hash_data, false);
                wxTheClipboard->SetData(data);
            }
            else
            {
                // If it wasn't passed to wxDataObjectComposite, then we need to delete it
                // ourselves.
                delete u8_data;
            }
        }
    }
}

void MainFrame::PasteNode(Node* parent)
{
    if (auto result = isClipboardDataAvailable(); result)
    {
        auto new_node = GetClipboardNode();
        if (new_node)
        {
            m_clipboard = new_node;
        }
    }

    if (!m_clipboard)
    {
        wxMessageBox("There is nothing in the clipboard that can be pasted!", "Paste Clipboard");
        return;
    }

    if (!parent)
    {
        ASSERT(m_selected_node)
        if (!m_selected_node)
            return;
        parent = m_selected_node.get();
    }

    if (parent->isGen(gen_wxSplitterWindow) && parent->GetChildCount() > 1)
    {
        wxMessageBox("A wxSplitterWindow can't have more than two windows.");
        return;
    }

    auto new_node = g_NodeCreator.MakeCopy(m_clipboard.get(), parent);
    if (new_node->IsForm())
        GetProject()->FixupDuplicatedNode(new_node.get());

    if (!parent->IsChildAllowed(new_node))
    {
        auto grandparent = parent->GetParent();
        if (!grandparent || !grandparent->IsChildAllowed(new_node))
        {
            wxMessageBox(ttlib::cstr() << "You cannot paste " << new_node->DeclName() << " into " << parent->DeclName());
            return;
        }
        parent = grandparent;
    }

    if (parent->isGen(gen_wxGridBagSizer))
    {
        GridBag grid_bag(parent);
        grid_bag.InsertNode(parent, new_node.get());
        return;
    }

    ttlib::cstr undo_str("paste ");
    undo_str << m_clipboard->DeclName();

    auto pos = parent->FindInsertionPos(m_selected_node);
    PushUndoAction(std::make_shared<InsertNodeAction>(new_node.get(), parent, undo_str, pos));
    FireCreatedEvent(new_node);
    SelectNode(new_node, evt_flags::fire_event | evt_flags::force_selection);
}

void MainFrame::DuplicateNode(Node* node)
{
    ASSERT(node);
    ASSERT(node->GetParent());

    auto new_node = g_NodeCreator.MakeCopy(node);
    if (new_node->IsForm())
        GetProject()->FixupDuplicatedNode(new_node.get());
    auto* parent = node->GetParent();
    if (parent->isGen(gen_wxGridBagSizer))
    {
        GridBag grid_bag(parent);
        grid_bag.InsertNode(parent, new_node.get());
        // GridBag::InsertNode() will have already fired events
    }
    else
    {
        ttlib::cstr undo_str("duplicate ");
        undo_str << node->DeclName();
        auto pos = parent->FindInsertionPos(m_selected_node);
        PushUndoAction(std::make_shared<InsertNodeAction>(new_node.get(), parent, undo_str, pos));
        m_selected_node = new_node;
        FireCreatedEvent(new_node);
        SelectNode(new_node, evt_flags::fire_event | evt_flags::force_selection);
    }
}

bool MainFrame::CanCopyNode()
{
    return (m_selected_node.get() && !m_selected_node->isGen(gen_Project));
}

bool MainFrame::CanPasteNode()
{
    return (m_selected_node.get() && (m_clipboard.get() || isClipboardDataAvailable()));
}

void MainFrame::Undo()
{
    wxWindowUpdateLocker freeze(this);

    m_undo_stack.Undo();
    m_isProject_modified = (m_undo_stack_size != m_undo_stack.size());
    if (!m_undo_stack.wasUndoEventGenerated())
        FireProjectUpdatedEvent();
    if (!m_undo_stack.wasUndoSelectEventGenerated())
        FireSelectedEvent(m_selected_node.get());
}

void MainFrame::Redo()
{
    wxWindowUpdateLocker freeze(this);

    m_undo_stack.Redo();
    m_isProject_modified = (m_undo_stack_size != m_undo_stack.size());
    if (!m_undo_stack.wasRedoEventGenerated())
        FireProjectUpdatedEvent();
    if (!m_undo_stack.wasRedoSelectEventGenerated())
        FireSelectedEvent(GetSelectedNode());
}

void MainFrame::OnToggleExpandLayout(wxCommandEvent&)
{
    if (!m_selected_node || !m_selected_node->GetParent() || !m_selected_node->GetParent()->IsSizer())
    {
        return;
    }

    auto propFlag = m_selected_node->get_prop_ptr(prop_flags);

    if (!propFlag)
    {
        return;
    }

    auto currentValue = propFlag->as_string();
    auto wasExpanded = isPropFlagSet("wxEXPAND", currentValue);
    auto value = (wasExpanded ? ClearPropFlag("wxEXPAND", currentValue) : SetPropFlag("wxEXPAND", currentValue));

    if (!wasExpanded)
    {
        auto alignment = m_selected_node->get_prop_ptr(prop_alignment);
        if (alignment && alignment->as_string().size())
        {
            // All alignment flags are invalid if wxEXPAND is set
            ModifyProperty(alignment, "");
        }
    }

    ModifyProperty(propFlag, value);
}

void MainFrame::ToggleBorderFlag(Node* node, int border)
{
    if (!node)
        return;

    auto propFlag = node->get_prop_ptr(prop_borders);

    if (!propFlag)
        return;

    auto value = ClearMultiplePropFlags("wxALL|wxTOP|wxBOTTOM|wxRIGHT|wxLEFT", propFlag->as_string());

    auto intVal = propFlag->as_int();
    intVal ^= border;

    if ((intVal & wxALL) == wxALL)
        value = value << "|wxALL";
    else
    {
        if ((intVal & wxTOP))
            value << "|wxTOP";

        if ((intVal & wxBOTTOM))
            value << "|wxBOTTOM";

        if ((intVal & wxRIGHT))
            value << "|wxRIGHT";

        if ((intVal & wxLEFT))
            value << "|wxLEFT";
    }

    if (value[0] == '|')
        value.erase(0, 1);

    ModifyProperty(propFlag, value);
}

void MainFrame::ModifyProperty(NodeProperty* prop, ttlib::sview value)
{
    if (prop && value != prop->as_string())
    {
        PushUndoAction(std::make_shared<ModifyPropertyAction>(prop, value));
    }
}

void MainFrame::ChangeAlignment(Node* node, int align, bool vertical)
{
    if (!node)
        return;

    auto propFlag = node->get_prop_ptr(prop_alignment);

    if (!propFlag)
        return;

    ttlib::cstr value;

    // First we delete the flags from the previous configuration, in order to avoid alignment conflicts.

    if (vertical)
    {
        value = ClearMultiplePropFlags("wxALIGN_TOP|wxALIGN_BOTTOM|wxALIGN_CENTER_VERTICAL", propFlag->as_string());
    }
    else
    {
        value = ClearMultiplePropFlags("wxALIGN_LEFT|wxALIGN_RIGHT|wxALIGN_CENTER_HORIZONTAL", propFlag->as_string());
    }

    const char* alignStr;
    switch (align)
    {
        case wxALIGN_RIGHT:
            alignStr = "wxALIGN_RIGHT";
            break;

        case wxALIGN_CENTER_HORIZONTAL:
            alignStr = "wxALIGN_CENTER_HORIZONTAL";
            break;

        case wxALIGN_BOTTOM:
            alignStr = "wxALIGN_BOTTOM";
            break;

        case wxALIGN_CENTER_VERTICAL:
            alignStr = "wxALIGN_CENTER_VERTICAL";
            break;

        default:
            alignStr = "";
            break;
    }

    ModifyProperty(propFlag, SetPropFlag(alignStr, value));
}

bool MainFrame::GetLayoutSettings(int* flag, int* option, int* border, int* orient)
{
    if (!m_selected_node || !m_selected_node->GetParent() || !m_selected_node->GetParent()->IsSizer())
    {
        return false;
    }

    auto prop_flags = m_selected_node->GetSizerFlags();

    auto propOption = m_selected_node->get_prop_ptr(prop_proportion);
    if (propOption)
    {
        *option = prop_flags.GetProportion();
    }

    *flag = prop_flags.GetFlags();
    *border = prop_flags.GetBorderInPixels();

    auto sizer = m_selected_node->GetParent();
    if (sizer)
    {
        if (sizer->isGen(gen_wxBoxSizer) || m_selected_node->IsStaticBoxSizer())
        {
            auto propOrient = sizer->get_prop_ptr(prop_orientation);
            if (propOrient)
            {
                *orient = propOrient->as_int();
            }
        }
    }
    return true;
}

bool MainFrame::MoveNode(Node* node, MoveDirection where, bool check_only)
{
    auto parent = node->GetParent();

    ASSERT(parent || node->isGen(gen_Project));
    if (!parent)
        return false;

    if (parent->isGen(gen_wxGridBagSizer))
        return GridBag::MoveNode(node, where, check_only);

    if (where == MoveDirection::Left)
    {
        if (node->isGen(gen_folder))
            return false;
        else if (node->isGen(gen_sub_folder) && parent->isGen(gen_folder))
            return false;  // You can't have Project as the parent of a sub_folder

        if (parent->isGen(gen_folder) || parent->isGen(gen_sub_folder))
        {
            if (!check_only)
            {
                wxWindowUpdateLocker freeze(this);
                PushUndoAction(std::make_shared<ChangeParentAction>(node, parent->GetParent()));
            }
            return true;
        }

        auto grandparent = parent->GetParent();
        while (grandparent && !grandparent->IsSizer())
        {
            grandparent = grandparent->GetParent();
        }

        if (check_only)
            return (grandparent ? true : false);

        if (grandparent)
        {
            wxWindowUpdateLocker freeze(this);
            PushUndoAction(std::make_shared<ChangeParentAction>(node, grandparent));
            return true;
        }
        wxMessageBox("There is no sizer to the left of this item that it can be moved into.", "Move item");
    }
    else if (where == MoveDirection::Right)
    {
        if (node->isGen(gen_folder))
            return false;

        auto pos = parent->GetChildPosition(node) - 1;
        if (pos < parent->GetChildCount())
        {
            if (node->IsForm() && pos >= 0)
            {
                auto* new_parent = parent->GetChild(pos);
                if (new_parent->IsForm())
                {
                    if (!check_only)
                        wxMessageBox("You cannot move a form to the right of another form.", "Move item");
                    return false;
                }
                else if (new_parent->isGen(gen_folder) || new_parent->isGen(gen_sub_folder))
                {
                    if (!check_only)
                    {
                        wxWindowUpdateLocker freeze(this);
                        PushUndoAction(std::make_shared<ChangeParentAction>(node, new_parent));
                    }
                    return true;
                }
            }
            else if (node->isGen(gen_sub_folder) && pos >= 0)
            {
                auto* new_parent = parent->GetChild(pos);
                while (new_parent->IsForm())
                {
                    if (pos == 0)
                    {
                        if (!check_only)
                            wxMessageBox("You cannot move a folder to the right of a form.", "Move item");
                        return false;
                    }
                }
                if (new_parent->isGen(gen_folder) || new_parent->isGen(gen_sub_folder))
                {
                    if (!check_only)
                    {
                        wxWindowUpdateLocker freeze(this);
                        PushUndoAction(std::make_shared<ChangeParentAction>(node, new_parent));
                    }
                    return true;
                }
            }
            parent = FindChildSizerItem(parent->GetChild(pos), true);

            if (check_only)
                return (parent ? true : false);

            if (parent)
            {
                wxWindowUpdateLocker freeze(this);
                PushUndoAction(std::make_shared<ChangeParentAction>(node, parent));
                return true;
            }
        }
        if (!check_only)
            wxMessageBox("There is nothing above this item that it can be moved into.", "Move item");
    }
    else if (where == MoveDirection::Up)
    {
        auto pos = parent->GetChildPosition(node);
        if (check_only)
            return (pos > 0);
        if (pos > 0)
        {
            wxWindowUpdateLocker freeze(this);
            PushUndoAction(std::make_shared<ChangePositionAction>(node, pos - 1));
            return true;
        }
        wxMessageBox("This component cannot be moved up any further.", "Move item");
    }
    else if (where == MoveDirection::Down)
    {
        auto pos = parent->GetChildPosition(node) + 1;
        if (check_only)
            return (pos < parent->GetChildCount());
        if (pos < parent->GetChildCount())
        {
            wxWindowUpdateLocker freeze(this);
            PushUndoAction(std::make_shared<ChangePositionAction>(node, pos));
            return true;
        }
        wxMessageBox(ttlib::cstr() << node->DeclName() << " cannot be moved down any lower.", "Move item");
    }

    return false;
}

void MainFrame::RemoveNode(Node* node, bool isCutMode)
{
    ASSERT_MSG(!node->isType(type_project), "Don't call RemoveNode to remove the entire project.");
    ASSERT_MSG(node->GetParent(), "The node being removed has no parent -- that should be impossible.");

    auto parent = node->GetParent();
    if (!parent)
        return;

    if (isCutMode)
    {
        ttlib::cstr undo_str;
        undo_str << "cut " << node->DeclName();
        PushUndoAction(std::make_shared<RemoveNodeAction>(node, undo_str, true));
    }
    else
    {
        ttlib::cstr undo_str;
        undo_str << "delete " << node->DeclName();
        PushUndoAction(std::make_shared<RemoveNodeAction>(node, undo_str, false));
    }
    UpdateWakaTime();
}

void MainFrame::ChangeEventHandler(NodeEvent* event, const ttlib::cstr& value)
{
    if (event && value != event->get_value())
    {
        PushUndoAction(std::make_shared<ModifyEventAction>(event, value));
        UpdateWakaTime();
    }
}

Node* MainFrame::FindChildSizerItem(Node* node, bool include_splitter)
{
    if (include_splitter && node->isGen(gen_wxSplitterWindow) && node->GetChildCount() < 2)
        return node;
    else if (node->GetNodeDeclaration()->isSubclassOf(gen_sizer_dimension))
        return node;
    else
    {
        for (const auto& child: node->GetChildNodePtrs())
        {
            if (auto result = FindChildSizerItem(child, include_splitter); result)
            {
                return result;
            }
        }
    }

    return nullptr;
}

void MainFrame::UpdateWakaTime(bool FileSavedEvent)
{
    if (m_wakatime && wxGetApp().Preferences().is_WakaTimeEnabled())
    {
        m_wakatime->SendHeartbeat(FileSavedEvent);
    }
}

void MainFrame::RemoveFileFromHistory(ttString file)
{
    if (file.empty())
        return;

    for (size_t idx = 0; idx < m_FileHistory.GetCount(); ++idx)
    {
        if (file == m_FileHistory.GetHistoryFile(idx))
        {
            m_FileHistory.RemoveFileFromHistory(idx);
            break;
        }
    }
}

void MainFrame::PushUndoAction(UndoActionPtr cmd, bool add_to_stack)
{
    m_isProject_modified = true;
    if (!add_to_stack)
        cmd->Change();
    else
        m_undo_stack.Push(cmd);
}

#if defined(INTERNAL_TESTING)

void MainFrame::OnCodeCompare(wxCommandEvent& WXUNUSED(event))
{
    CodeCompare dlg(this);
    dlg.ShowModal();
}

#endif

Node* FindChildNode(Node* node, GenEnum::GenName name)
{
    for (const auto& child: node->GetChildNodePtrs())
    {
        if (child->isGen(name))
        {
            return child.get();
        }
        else if (child->GetChildCount() > 0)
        {
            if (auto child_node = FindChildNode(child.get(), name); child_node)
            {
                return child_node;
            }
        }
    }
    return nullptr;
}

#if defined(_DEBUG) || defined(INTERNAL_TESTING)

void MainFrame::OnFindWidget(wxCommandEvent& WXUNUSED(event))
{
    InsertWidget dlg(this);
    if (dlg.ShowModal() == wxID_OK)
    {
        if (auto result = rmap_GenNames.find(dlg.GetWidget()); result != rmap_GenNames.end())
        {
            auto start_node = GetSelectedNode();
            if (!start_node)
            {
                start_node = GetProject();
            }
            auto found_node = FindChildNode(start_node, result->second);
            if (found_node)
            {
                SelectNode(found_node, evt_flags::fire_event | evt_flags::force_selection);
            }
            else
            {
                wxMessageBox(wxString() << "Unable to find " << dlg.GetWidget().wx_str());
            }
        }
        else
        {
            wxMessageBox(wxString() << "Cannot find a generator for " << dlg.GetWidget().wx_str());
        }
    }
}

#endif  // defined(_DEBUG) || defined(INTERNAL_TESTING)
