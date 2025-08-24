/*
 * NURBS Map Editor - Main Editor Implementation
 * A Roblox Studio-like editor for NURBS-based FPS games with full IGES support
 */

#include "editor.h"
#include "iges_loader.h"
#include "nurbs_primitives.h"
#include <string.h>
#include <math.h>
#include <stdio.h>
#include <stdlib.h>

// Forward declarations for callback functions
static void on_new_scene(GtkWidget *widget, gpointer data);
static void on_open_scene(GtkWidget *widget, gpointer data);
static void on_save_scene(GtkWidget *widget, gpointer data);
static void on_save_scene_as(GtkWidget *widget, gpointer data);
static void on_export_for_game(GtkWidget *widget, gpointer data);
static void on_import_iges(GtkWidget *widget, gpointer data);
static void on_export_iges(GtkWidget *widget, gpointer data);
static void on_quit(GtkWidget *widget, gpointer data);

static void on_undo(GtkWidget *widget, gpointer data);
static void on_redo(GtkWidget *widget, gpointer data);
static void on_delete_selected(GtkWidget *widget, gpointer data);
static void on_duplicate_selected(GtkWidget *widget, gpointer data);

static void on_insert_sphere(GtkWidget *widget, gpointer data);
static void on_insert_plane(GtkWidget *widget, gpointer data);
static void on_insert_cylinder(GtkWidget *widget, gpointer data);
static void on_insert_torus(GtkWidget *widget, gpointer data);
static void on_insert_point_light(GtkWidget *widget, gpointer data);
static void on_insert_directional_light(GtkWidget *widget, gpointer data);
static void on_insert_spot_light(GtkWidget *widget, gpointer data);

static void on_reset_camera(GtkWidget *widget, gpointer data);
static void on_focus_selected(GtkWidget *widget, gpointer data);
static void on_toggle_wireframe(GtkWidget *widget, gpointer data);
static void on_toggle_grid(GtkWidget *widget, gpointer data);
static void on_toggle_lighting(GtkWidget *widget, gpointer data);

static void on_tool_select(GtkWidget *widget, gpointer data);
static void on_tool_move(GtkWidget *widget, gpointer data);
static void on_tool_rotate(GtkWidget *widget, gpointer data);
static void on_tool_scale(GtkWidget *widget, gpointer data);

static void on_view_top(GtkWidget *widget, gpointer data);
static void on_view_front(GtkWidget *widget, gpointer data);
static void on_view_side(GtkWidget *widget, gpointer data);
static void on_view_perspective(GtkWidget *widget, gpointer data);

static void on_tree_selection_changed(GtkTreeSelection *selection, gpointer data);
static void on_transform_changed(GtkWidget *widget, gpointer data);
static void on_material_changed(GtkWidget *widget, gpointer data);

static gboolean on_viewport_draw(GtkWidget *widget, cairo_t *cr, gpointer data);
static gboolean on_viewport_button_press(GtkWidget *widget, GdkEventButton *event, gpointer data);
static gboolean on_viewport_button_release(GtkWidget *widget, GdkEventButton *event, gpointer data);
static gboolean on_viewport_motion_notify(GtkWidget *widget, GdkEventMotion *event, gpointer data);
static gboolean on_viewport_scroll(GtkWidget *widget, GdkEventScroll *event, gpointer data);

// Menu structure - split into smaller parts to avoid string length warning
static const char *menu_xml_start = 
"<interface>"
"  <menu id='menubar'>"
"    <submenu>"
"      <attribute name='label'>File</attribute>"
"      <item>"
"        <attribute name='label'>New</attribute>"
"        <attribute name='action'>app.new</attribute>"
"        <attribute name='accel'>&lt;Ctrl&gt;n</attribute>"
"      </item>"
"      <item>"
"        <attribute name='label'>Open...</attribute>"
"        <attribute name='action'>app.open</attribute>"
"        <attribute name='accel'>&lt;Ctrl&gt;o</attribute>"
"      </item>"
"      <item>"
"        <attribute name='label'>Save</attribute>"
"        <attribute name='action'>app.save</attribute>"
"        <attribute name='accel'>&lt;Ctrl&gt;s</attribute>"
"      </item>"
"      <item>"
"        <attribute name='label'>Save As...</attribute>"
"        <attribute name='action'>app.save_as</attribute>"
"        <attribute name='accel'>&lt;Ctrl&gt;&lt;Shift&gt;s</attribute>"
"      </item>"
"      <section>"
"        <item>"
"          <attribute name='label'>Import IGES...</attribute>"
"          <attribute name='action'>app.import_iges</attribute>"
"        </item>"
"        <item>"
"          <attribute name='label'>Export IGES...</attribute>"
"          <attribute name='action'>app.export_iges</attribute>"
"        </item>"
"        <item>"
"          <attribute name='label'>Export for Game...</attribute>"
"          <attribute name='action'>app.export_game</attribute>"
"        </item>"
"      </section>"
"      <item>"
"        <attribute name='label'>Quit</attribute>"
"        <attribute name='action'>app.quit</attribute>"
"        <attribute name='accel'>&lt;Ctrl&gt;q</attribute>"
"      </item>"
"    </submenu>";

