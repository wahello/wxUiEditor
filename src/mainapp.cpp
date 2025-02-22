/////////////////////////////////////////////////////////////////////////////
// Purpose:   Main application class
// Author:    Ralph Walden
// Copyright: Copyright (c) 2020-2022 KeyWorks Software (Ralph Walden)
// License:   Apache License -- see ../LICENSE
/////////////////////////////////////////////////////////////////////////////

#include <wx/cmdline.h>  // wxCmdLineParser and related classes for parsing the command
#include <wx/config.h>   // wxConfig base header
#include <wx/cshelp.h>   // Context-sensitive help support classes
#include <wx/filedlg.h>  // wxFileDialog base header
#include <wx/sysopt.h>   // wxSystemOptions
#include <wx/utils.h>    // Miscellaneous utilities

#include "mainapp.h"

#include "bitmaps.h"        // Contains various images handling functions
#include "mainframe.h"      // MainFrame -- Main window frame
#include "node.h"           // Node -- Node class
#include "node_creator.h"   // NodeCreator class
#include "project_class.h"  // Project class
#include "utils.h"          // Utility functions that work with properties

#include "wxui/startupdlg_base.h"  // CStartup -- Dialog to display is wxUE is launched with no arguments

#include "pugixml.hpp"

#ifdef _MSC_VER
    #if defined(_WIN32)

        #pragma comment(lib, "kernel32.lib")
        #pragma comment(lib, "user32.lib")
        #pragma comment(lib, "gdi32.lib")
        #pragma comment(lib, "comctl32.lib")
        #pragma comment(lib, "comdlg32.lib")
        #pragma comment(lib, "shell32.lib")

        #pragma comment(lib, "rpcrt4.lib")
        #pragma comment(lib, "advapi32.lib")

        #if wxUSE_URL_NATIVE
            #pragma comment(lib, "wininet.lib")
        #endif
    #endif
#endif

#if defined(_DEBUG)
ttlib::cstr widgets_build_signature = WX_BUILD_OPTIONS_SIGNATURE;
#endif  // _DEBUG

wxIMPLEMENT_APP(App);

#if defined(_WIN32) && defined(_DEBUG)

// There are several classes that are created during initialization which are not explicitly deleted before the memory check,
// so don't be surprised to see quite a few memory "leaks"

// Uncomment the following to turn on a memory leak dump on exit.
// #define USE_CRT_MEMORY_DUMP

#endif  // _WIN32 && defined(_DEBUG)

ttlib::cstr tt_empty_cstr;

App::App()
{
    m_lang = wxLANGUAGE_DEFAULT;
}

bool App::OnInit()
{
#if defined(_WIN32) && defined(_DEBUG)
    #if !defined(USE_CRT_MEMORY_DUMP)

    _CrtSetDbgFlag(0);

    #else

    // If memory dump shows a leak, uncomment the following and use the memory dump number you want to check. Works with
    // Visual Studio Debugger, does not work with Visual Studio Code debugger.

    // _CrtSetBreakAlloc(25045);
    #endif
#endif

    wxInitAllImageHandlers();

    // The name is sort of a standard. More importantly, it is sometimes used as the mask in Windows bitmaps for toolbar
    // images.
    wxTheColourDatabase->AddColour("Grey94", wxColour(240, 240, 240));

#ifdef _MSC_VER
    #if defined(wxUSE_ON_FATAL_EXCEPTION)
    ::wxHandleFatalExceptions(true);
    #endif
#endif

#if defined(_WIN32)
    // Docs say we need this since our toolbar bitmaps >16 colors
    wxSystemOptions::SetOption("msw.remap", 0);  // only remap background of toolbar bitmaps, not the actual content
#endif

    // If we're just providing text-popups for help, then this is all we need.
    wxHelpProvider::Set(new wxSimpleHelpProvider);

#if defined(_DEBUG) || defined(INTERNAL_TESTING)
    g_pMsgLogging = new MsgLogging();
#endif

#if defined(_DEBUG)
    // wxLog only exists in _DEBUG builds
    wxLog::SetActiveTarget(g_pMsgLogging);
#endif  // _DEBUG

    SetVendorName("KeyWorks");
    m_prefs.ReadConfig();

    return true;
}

