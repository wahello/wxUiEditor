set (file_list_dir ${CMAKE_CURRENT_LIST_DIR})

set (file_list

    wxUiEditor.rc         # Only processed when compiling for Windows, ignored otherwise

    mainapp.cpp           # Main application class
    mainframe.cpp         # Main window frame
    assertion_dlg.cpp     # Assertion Dialog

    bitmaps.cpp           # Map of bitmaps accessed by name
    clipboard.cpp         # Handles reading and writing OS clipboard data
    cstm_event.cpp        # Custom Event handling
    frame_status_bar.cpp  # MainFrame status bar functions
    gen_enums.cpp         # Enumerations for generators
    image_bundle.cpp      # Functions for working with wxBitmapBundle
    lambdas.cpp           # Functions for formatting and storage of lamda events
    paths.cpp             # Handles *_directory properties
    previews.cpp          # Top level Preview functions
    project_class.cpp     # Project class
    startup_dlg.cpp       # Startup dialog
    undo_cmds.cpp         # Undoable command classes derived from UndoStackCmd
    undo_stack.cpp        # Maintain an undo and redo stack
    wakatime.cpp          # Updates WakaTime metrics

    # Custom property handling for Property Grid panel

    customprops/code_string_prop.cpp    # Derived wxStringProperty class for code
    customprops/custom_colour_prop.cpp  # Property editor for colour
    customprops/custom_param_prop.cpp   # Derived wxStringProperty class for custom control parameters
    customprops/directory_prop.cpp      # Derived wxStringProperty class for choosing a directory
    customprops/evt_string_prop.cpp     # Derived wxStringProperty class for event function
    customprops/font_string_prop.cpp    # Derived wxStringProperty class for font property
    customprops/html_string_prop.cpp    # Derived wxStringProperty class for HTML
    customprops/img_string_prop.cpp     # Derived wxStringProperty class for handling wxImage files or art
    customprops/rearrange_prop.cpp      # wxRearangeList contents editor
    customprops/sb_fields_prop.cpp      # Property editor for status bar fields
    customprops/txt_string_prop.cpp     # Derived wxStringProperty class for single-line text

    customprops/art_prop_dlg.cpp        # Art Property Dialog for image property
    customprops/eventhandler_dlg.cpp    # Dialog for editing event handlers
    customprops/font_prop_dlg.cpp       # Dialog for editing Font Property
    customprops/img_props.cpp           # Handles property grid image properties

    customprops/pg_image.cpp            # Custom property grid class for images
    customprops/pg_animation.cpp        # Custom property grid class for animations
    customprops/pg_point.cpp            # Custom wxPGProperty for wxPoint

    custom_ctrls/colour_rect_ctrl.cpp   # Control that displays a solid color

    # Code generation modules

    generate/code.cpp              # Helper class for generating code
    generate/gen_base.cpp          # Generate Src and Hdr files for Base and Derived Class
    generate/gen_construction.cpp  # Top level Object construction code
    generate/gen_cmake.cpp         # Auto-generate a .cmake file
    generate/write_code.cpp        # Write code to Scintilla or file

    generate/base_generator.cpp    # Base widget generator class
    generate/gen_initialize.cpp    # Initialize all widget generate classes
    generate/gen_common.cpp        # Common widget generation functions
    generate/gen_lang_common.cpp   # Common mulit-language functions
    generate/gen_xrc_utils.cpp     # Common XRC generating functions
    generate/gen_book_utils.cpp    # Common Book utilities

    # Code generation output languages

    generate/gen_codefiles.cpp     # Generate code files
    generate/gen_derived.cpp       # Generate Derived class code
    generate/gen_python.cpp        # Generate Python code files
    generate/gen_xrc.cpp           # Generate XRC file

    # Generators are responsible for displaying the widget in the Mockup window,
    # and generating both C++ and XRC code.

    generate/dataview_widgets.cpp  # wxDataView generation classes
    generate/grid_widgets.cpp      # Grid generation classes
    generate/images_form.cpp       # Embedded images generator
    generate/menu_widgets.cpp      # Menu generation classes
    generate/project.cpp           # Project generator
    generate/ribbon_widgets.cpp    # Ribbon generation classes
    generate/styled_text.cpp       # wxStyledText (scintilla) generate
    generate/window_widgets.cpp    # Splitter and Scroll generation classes

    generate/gen_activity.cpp       # wxActivityIndicator generator
    generate/gen_animation.cpp      # wxAnimationCtrl generator
    generate/gen_aui_toolbar.cpp    # wxAuiToolBar generator
    generate/gen_banner_window.cpp  # wxBannerWindow generator
    generate/gen_bitmap_combo.cpp   # wxBitmapComboBox generator
    generate/gen_button.cpp         # wxButton generator
    generate/gen_calendar_ctrl.cpp  # wxCalendarCtrl generator
    generate/gen_check_listbox.cpp  # wxCheckListBox generator
    generate/gen_checkbox.cpp       # wxCheckBox generator
    generate/gen_choice.cpp         # wxChoice generator
    generate/gen_close_btn.cpp      # wxBitmapButton::CreateCloseButton generator
    generate/gen_cmd_link_btn.cpp   # wxCommandLinkButton generator
    generate/gen_collapsible.cpp    # wxCollapsiblePane generator
    generate/gen_combobox.cpp       # wxComboBox generator
    generate/gen_custom_ctrl.cpp    # Custom Control generator
    generate/gen_dir_ctrl.cpp       # wxGenericDirCtrl generator
    generate/gen_edit_listbox.cpp   # wxEditableListBox generator
    generate/gen_file_ctrl.cpp      # wxFileCtrl generator
    generate/gen_gauge.cpp          # wxGauge generator
    generate/gen_grid.cpp           # wxGrid generator
    generate/gen_html_listbox.cpp   # wxSimpleHtmlListBox generator
    generate/gen_html_window.cpp    # wxHtmlWindow generator
    generate/gen_hyperlink.cpp      # wxHyperlinkCtrl generator
    generate/gen_infobar.cpp        # wxInfoBar generator
    generate/gen_listbox.cpp        # wxListBox generator
    generate/gen_listview.cpp       # wxListView generator
    generate/gen_panel.cpp          # wxPanel generator
    generate/gen_prop_grid.cpp      # wxPropertyGrid generator
    generate/gen_prop_item.cpp      # PropertyGrid/Manager Item generator
    generate/gen_radio_box.cpp      # wxRadioBox generator
    generate/gen_radio_btn.cpp      # wxRadioButton generator
    generate/gen_rearrange.cpp      # wxRearrangeCtrl generator
    generate/gen_rich_text.cpp      # wxRichTextCtrl generator
    generate/gen_scrollbar.cpp      # wxScrollBar generator
    generate/gen_search_ctrl.cpp    # wxSearchCtrl generator
    generate/gen_slider.cpp         # wxSlider generator
    generate/gen_spin_btn.cpp       # wxSpinButton generator
    generate/gen_spin_ctrl.cpp      # wxSpinCtrl generator
    generate/gen_split_win.cpp      # wxSplitterWindow generator
    generate/gen_static_bmp.cpp     # wxStaticBitmap generator
    generate/gen_static_box.cpp     # wxStaticBox generator
    generate/gen_static_line.cpp    # wxStaticLine generator
    generate/gen_static_text.cpp    # wxStaticText generator
    generate/gen_status_bar.cpp     # wxStatusBar generator
    generate/gen_text_ctrl.cpp      # wxTextCtrl generator
    generate/gen_toggle_btn.cpp     # wxToggleButton generator
    generate/gen_toolbar.cpp        # wxToolBar generator
    generate/gen_tree_ctrl.cpp      # wxTreeCtrl generator
    generate/gen_tree_list.cpp      # wxTreeListCtrl generator
    generate/gen_web_view.cpp       # wxWebView generator

    # Books

    generate/gen_aui_notebook.cpp       # wxAuiNotebook generator
    generate/gen_book_page.cpp          # Book page generator
    generate/gen_choicebook.cpp         # wxChoicebook generator
    generate/gen_listbook.cpp           # wxListbook generator
    generate/gen_notebook.cpp           # wxNotebook generator
    generate/gen_page_ctrl.cpp          # Page control generator
    generate/gen_simplebook.cpp         # wxSimplebook generator
    generate/gen_toolbook.cpp           # wxToolbook generator
    generate/gen_treebook.cpp           # wxTreebook generator

    # Forms

    generate/gen_dialog.cpp             # wxDialog generator
    generate/gen_frame.cpp              # wxFrame generator
    generate/gen_panel_form.cpp         # wxPanel Form generator
    generate/gen_popup_trans_win.cpp    # wxPopupTransientWindow generator
    generate/gen_wizard.cpp             # wxWizard generator

    # Pickers

    generate/gen_clr_picker.cpp         # wxColourPickerCtrl generator
    generate/gen_date_picker.cpp        # wxDatePickerCtrl generator
    generate/gen_dir_picker.cpp         # wxDirPickerCtrl generator
    generate/gen_file_picker.cpp        # wxFilePickerCtrl generator
    generate/gen_font_picker.cpp        # wxFontPickerCtrl generator
    generate/gen_time_picker.cpp        # wxTimePickerCtrl generator

    # Sizers

    generate/gen_box_sizer.cpp          # wxBoxSizer generator
    generate/gen_flexgrid_sizer.cpp     # wxFlexGridSizer generator
    generate/gen_grid_sizer.cpp         # wxGridSizer generator
    generate/gen_gridbag_sizer.cpp      # wxGridBagSizer generator
    generate/gen_spacer_sizer.cpp       # Add space to sizer generator
    generate/gen_statchkbox_sizer.cpp   # wxStaticBoxSizer with wxCheckBox generator
    generate/gen_staticbox_sizer.cpp    # wxStaticBoxSizer gemeratpr
    generate/gen_statradiobox_sizer.cpp # wxStaticBoxSizer with wxRadioButton generator
    generate/gen_std_dlgbtn_sizer.cpp   # wxStdDialogButtonSizer generator
    generate/gen_text_sizer.cpp         # wxTextSizerWrapper generator
    generate/gen_wrap_sizer.cpp         # wxWrapSizer generator

    # Importers (also see Windows Resource importer below)

    import/import_crafter_maps.cpp  # wxCrafter mappings
    import/import_formblder.cpp     # Import a wxFormBuider project
    import/import_wxcrafter.cpp     # Import a wxCrafter project
    import/import_wxglade.cpp       # Import a Import a wxGlade file
    import/import_wxsmith.cpp       # Process XRC files
    import/import_xml.cpp           # Base class for XML importing

    # Mockup panel

    mockup/mockup_content.cpp   # Mockup of a form's contents
    mockup/mockup_parent.cpp    # Top-level MockUp Parent window
    mockup/mockup_preview.cpp   # Preview Mockup
    mockup/mockup_wizard.cpp    # Emulate a wxWizard

    # Dialogs for creating new forms

    newdialogs/new_common.cpp   # Contains code common between all new_ dialogs
    newdialogs/new_dialog.cpp   # Dialog for creating a new project dialog
    newdialogs/new_frame.cpp    # Dialog for creating a new project wxFrame
    newdialogs/new_panel.cpp    # Dialog for creating a new form panel
    newdialogs/new_ribbon.cpp   # Dialog for creating a new wxRibbonBar
    newdialogs/new_wizard.cpp   # Dialog for creating a new wxWizard

    # Nodes

    nodes/node.cpp              # Contains user-modifiable node
    nodes/node_constants.cpp    # Maps wxWidgets constants to their numerical value
    nodes/node_creator.cpp      # Class used to create nodes
    nodes/node_decl.cpp         # Contains the declarations for a node (properties, events, etc.)
    nodes/node_gridbag.cpp      # Create and modify a node containing a wxGridBagSizer
    nodes/node_init.cpp         # Initialize NodeCreator class
    nodes/node_prop.cpp         # NodeProperty class
    nodes/tool_creator.cpp      # Functions for creating new nodes from Ribbon Panel

    # Panels

    panels/cstm_propgrid.cpp    # Derived wxPropertyGrid class
    panels/code_display.cpp     # Display code in scintilla control
    panels/base_panel.cpp       # Code generation panel
    panels/nav_panel.cpp        # Navigation panel
    panels/navpopupmenu.cpp     # Context-menu for Navigation panel
    panels/propgrid_panel.cpp   # Property panel
    panels/ribbon_tools.cpp     # Displays node creation tools in a wxRibbonBar

    # Misc

    project/loadproject.cpp     # Load wxUiEditor project
    project/saveproject.cpp     # Save a wxUiEditor project file

    utils/font_prop.cpp         # FontProperty class
    utils/utils.cpp             # Utility functions that work with properties

    # Windows resource importer

    winres/ctrl_utils.cpp       # resCtrl class utility functions
    winres/form_utils.cpp       # resForm utility functions code
    winres/import_winres.cpp    # Import a Windows resource file
    winres/winres_ctrl.cpp      # Process Windows Resource control data
    winres/winres_dlg.cpp       # Process a Windows Resource DIALOG or DIALOGEX
    winres/winres_menu.cpp      # Process a Windows Resource MENU
    winres/winres_images.cpp    # resCtrl class image/icon functions
    winres/winres_layout.cpp    # resForm layout code
    winres/winres_styles.cpp    # resCtrl class style processing functions

    ../pugixml/pugixml.cpp      # XML parser

    ui/generate_xrc_dlg.cpp     # Dialog for generating XRC file(s)
    ui/generate_dlg.cpp         # Dialog for choosing and generating specific language file(s)
    ui/gridbag_item.cpp         # Dialog for inserting an item into a wxGridBagSizer node
    ui/import_dlg.cpp           # Dialog to import one or more projects
    ui/importwinresdlg.cpp      # Dialog for Importing a Windows resource file
    ui/insertwidget.cpp         # Dialog to lookup and insert a widget
    ui/optionsdlg.cpp           # Dialog containing special Debugging commands
    ui/xrccompare.cpp           # C++/XRC UI Comparison dialog

    ui/preview_settings_events.cpp  # Event handlers for PreviewSettings dialog

    # Debug-only files
    $<$<CONFIG:Debug>:internal/code_compare.cpp>    # Compare code generation
    $<$<CONFIG:Debug>:internal/convert_img.cpp>     # Convert image
    $<$<CONFIG:Debug>:internal/import_panel.cpp>    # Panel to display original imported file
    $<$<CONFIG:Debug>:internal/node_info.cpp>       # Node memory usage dialog
    $<$<CONFIG:Debug>:internal/xrcpreview.cpp>      # Test XRC

    $<$<CONFIG:Debug>:internal/code_compare_base.cpp>
    $<$<CONFIG:Debug>:internal/convert_img_base.cpp>
    $<$<CONFIG:Debug>:internal/nodeinfo_base.cpp>
    $<$<CONFIG:Debug>:internal/xrcpreview_base.cpp>

    $<$<CONFIG:Debug>:internal/debugsettings.cpp>  # Settings while running the Debug version of wxUiEditor
    $<$<CONFIG:Debug>:internal/msg_logging.cpp>    # Message logging class
    $<$<CONFIG:Debug>:internal/msgframe.cpp>       # Stores messages

    $<$<CONFIG:Debug>:internal/msgframe_base.cpp>       # wxUiEditor generated file
    $<$<CONFIG:Debug>:internal/debugsettings_base.cpp>  # wxUiEditor generated file

    $<$<CONFIG:Debug>:tests/test_xrc_import.cpp>  # XRC Import tests
)