static const char *menu_xml_middle = 
"    <submenu>"
"      <attribute name='label'>Edit</attribute>"
"      <item>"
"        <attribute name='label'>Undo</attribute>"
"        <attribute name='action'>app.undo</attribute>"
"        <attribute name='accel'>&lt;Ctrl&gt;z</attribute>"
"      </item>"
"      <item>"
"        <attribute name='label'>Redo</attribute>"
"        <attribute name='action'>app.redo</attribute>"
"        <attribute name='accel'>&lt;Ctrl&gt;&lt;Shift&gt;z</attribute>"
"      </item>"
"      <section>"
"        <item>"
"          <attribute name='label'>Delete Selected</attribute>"
"          <attribute name='action'>app.delete</attribute>"
"          <attribute name='accel'>Delete</attribute>"
"        </item>"
"        <item>"
"          <attribute name='label'>Duplicate Selected</attribute>"
"          <attribute name='action'>app.duplicate</attribute>"
"          <attribute name='accel'>&lt;Ctrl&gt;d</attribute>"
"        </item>"
"      </section>"
"    </submenu>"
"    <submenu>"
"      <attribute name='label'>Insert</attribute>"
"      <item>"
"        <attribute name='label'>NURBS Sphere</attribute>"
"        <attribute name='action'>app.insert_sphere</attribute>"
"      </item>"
"      <item>"
"        <attribute name='label'>NURBS Plane</attribute>"
"        <attribute name='action'>app.insert_plane</attribute>"
"      </item>"
"      <item>"
"        <attribute name='label'>NURBS Cylinder</attribute>"
"        <attribute name='action'>app.insert_cylinder</attribute>"
"      </item>"
"      <item>"
"        <attribute name='label'>NURBS Torus</attribute>"
"        <attribute name='action'>app.insert_torus</attribute>"
"      </item>";

static const char *menu_xml_end = 
"      <section>"
"        <item>"
"          <attribute name='label'>Point Light</attribute>"
"          <attribute name='action'>app.insert_point_light</attribute>"
"        </item>"
"        <item>"
"          <attribute name='label'>Directional Light</attribute>"
"          <attribute name='action'>app.insert_directional_light</attribute>"
"        </item>"
"        <item>"
"          <attribute name='label'>Spot Light</attribute>"
"          <attribute name='action'>app.insert_spot_light</attribute>"
"        </item>"
"      </section>"
"    </submenu>"
"    <submenu>"
"      <attribute name='label'>View</attribute>"
"      <item>"
"        <attribute name='label'>Reset Camera</attribute>"
"        <attribute name='action'>app.reset_camera</attribute>"
"      </item>"
"      <item>"
"        <attribute name='label'>Focus Selected</attribute>"
"        <attribute name='action'>app.focus_selected</attribute>"
"        <attribute name='accel'>f</attribute>"
"      </item>"
"      <section>"
"        <item>"
"          <attribute name='label'>Wireframe</attribute>"
"          <attribute name='action'>app.toggle_wireframe</attribute>"
"        </item>"
"        <item>"
"          <attribute name='label'>Show Grid</attribute>"
"          <attribute name='action'>app.toggle_grid</attribute>"
"        </item>"
"        <item>"
"          <attribute name='label'>Show Lighting</attribute>"
"          <attribute name='action'>app.toggle_lighting</attribute>"
"        </item>"
"      </section>"
"    </submenu>"
"  </menu>"
"</interface>";

MapEditor* editor_create(GtkApplication *app) {
    MapEditor *editor = calloc(1, sizeof(MapEditor));
    if (!editor) {
        return NULL;
    }

    editor->app = app;
    
    // Initialize camera
    editor->camera.position = (Vector3){0, 5, 10};
    editor->camera.target = (Vector3){0, 0, 0};
    editor->camera.up = (Vector3){0, 1, 0};
    editor->camera.fov = 45.0f;
    editor->camera.near_plane = 0.1f;
    editor->camera.far_plane = 1000.0f;
    editor->camera.view_mode = VIEW_PERSPECTIVE;
    
    // Initialize editor state
    editor->current_tool = TOOL_SELECT;
    editor->show_grid = true;
    editor->show_wireframe = false;
    editor->show_lighting = true;
    editor->scene_modified = false;
    editor->undo_depth = 50;
    
    // Create main window
    editor->main_window = gtk_application_window_new(app);
    gtk_window_set_title(GTK_WINDOW(editor->main_window), "NURBS Map Editor");
    gtk_window_set_default_size(GTK_WINDOW(editor->main_window), 1400, 900);
    
    // Create menu bar - concatenate menu XML parts
    size_t total_len = strlen(menu_xml_start) + strlen(menu_xml_middle) + strlen(menu_xml_end) + 1;
    char *menu_xml = malloc(total_len);
    snprintf(menu_xml, total_len, "%s%s%s", menu_xml_start, menu_xml_middle, menu_xml_end);
    
    GtkBuilder *builder = gtk_builder_new_from_string(menu_xml, -1);
    GMenuModel *menu_model = G_MENU_MODEL(gtk_builder_get_object(builder, "menubar"));
    editor->menubar = gtk_menu_bar_new_from_model(menu_model);
    g_object_unref(builder);
    free(menu_xml);
    
    // Set up actions
    editor_setup_actions(editor);
    
    // Create main layout
    editor_create_layout(editor);
    
    // Create toolbar
    editor_create_toolbar(editor);
    
    // Create scene hierarchy
    editor_create_scene_hierarchy(editor);
    
    // Create properties panel
    editor_create_properties_panel(editor);
    
    // Create viewport
    editor_create_viewport(editor);
    
    // Initialize with empty scene
    editor_new_scene(editor);
    
    return editor;
}