int App::OnRun()
{
    g_NodeCreator.Initialize();
    m_frame = new MainFrame();
    bool is_project_loaded = false;

    wxCmdLineParser parser(argc, argv);
    OnInitCmdLine(parser);
    parser.AddParam("Filename", wxCMD_LINE_VAL_STRING, wxCMD_LINE_PARAM_OPTIONAL);
    parser.Parse();
    if (parser.GetParamCount())
    {
        ttString filename = parser.GetParam(0);
        filename.make_absolute();
        if (filename.file_exists())
        {
            if (!filename.extension().is_sameas(".wxui", tt::CASE::either) &&
                !filename.extension().is_sameas(".wxue", tt::CASE::either))
            {
                is_project_loaded = ImportProject(filename);
            }
            else
            {
                is_project_loaded = LoadProject(filename);
            }
        }
    }

    if (!is_project_loaded)
    {
        StartupDlg start_dlg(nullptr);
        if (auto result = start_dlg.ShowModal(); result == wxID_OK)
        {
            switch (start_dlg.GetCommandType())
            {
                case StartupDlg::START_MRU:
                    if (!start_dlg.GetProjectFile().extension().is_sameas(".wxui", tt::CASE::either) &&
                        !start_dlg.GetProjectFile().extension().is_sameas(".wxue", tt::CASE::either))
                    {
                        is_project_loaded = ImportProject(start_dlg.GetProjectFile());
                    }
                    else
                    {
                        is_project_loaded = LoadProject(start_dlg.GetProjectFile());
                    }
                    break;

                case StartupDlg::START_EMPTY:
                    is_project_loaded = NewProject(true);
                    break;

                case StartupDlg::START_CONVERT:
                    is_project_loaded = NewProject(false);
                    break;

                case StartupDlg::START_OPEN:
                    {
                        // TODO: [KeyWorks - 02-21-2021] A CodeBlocks file will contain all of the wxSmith resources -- so it
                        // would actually make sense to process it since we can combine all of those resources into our
                        // single project file.

                        wxFileDialog dialog(nullptr, "Open or Import Project", wxEmptyString, wxEmptyString,
                                            "wxUiEditor Project File (*.wxui)|*.wxui"
                                            "|wxCrafter Project File (*.wxcp)|*.wxcp"
                                            "|wxFormBuilder Project File (*.fbp)|*.fbp"
                                            "|wxGlade File (*.wxg)|*.wxg"
                                            "|wxSmith File (*.wxs)|*.wxs"
                                            "|XRC File (*.xrc)|*.xrc"
                                            "|Windows Resource File (*.rc)|*.rc||",
                                            wxFD_OPEN);

                        if (dialog.ShowModal() == wxID_OK)
                        {
                            ttString filename = dialog.GetPath();
                            if (!filename.extension().is_sameas(".wxui", tt::CASE::either) &&
                                !filename.extension().is_sameas(".wxue", tt::CASE::either))
                            {
                                is_project_loaded = ImportProject(filename);
                            }
                            else
                            {
                                is_project_loaded = LoadProject(dialog.GetPath());
                            }
                        }
                    }
                    break;
            }
        }
    }

    if (is_project_loaded)
    {
        m_frame->Show();
        SetTopWindow(m_frame);

#if defined(_DEBUG)
        // if (AutoMsgWindow())
        // ShowMsgWindow();
#endif  // _DEBUG

        return wxApp::OnRun();
    }
    else
    {
        m_frame->Close();
        return 1;
    }
}

int App::OnExit()
{
    // This must get deleted in order to stop any thread it started to process embedded images
    m_project.reset();

    return wxApp::OnExit();
}

