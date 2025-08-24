/*
 * NURBS Map Editor - Main Editor Implementation
 * A Roblox Studio-like editor for NURBS-based FPS games with full IGES support
 */

#include "editor.h"
#include "iges_loader.h"
#include "nurbs_primitives.h"
#include <string.h>
#include <math.h>

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

// Menu structure
static const char *menu_xml = 
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
"    </submenu>"
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
"      </item>"
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
    
    // Create menu bar
    GtkBuilder *builder = gtk_builder_new_from_string(menu_xml, -1);
    GMenuModel *menu_model = G_MENU_MODEL(gtk_builder_get_object(builder, "menubar"));
    editor->menubar = gtk_menu_bar_new_from_model(menu_model);
    g_object_unref(builder);
    
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
static void editor_create_layout(MapEditor *editor) {
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

static void editor_create_toolbar(MapEditor *editor) {
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

static void editor_update_tool_buttons(MapEditor *editor) {
    for (int i = 0; i < 4; i++) {
        if (i == (int)editor->current_tool) {
            gtk_widget_set_state_flags(editor->tool_buttons[i], GTK_STATE_FLAG_ACTIVE, FALSE);
        } else {
            gtk_widget_unset_state_flags(editor->tool_buttons[i], GTK_STATE_FLAG_ACTIVE);
        }
    }
}

// Continue with more implementation...
// (The file is getting long, so I'll continue in the next part)