void editor_destroy(MapEditor *editor) {
    if (!editor) return;
    
    // Free scene objects
    EditorObject *obj = editor->objects;
    while (obj) {
        EditorObject *next = obj->next;
        if (obj->nurbs_surface) {
            free_nurbs_surface(obj->nurbs_surface);
        }
        if (obj->tessellated_surface) {
            free_tessellated_surface(obj->tessellated_surface);
        }
        free(obj);
        obj = next;
    }
    
    // Free lights
    EditorLight *light = editor->lights;
    while (light) {
        EditorLight *next = light->next;
        free(light);
        light = next;
    }
    
    // Free undo/redo stacks
    UndoAction *action = editor->undo_stack;
    while (action) {
        UndoAction *next = action->next;
        free(action->data);
        free(action);
        action = next;
    }
    
    action = editor->redo_stack;
    while (action) {
        UndoAction *next = action->next;
        free(action->data);
        free(action);
        action = next;
    }
    
    free(editor);
}

void editor_show(MapEditor *editor) {
    if (!editor) return;
    
    gtk_widget_show_all(editor->main_window);
}

// Layout creation functions
void editor_create_layout(MapEditor *editor) {
    GtkWidget *main_vbox = gtk_box_new(GTK_ORIENTATION_VERTICAL, 0);
    gtk_container_add(GTK_CONTAINER(editor->main_window), main_vbox);
    
    // Add menu bar
    gtk_box_pack_start(GTK_BOX(main_vbox), editor->menubar, FALSE, FALSE, 0);
    
    // Create main paned window
    editor->main_paned = gtk_paned_new(GTK_ORIENTATION_HORIZONTAL);
    gtk_box_pack_start(GTK_BOX(main_vbox), editor->main_paned, TRUE, TRUE, 0);
    
    // Left panel for scene hierarchy
    editor->left_panel = gtk_box_new(GTK_ORIENTATION_VERTICAL, 5);
    gtk_widget_set_size_request(editor->left_panel, 300, -1);
    gtk_paned_pack1(GTK_PANED(editor->main_paned), editor->left_panel, FALSE, FALSE);
    
    // Center panel for viewport and toolbar
    editor->center_panel = gtk_box_new(GTK_ORIENTATION_VERTICAL, 0);
    gtk_paned_pack2(GTK_PANED(editor->main_paned), editor->center_panel, TRUE, FALSE);
    
    // Right panel for properties (will be added as a second paned window)
    GtkWidget *right_paned = gtk_paned_new(GTK_ORIENTATION_HORIZONTAL);
    gtk_paned_pack2(GTK_PANED(editor->main_paned), right_paned, TRUE, FALSE);
    gtk_paned_pack1(GTK_PANED(right_paned), editor->center_panel, TRUE, FALSE);
    
    editor->right_panel = gtk_box_new(GTK_ORIENTATION_VERTICAL, 5);
    gtk_widget_set_size_request(editor->right_panel, 300, -1);
    gtk_paned_pack2(GTK_PANED(right_paned), editor->right_panel, FALSE, FALSE);
}

void editor_create_toolbar(MapEditor *editor) {
    editor->toolbar = gtk_toolbar_new();
    gtk_toolbar_set_style(GTK_TOOLBAR(editor->toolbar), GTK_TOOLBAR_BOTH);
    gtk_box_pack_start(GTK_BOX(editor->center_panel), editor->toolbar, FALSE, FALSE, 0);
    
    // Tool buttons
    const char *tool_names[] = {"Select", "Move", "Rotate", "Scale"};
    const char *tool_icons[] = {"edit-select", "transform-move", "object-rotate-right", "transform-scale"};
    void (*tool_callbacks[])(GtkWidget*, gpointer) = {
        on_tool_select, on_tool_move, on_tool_rotate, on_tool_scale
    };
    
    for (int i = 0; i < 4; i++) {
        editor->tool_buttons[i] = GTK_WIDGET(gtk_tool_button_new(
            gtk_image_new_from_icon_name(tool_icons[i], GTK_ICON_SIZE_LARGE_TOOLBAR),
            tool_names[i]
        ));
        gtk_toolbar_insert(GTK_TOOLBAR(editor->toolbar), GTK_TOOL_ITEM(editor->tool_buttons[i]), -1);
        g_signal_connect(editor->tool_buttons[i], "clicked", G_CALLBACK(tool_callbacks[i]), editor);
    }
    
    // Separator
    GtkToolItem *separator = gtk_separator_tool_item_new();
    gtk_toolbar_insert(GTK_TOOLBAR(editor->toolbar), separator, -1);
    
    // View buttons
    const char *view_names[] = {"Top View", "Front View", "Side View", "Perspective"};
    void (*view_callbacks[])(GtkWidget*, gpointer) = {
        on_view_top, on_view_front, on_view_side, on_view_perspective
    };
    
    for (int i = 0; i < 4; i++) {
        GtkWidget *btn = GTK_WIDGET(gtk_tool_button_new(NULL, view_names[i]));
        gtk_toolbar_insert(GTK_TOOLBAR(editor->toolbar), GTK_TOOL_ITEM(btn), -1);
        g_signal_connect(btn, "clicked", G_CALLBACK(view_callbacks[i]), editor);
    }
    
    // Update tool button states
    editor_update_tool_buttons(editor);
}