set (doc_list

    ../CHANGELOG.md
    ../README.md
    ../docs/DEV_NOTES.md
    ../docs/images.md
    ../docs/import_crafter.md
    ../docs/import_formbuilder.md
    ../docs/import_winres.md
    ../docs/xrc.md
    generate/CONTENTS.md
    generate/README.md
    nodes/README.md
    winres/README.md
    xml/README.md

)

set (xml_list

    # interface declarations

    xml/interface_xml.xml
    xml/sizer_child_xml.xml
    xml/validators_xml.xml
    xml/window_interfaces_xml.xml

    # generator declarations

    xml/aui_xml.xml
    xml/bars_xml.xml
    xml/books_xml.xml
    xml/boxes_xml.xml
    xml/buttons_xml.xml
    xml/containers_xml.xml
    xml/dataview_xml.xml
    xml/dialogs_xml.xml
    xml/forms_xml.xml
    xml/grid_xml.xml
    xml/listview_xml.xml
    xml/menus_xml.xml
    xml/pickers_xml.xml
    xml/project_xml.xml
    xml/propgrid_xml.xml
    xml/ribbon_xml.xml
    xml/scintilla_xml.xml
    xml/sizers_xml.xml
    xml/std_dlg_btns_xml.xml
    xml/textctrls_xml.xml
    xml/toolbars_xml.xml
    xml/trees_xml.xml
    xml/widgets_xml.xml
    xml/wizard_xml.xml

)