#if defined(_DEBUG) && defined(wxUSE_ON_FATAL_EXCEPTION) && defined(wxUSE_STACKWALKER)

    #include <wx/stackwalk.h>

class StackLogger : public wxStackWalker
{
public:
    auto& GetCalls() { return m_calls; }

protected:
    void OnStackFrame(const wxStackFrame& frame) override
    {
        if (frame.HasSourceLocation())
        {
            ttlib::cstr source;
            source << frame.GetFileName().wx_str() << ':' << (to_int) frame.GetLine();

            wxString params;
            if (auto paramCount = frame.GetParamCount(); paramCount > 0)
            {
                params << "(";

                for (size_t i = 0; i < paramCount; ++i)
                {
                    wxString type, name, value;
                    if (frame.GetParam(i, &type, &name, &value))
                    {
                        params << type << " " << name << " = " << value << ", ";
                    }
                }

                params << ")";
            }

            if (params.size() > 100)
                params = "(...)";

            m_calls.emplace_back() << (to_int) frame.GetLevel() << ' ' << frame.GetName().wx_str() << params.wx_str() << ' '
                                   << source;
        }
        else
        {
            m_calls.emplace_back() << (to_int) frame.GetLevel() << ' ' << frame.GetName().wx_str();
        }
    }

    std::vector<ttlib::cstr> m_calls;
};

#endif  // defined(_DEBUG) && defined(wxUSE_ON_FATAL_EXCEPTION) && defined(wxUSE_STACKWALKER)

#if defined(_MSC_VER) && defined(wxUSE_ON_FATAL_EXCEPTION)

void App::OnFatalException()
{
    #if defined(_DEBUG)

    StackLogger logger;
    logger.WalkFromException();
    for (auto& iter: logger.GetCalls())
    {
        // We're only interested in our own source code, so ignore the rest.
        if (!iter.contains("wxUiEditor"))
            continue;

        wxLogDebug(iter.c_str());
    }

    // We now have the relevant call stack displayed in the debugger, so break into it.
    wxTrap();

    #endif  // _DEBUG

    // Let the user know something terrible happened.
    wxMessageBox("A fatal exception has occured!", txtVersion);
}

#endif  // defined(_MSC_VER) && defined(wxUSE_ON_FATAL_EXCEPTION)

#if defined(_DEBUG) || defined(INTERNAL_TESTING)

void App::ShowMsgWindow()
{
    g_pMsgLogging->ShowLogger();
}

void App::DbgCurrentTest(wxCommandEvent&)
{
    FAIL_MSG("test FAIL_MSG() macro")

    wxMessageBox("Add code you want to test to (mainapp.cpp) App::DbgCurrentTest()", txtVersion);
}

#endif

void PREFS::ReadConfig()
{
    auto* config = wxConfig::Get();
    config->SetPath("/preferences");

    m_flags = config->ReadLong("flags", PREFS_MSG_WINDOW | PREFS_MSG_INFO | PREFS_MSG_EVENT | PREFS_MSG_WARNING);
    m_project_flags = config->ReadLong("project_flags", PREFS_PJT_MEMBER_PREFIX);
    m_preview_type = static_cast<PREVIEW_TYPE>(config->ReadLong("preview_type", PREVIEW_TYPE_XRC));

    m_sizers_all_borders = config->ReadBool("all_borders", true);
    m_sizers_always_expand = config->ReadBool("always_expand", true);
    m_enable_wakatime = config->ReadBool("enable_wakatime", true);

    config->SetPath("/");
}

void PREFS::WriteConfig()
{
    auto* config = wxConfig::Get();
    config->SetPath("/preferences");

    config->Write("all_borders", m_sizers_all_borders);
    config->Write("always_expand", m_sizers_always_expand);
    config->Write("enable_wakatime", m_enable_wakatime);

    config->SetPath("/");
}