void editor_update_tool_buttons(MapEditor *editor) {
    for (int i = 0; i < 4; i++) {
        if (i == (int)editor->current_tool) {
            gtk_widget_set_state_flags(editor->tool_buttons[i], GTK_STATE_FLAG_ACTIVE, FALSE);
        } else {
            gtk_widget_unset_state_flags(editor->tool_buttons[i], GTK_STATE_FLAG_ACTIVE);
        }
    }
}

// Action callback wrappers (GAction callbacks have different signature)
static void action_new_scene(GSimpleAction *action, GVariant *parameter, gpointer data) {
    (void)action; (void)parameter; // Suppress unused parameter warnings
    on_new_scene(NULL, data);
}

#define UNUSED_ACTION_PARAMS (void)action; (void)parameter

static void action_open_scene(GSimpleAction *action, GVariant *parameter, gpointer data) {
    UNUSED_ACTION_PARAMS;
    on_open_scene(NULL, data);
}

static void action_save_scene(GSimpleAction *action, GVariant *parameter, gpointer data) {
    UNUSED_ACTION_PARAMS;
    on_save_scene(NULL, data);
}

static void action_save_scene_as(GSimpleAction *action, GVariant *parameter, gpointer data) {
    UNUSED_ACTION_PARAMS;
    on_save_scene_as(NULL, data);
}

static void action_import_iges(GSimpleAction *action, GVariant *parameter, gpointer data) {
    UNUSED_ACTION_PARAMS;
    on_import_iges(NULL, data);
}

static void action_export_iges(GSimpleAction *action, GVariant *parameter, gpointer data) {
    UNUSED_ACTION_PARAMS;
    on_export_iges(NULL, data);
}

static void action_export_for_game(GSimpleAction *action, GVariant *parameter, gpointer data) {
    UNUSED_ACTION_PARAMS;
    on_export_for_game(NULL, data);
}

static void action_quit(GSimpleAction *action, GVariant *parameter, gpointer data) {
    UNUSED_ACTION_PARAMS;
    on_quit(NULL, data);
}

static void action_undo(GSimpleAction *action, GVariant *parameter, gpointer data) {
    UNUSED_ACTION_PARAMS;
    on_undo(NULL, data);
}

static void action_redo(GSimpleAction *action, GVariant *parameter, gpointer data) {
    UNUSED_ACTION_PARAMS;
    on_redo(NULL, data);
}

static void action_delete_selected(GSimpleAction *action, GVariant *parameter, gpointer data) {
    UNUSED_ACTION_PARAMS;
    on_delete_selected(NULL, data);
}

static void action_duplicate_selected(GSimpleAction *action, GVariant *parameter, gpointer data) {
    UNUSED_ACTION_PARAMS;
    on_duplicate_selected(NULL, data);
}

static void action_insert_sphere(GSimpleAction *action, GVariant *parameter, gpointer data) {
    UNUSED_ACTION_PARAMS;
    on_insert_sphere(NULL, data);
}

static void action_insert_plane(GSimpleAction *action, GVariant *parameter, gpointer data) {
    UNUSED_ACTION_PARAMS;
    on_insert_plane(NULL, data);
}

static void action_insert_cylinder(GSimpleAction *action, GVariant *parameter, gpointer data) {
    UNUSED_ACTION_PARAMS;
    on_insert_cylinder(NULL, data);
}

static void action_insert_torus(GSimpleAction *action, GVariant *parameter, gpointer data) {
    UNUSED_ACTION_PARAMS;
    on_insert_torus(NULL, data);
}

static void action_insert_point_light(GSimpleAction *action, GVariant *parameter, gpointer data) {
    UNUSED_ACTION_PARAMS;
    on_insert_point_light(NULL, data);
}

static void action_insert_directional_light(GSimpleAction *action, GVariant *parameter, gpointer data) {
    UNUSED_ACTION_PARAMS;
    on_insert_directional_light(NULL, data);
}

static void action_insert_spot_light(GSimpleAction *action, GVariant *parameter, gpointer data) {
    UNUSED_ACTION_PARAMS;
    on_insert_spot_light(NULL, data);
}

static void action_reset_camera(GSimpleAction *action, GVariant *parameter, gpointer data) {
    UNUSED_ACTION_PARAMS;
    on_reset_camera(NULL, data);
}

static void action_focus_selected(GSimpleAction *action, GVariant *parameter, gpointer data) {
    UNUSED_ACTION_PARAMS;
    on_focus_selected(NULL, data);
}

static void action_toggle_wireframe(GSimpleAction *action, GVariant *parameter, gpointer data) {
    UNUSED_ACTION_PARAMS;
    on_toggle_wireframe(NULL, data);
}

static void action_toggle_grid(GSimpleAction *action, GVariant *parameter, gpointer data) {
    UNUSED_ACTION_PARAMS;
    on_toggle_grid(NULL, data);
}

static void action_toggle_lighting(GSimpleAction *action, GVariant *parameter, gpointer data) {
    UNUSED_ACTION_PARAMS;
    on_toggle_lighting(NULL, data);
}

// Setup actions for menu items
void editor_setup_actions(MapEditor *editor) {
    // Create action group
    GSimpleActionGroup *action_group = g_simple_action_group_new();
    
    // File actions
    const GActionEntry action_entries[] = {
        {"new", action_new_scene, NULL, NULL, NULL, {0}},
        {"open", action_open_scene, NULL, NULL, NULL, {0}},
        {"save", action_save_scene, NULL, NULL, NULL, {0}},
        {"save_as", action_save_scene_as, NULL, NULL, NULL, {0}},
        {"import_iges", action_import_iges, NULL, NULL, NULL, {0}},
        {"export_iges", action_export_iges, NULL, NULL, NULL, {0}},
        {"export_game", action_export_for_game, NULL, NULL, NULL, {0}},
        {"quit", action_quit, NULL, NULL, NULL, {0}},
        
        // Edit actions
        {"undo", action_undo, NULL, NULL, NULL, {0}},
        {"redo", action_redo, NULL, NULL, NULL, {0}},
        {"delete", action_delete_selected, NULL, NULL, NULL, {0}},
        {"duplicate", action_duplicate_selected, NULL, NULL, NULL, {0}},
        
        // Insert actions
        {"insert_sphere", action_insert_sphere, NULL, NULL, NULL, {0}},
        {"insert_plane", action_insert_plane, NULL, NULL, NULL, {0}},
        {"insert_cylinder", action_insert_cylinder, NULL, NULL, NULL, {0}},
        {"insert_torus", action_insert_torus, NULL, NULL, NULL, {0}},
        {"insert_point_light", action_insert_point_light, NULL, NULL, NULL, {0}},
        {"insert_directional_light", action_insert_directional_light, NULL, NULL, NULL, {0}},
        {"insert_spot_light", action_insert_spot_light, NULL, NULL, NULL, {0}},
        
        // View actions
        {"reset_camera", action_reset_camera, NULL, NULL, NULL, {0}},
        {"focus_selected", action_focus_selected, NULL, NULL, NULL, {0}},
        {"toggle_wireframe", action_toggle_wireframe, NULL, NULL, NULL, {0}},
        {"toggle_grid", action_toggle_grid, NULL, NULL, NULL, {0}},
        {"toggle_lighting", action_toggle_lighting, NULL, NULL, NULL, {0}},
    };
    
    g_action_map_add_action_entries(G_ACTION_MAP(action_group), action_entries, 
                                   G_N_ELEMENTS(action_entries), editor);
    
    gtk_widget_insert_action_group(editor->main_window, "app", G_ACTION_GROUP(action_group));
}

// Create scene hierarchy panel
void editor_create_scene_hierarchy(MapEditor *editor) {
    GtkWidget *frame = gtk_frame_new("Scene Hierarchy");
    gtk_box_pack_start(GTK_BOX(editor->left_panel), frame, TRUE, TRUE, 0);
    
    // Create scrolled window
    GtkWidget *scrolled = gtk_scrolled_window_new(NULL, NULL);
    gtk_scrolled_window_set_policy(GTK_SCROLLED_WINDOW(scrolled), 
                                   GTK_POLICY_AUTOMATIC, GTK_POLICY_AUTOMATIC);
    gtk_container_add(GTK_CONTAINER(frame), scrolled);
    
    // Create tree store and view
    editor->tree_store = gtk_tree_store_new(3, G_TYPE_STRING, G_TYPE_STRING, G_TYPE_POINTER);
    editor->scene_tree = gtk_tree_view_new_with_model(GTK_TREE_MODEL(editor->tree_store));
    
    // Add columns
    GtkCellRenderer *renderer = gtk_cell_renderer_text_new();
    GtkTreeViewColumn *column = gtk_tree_view_column_new_with_attributes(
        "Name", renderer, "text", 0, NULL);
    gtk_tree_view_append_column(GTK_TREE_VIEW(editor->scene_tree), column);
    
    column = gtk_tree_view_column_new_with_attributes(
        "Type", renderer, "text", 1, NULL);
    gtk_tree_view_append_column(GTK_TREE_VIEW(editor->scene_tree), column);
    
    gtk_container_add(GTK_CONTAINER(scrolled), editor->scene_tree);
    
    // Connect selection changed signal
    GtkTreeSelection *selection = gtk_tree_view_get_selection(GTK_TREE_VIEW(editor->scene_tree));
    g_signal_connect(selection, "changed", G_CALLBACK(on_tree_selection_changed), editor);
}

// Create properties panel
void editor_create_properties_panel(MapEditor *editor) {
    GtkWidget *frame = gtk_frame_new("Properties");
    gtk_box_pack_start(GTK_BOX(editor->right_panel), frame, TRUE, TRUE, 0);
    
    // Create notebook for different property pages
    editor->properties_notebook = gtk_notebook_new();
    gtk_container_add(GTK_CONTAINER(frame), editor->properties_notebook);
    
    // Transform page
    editor->transform_page = gtk_box_new(GTK_ORIENTATION_VERTICAL, 5);
    gtk_container_set_border_width(GTK_CONTAINER(editor->transform_page), 10);
    gtk_notebook_append_page(GTK_NOTEBOOK(editor->properties_notebook), 
                           editor->transform_page, gtk_label_new("Transform"));
    
    // Position controls
    GtkWidget *pos_frame = gtk_frame_new("Position");
    gtk_box_pack_start(GTK_BOX(editor->transform_page), pos_frame, FALSE, FALSE, 0);
    GtkWidget *pos_grid = gtk_grid_new();
    gtk_container_add(GTK_CONTAINER(pos_frame), pos_grid);
    
    const char *labels[] = {"X:", "Y:", "Z:"};
    for (int i = 0; i < 3; i++) {
        gtk_grid_attach(GTK_GRID(pos_grid), gtk_label_new(labels[i]), 0, i, 1, 1);
        editor->pos_entries[i] = gtk_entry_new();
        gtk_entry_set_text(GTK_ENTRY(editor->pos_entries[i]), "0.0");
        gtk_grid_attach(GTK_GRID(pos_grid), editor->pos_entries[i], 1, i, 1, 1);
        g_signal_connect(editor->pos_entries[i], "changed", G_CALLBACK(on_transform_changed), editor);
    }
    
    // Rotation controls
    GtkWidget *rot_frame = gtk_frame_new("Rotation");
    gtk_box_pack_start(GTK_BOX(editor->transform_page), rot_frame, FALSE, FALSE, 0);
    GtkWidget *rot_grid = gtk_grid_new();
    gtk_container_add(GTK_CONTAINER(rot_frame), rot_grid);
    
    for (int i = 0; i < 3; i++) {
        gtk_grid_attach(GTK_GRID(rot_grid), gtk_label_new(labels[i]), 0, i, 1, 1);
        editor->rot_entries[i] = gtk_entry_new();
        gtk_entry_set_text(GTK_ENTRY(editor->rot_entries[i]), "0.0");
        gtk_grid_attach(GTK_GRID(rot_grid), editor->rot_entries[i], 1, i, 1, 1);
        g_signal_connect(editor->rot_entries[i], "changed", G_CALLBACK(on_transform_changed), editor);
    }
    
    // Scale controls
    GtkWidget *scale_frame = gtk_frame_new("Scale");
    gtk_box_pack_start(GTK_BOX(editor->transform_page), scale_frame, FALSE, FALSE, 0);
    GtkWidget *scale_grid = gtk_grid_new();
    gtk_container_add(GTK_CONTAINER(scale_frame), scale_grid);
    
    for (int i = 0; i < 3; i++) {
        gtk_grid_attach(GTK_GRID(scale_grid), gtk_label_new(labels[i]), 0, i, 1, 1);
        editor->scale_entries[i] = gtk_entry_new();
        gtk_entry_set_text(GTK_ENTRY(editor->scale_entries[i]), "1.0");
        gtk_grid_attach(GTK_GRID(scale_grid), editor->scale_entries[i], 1, i, 1, 1);
        g_signal_connect(editor->scale_entries[i], "changed", G_CALLBACK(on_transform_changed), editor);
    }
    
    // Material page
    editor->material_page = gtk_box_new(GTK_ORIENTATION_VERTICAL, 5);
    gtk_container_set_border_width(GTK_CONTAINER(editor->material_page), 10);
    gtk_notebook_append_page(GTK_NOTEBOOK(editor->properties_notebook), 
                           editor->material_page, gtk_label_new("Material"));
    
    // Material controls
    GtkWidget *material_grid = gtk_grid_new();
    gtk_box_pack_start(GTK_BOX(editor->material_page), material_grid, FALSE, FALSE, 0);
    
    gtk_grid_attach(GTK_GRID(material_grid), gtk_label_new("Ambient:"), 0, 0, 1, 1);
    editor->ambient_color_btn = gtk_color_button_new();
    gtk_grid_attach(GTK_GRID(material_grid), editor->ambient_color_btn, 1, 0, 1, 1);
    g_signal_connect(editor->ambient_color_btn, "color-set", G_CALLBACK(on_material_changed), editor);
    
    gtk_grid_attach(GTK_GRID(material_grid), gtk_label_new("Diffuse:"), 0, 1, 1, 1);
    editor->diffuse_color_btn = gtk_color_button_new();
    gtk_grid_attach(GTK_GRID(material_grid), editor->diffuse_color_btn, 1, 1, 1, 1);
    g_signal_connect(editor->diffuse_color_btn, "color-set", G_CALLBACK(on_material_changed), editor);
    
    gtk_grid_attach(GTK_GRID(material_grid), gtk_label_new("Specular:"), 0, 2, 1, 1);
    editor->specular_color_btn = gtk_color_button_new();
    gtk_grid_attach(GTK_GRID(material_grid), editor->specular_color_btn, 1, 2, 1, 1);
    g_signal_connect(editor->specular_color_btn, "color-set", G_CALLBACK(on_material_changed), editor);
    
    gtk_grid_attach(GTK_GRID(material_grid), gtk_label_new("Shininess:"), 0, 3, 1, 1);
    editor->shininess_entry = gtk_entry_new();
    gtk_entry_set_text(GTK_ENTRY(editor->shininess_entry), "32.0");
    gtk_grid_attach(GTK_GRID(material_grid), editor->shininess_entry, 1, 3, 1, 1);
    g_signal_connect(editor->shininess_entry, "changed", G_CALLBACK(on_material_changed), editor);
}

// Create 3D viewport
void editor_create_viewport(MapEditor *editor) {
    GtkWidget *frame = gtk_frame_new("3D Viewport");
    gtk_box_pack_start(GTK_BOX(editor->center_panel), frame, TRUE, TRUE, 0);
    
    // Create drawing area for OpenGL rendering
    editor->viewport_area = gtk_drawing_area_new();
    gtk_widget_set_size_request(editor->viewport_area, 800, 600);
    gtk_container_add(GTK_CONTAINER(frame), editor->viewport_area);
    
    // Set up event masks
    gtk_widget_set_events(editor->viewport_area, 
                         GDK_EXPOSURE_MASK | GDK_BUTTON_PRESS_MASK | 
                         GDK_BUTTON_RELEASE_MASK | GDK_POINTER_MOTION_MASK |
                         GDK_SCROLL_MASK);
    
    // Connect signals
    g_signal_connect(editor->viewport_area, "draw", G_CALLBACK(on_viewport_draw), editor);
    g_signal_connect(editor->viewport_area, "button-press-event", G_CALLBACK(on_viewport_button_press), editor);
    g_signal_connect(editor->viewport_area, "button-release-event", G_CALLBACK(on_viewport_button_release), editor);
    g_signal_connect(editor->viewport_area, "motion-notify-event", G_CALLBACK(on_viewport_motion_notify), editor);
    g_signal_connect(editor->viewport_area, "scroll-event", G_CALLBACK(on_viewport_scroll), editor);
}

// Stub implementations for callback functions
// File menu callbacks
static void on_new_scene(GtkWidget *widget, gpointer data) {
    MapEditor *editor = (MapEditor*)data;
    // TODO: Implement new scene functionality
    printf("New scene requested\n");
}

static void on_open_scene(GtkWidget *widget, gpointer data) {
    MapEditor *editor = (MapEditor*)data;
    // TODO: Implement open scene functionality
    printf("Open scene requested\n");
}

static void on_save_scene(GtkWidget *widget, gpointer data) {
    MapEditor *editor = (MapEditor*)data;
    // TODO: Implement save scene functionality
    printf("Save scene requested\n");
}

static void on_save_scene_as(GtkWidget *widget, gpointer data) {
    MapEditor *editor = (MapEditor*)data;
    // TODO: Implement save scene as functionality
    printf("Save scene as requested\n");
}

static void on_export_for_game(GtkWidget *widget, gpointer data) {
    MapEditor *editor = (MapEditor*)data;
    // TODO: Implement export for game functionality
    printf("Export for game requested\n");
}

static void on_import_iges(GtkWidget *widget, gpointer data) {
    MapEditor *editor = (MapEditor*)data;
    // TODO: Implement IGES import functionality
    printf("Import IGES requested\n");
}

static void on_export_iges(GtkWidget *widget, gpointer data) {
    MapEditor *editor = (MapEditor*)data;
    // TODO: Implement IGES export functionality
    printf("Export IGES requested\n");
}

static void on_quit(GtkWidget *widget, gpointer data) {
    MapEditor *editor = (MapEditor*)data;
    // TODO: Implement quit functionality with save check
    gtk_main_quit();
}

// Edit menu callbacks
static void on_undo(GtkWidget *widget, gpointer data) {
    MapEditor *editor = (MapEditor*)data;
    // TODO: Implement undo functionality
    printf("Undo requested\n");
}

static void on_redo(GtkWidget *widget, gpointer data) {
    MapEditor *editor = (MapEditor*)data;
    // TODO: Implement redo functionality
    printf("Redo requested\n");
}

static void on_delete_selected(GtkWidget *widget, gpointer data) {
    MapEditor *editor = (MapEditor*)data;
    // TODO: Implement delete selected functionality
    printf("Delete selected requested\n");
}

static void on_duplicate_selected(GtkWidget *widget, gpointer data) {
    MapEditor *editor = (MapEditor*)data;
    // TODO: Implement duplicate selected functionality
    printf("Duplicate selected requested\n");
}

// Insert menu callbacks
static void on_insert_sphere(GtkWidget *widget, gpointer data) {
    MapEditor *editor = (MapEditor*)data;
    // TODO: Implement insert sphere functionality
    printf("Insert sphere requested\n");
}

static void on_insert_plane(GtkWidget *widget, gpointer data) {
    MapEditor *editor = (MapEditor*)data;
    // TODO: Implement insert plane functionality
    printf("Insert plane requested\n");
}

static void on_insert_cylinder(GtkWidget *widget, gpointer data) {
    MapEditor *editor = (MapEditor*)data;
    // TODO: Implement insert cylinder functionality
    printf("Insert cylinder requested\n");
}

static void on_insert_torus(GtkWidget *widget, gpointer data) {
    MapEditor *editor = (MapEditor*)data;
    // TODO: Implement insert torus functionality
    printf("Insert torus requested\n");
}

static void on_insert_point_light(GtkWidget *widget, gpointer data) {
    MapEditor *editor = (MapEditor*)data;
    // TODO: Implement insert point light functionality
    printf("Insert point light requested\n");
}

static void on_insert_directional_light(GtkWidget *widget, gpointer data) {
    MapEditor *editor = (MapEditor*)data;
    // TODO: Implement insert directional light functionality
    printf("Insert directional light requested\n");
}

static void on_insert_spot_light(GtkWidget *widget, gpointer data) {
    MapEditor *editor = (MapEditor*)data;
    // TODO: Implement insert spot light functionality
    printf("Insert spot light requested\n");
}

// View menu callbacks
static void on_reset_camera(GtkWidget *widget, gpointer data) {
    MapEditor *editor = (MapEditor*)data;
    // TODO: Implement reset camera functionality
    printf("Reset camera requested\n");
}

static void on_focus_selected(GtkWidget *widget, gpointer data) {
    MapEditor *editor = (MapEditor*)data;
    // TODO: Implement focus selected functionality
    printf("Focus selected requested\n");
}

static void on_toggle_wireframe(GtkWidget *widget, gpointer data) {
    MapEditor *editor = (MapEditor*)data;
    // TODO: Implement toggle wireframe functionality
    editor->show_wireframe = !editor->show_wireframe;
    printf("Toggle wireframe: %s\n", editor->show_wireframe ? "ON" : "OFF");
}

static void on_toggle_grid(GtkWidget *widget, gpointer data) {
    MapEditor *editor = (MapEditor*)data;
    // TODO: Implement toggle grid functionality
    editor->show_grid = !editor->show_grid;
    printf("Toggle grid: %s\n", editor->show_grid ? "ON" : "OFF");
}

static void on_toggle_lighting(GtkWidget *widget, gpointer data) {
    MapEditor *editor = (MapEditor*)data;
    // TODO: Implement toggle lighting functionality
    editor->show_lighting = !editor->show_lighting;
    printf("Toggle lighting: %s\n", editor->show_lighting ? "ON" : "OFF");
}

// Tool callbacks
static void on_tool_select(GtkWidget *widget, gpointer data) {
    MapEditor *editor = (MapEditor*)data;
    editor->current_tool = TOOL_SELECT;
    editor_update_tool_buttons(editor);
    printf("Select tool activated\n");
}

static void on_tool_move(GtkWidget *widget, gpointer data) {
    MapEditor *editor = (MapEditor*)data;
    editor->current_tool = TOOL_MOVE;
    editor_update_tool_buttons(editor);
    printf("Move tool activated\n");
}

static void on_tool_rotate(GtkWidget *widget, gpointer data) {
    MapEditor *editor = (MapEditor*)data;
    editor->current_tool = TOOL_ROTATE;
    editor_update_tool_buttons(editor);
    printf("Rotate tool activated\n");
}

static void on_tool_scale(GtkWidget *widget, gpointer data) {
    MapEditor *editor = (MapEditor*)data;
    editor->current_tool = TOOL_SCALE;
    editor_update_tool_buttons(editor);
    printf("Scale tool activated\n");
}

// View callbacks
static void on_view_top(GtkWidget *widget, gpointer data) {
    MapEditor *editor = (MapEditor*)data;
    // TODO: Implement top view functionality
    editor->camera.view_mode = VIEW_TOP;
    printf("Top view activated\n");
}

static void on_view_front(GtkWidget *widget, gpointer data) {
    MapEditor *editor = (MapEditor*)data;
    // TODO: Implement front view functionality
    editor->camera.view_mode = VIEW_FRONT;
    printf("Front view activated\n");
}

static void on_view_side(GtkWidget *widget, gpointer data) {
    MapEditor *editor = (MapEditor*)data;
    // TODO: Implement side view functionality
    editor->camera.view_mode = VIEW_SIDE;
    printf("Side view activated\n");
}

static void on_view_perspective(GtkWidget *widget, gpointer data) {
    MapEditor *editor = (MapEditor*)data;
    // TODO: Implement perspective view functionality
    editor->camera.view_mode = VIEW_PERSPECTIVE;
    printf("Perspective view activated\n");
}

// Properties callbacks
static void on_tree_selection_changed(GtkTreeSelection *selection, gpointer data) {
    MapEditor *editor = (MapEditor*)data;
    // TODO: Implement tree selection changed functionality
    printf("Tree selection changed\n");
}

static void on_transform_changed(GtkWidget *widget, gpointer data) {
    MapEditor *editor = (MapEditor*)data;
    // TODO: Implement transform changed functionality
    printf("Transform changed\n");
}

static void on_material_changed(GtkWidget *widget, gpointer data) {
    MapEditor *editor = (MapEditor*)data;
    // TODO: Implement material changed functionality
    printf("Material changed\n");
}

// Viewport callbacks
static gboolean on_viewport_draw(GtkWidget *widget, cairo_t *cr, gpointer data) {
    MapEditor *editor = (MapEditor*)data;
    // TODO: Implement viewport drawing functionality
    // For now, just clear the viewport with a dark background
    cairo_set_source_rgb(cr, 0.2, 0.2, 0.2);
    cairo_paint(cr);
    return TRUE;
}

static gboolean on_viewport_button_press(GtkWidget *widget, GdkEventButton *event, gpointer data) {
    MapEditor *editor = (MapEditor*)data;
    // TODO: Implement viewport button press functionality
    printf("Viewport button press at (%f, %f)\n", event->x, event->y);
    return TRUE;
}

static gboolean on_viewport_button_release(GtkWidget *widget, GdkEventButton *event, gpointer data) {
    MapEditor *editor = (MapEditor*)data;
    // TODO: Implement viewport button release functionality
    printf("Viewport button release at (%f, %f)\n", event->x, event->y);
    return TRUE;
}

static gboolean on_viewport_motion_notify(GtkWidget *widget, GdkEventMotion *event, gpointer data) {
    MapEditor *editor = (MapEditor*)data;
    // TODO: Implement viewport motion notify functionality
    return TRUE;
}

static gboolean on_viewport_scroll(GtkWidget *widget, GdkEventScroll *event, gpointer data) {
    MapEditor *editor = (MapEditor*)data;
    // TODO: Implement viewport scroll functionality
    printf("Viewport scroll\n");
    return TRUE;
}