#include "hierarchy_panel.h"
#include "ui_builder.h"
#include "window.h"
#include "property_panel.h"

static HierarchyPanel *g_hierarchy_panel = NULL;

static GtkWidget *create_hierarchy_toolbar(void) {
    GtkWidget *toolbar = gtk_toolbar_new();
    gtk_toolbar_set_style(GTK_TOOLBAR(toolbar), GTK_TOOLBAR_ICONS);
    gtk_toolbar_set_icon_size(GTK_TOOLBAR(toolbar), GTK_ICON_SIZE_SMALL_TOOLBAR);
    
    // Add curve button
    g_hierarchy_panel->add_curve_btn = GTK_WIDGET(gtk_tool_button_new(NULL, "Curve"));
    gtk_tool_button_set_icon_name(GTK_TOOL_BUTTON(g_hierarchy_panel->add_curve_btn), "insert-object");
    gtk_widget_set_tooltip_text(g_hierarchy_panel->add_curve_btn, "Add NURBS Curve");
    gtk_toolbar_insert(GTK_TOOLBAR(toolbar), GTK_TOOL_ITEM(g_hierarchy_panel->add_curve_btn), -1);
    
    // Add surface button
    g_hierarchy_panel->add_surface_btn = GTK_WIDGET(gtk_tool_button_new(NULL, "Surface"));
    gtk_tool_button_set_icon_name(GTK_TOOL_BUTTON(g_hierarchy_panel->add_surface_btn), "insert-object");
    gtk_widget_set_tooltip_text(g_hierarchy_panel->add_surface_btn, "Add NURBS Surface");
    gtk_toolbar_insert(GTK_TOOLBAR(toolbar), GTK_TOOL_ITEM(g_hierarchy_panel->add_surface_btn), -1);
    
    // Add group button
    g_hierarchy_panel->add_group_btn = GTK_WIDGET(gtk_tool_button_new(NULL, "Group"));
    gtk_tool_button_set_icon_name(GTK_TOOL_BUTTON(g_hierarchy_panel->add_group_btn), "folder-new");
    gtk_widget_set_tooltip_text(g_hierarchy_panel->add_group_btn, "Add Group");
    gtk_toolbar_insert(GTK_TOOLBAR(toolbar), GTK_TOOL_ITEM(g_hierarchy_panel->add_group_btn), -1);
    
    // Separator
    GtkToolItem *separator = gtk_separator_tool_item_new();
    gtk_toolbar_insert(GTK_TOOLBAR(toolbar), separator, -1);
    
    // Delete button
    g_hierarchy_panel->delete_btn = GTK_WIDGET(gtk_tool_button_new(NULL, "Delete"));
    gtk_tool_button_set_icon_name(GTK_TOOL_BUTTON(g_hierarchy_panel->delete_btn), "edit-delete");
    gtk_widget_set_tooltip_text(g_hierarchy_panel->delete_btn, "Delete Selected");
    gtk_toolbar_insert(GTK_TOOLBAR(toolbar), GTK_TOOL_ITEM(g_hierarchy_panel->delete_btn), -1);
    
    // Connect signals
    g_signal_connect(g_hierarchy_panel->add_curve_btn, "clicked", 
                     G_CALLBACK(on_hierarchy_add_curve_clicked), NULL);
    g_signal_connect(g_hierarchy_panel->add_surface_btn, "clicked", 
                     G_CALLBACK(on_hierarchy_add_surface_clicked), NULL);
    g_signal_connect(g_hierarchy_panel->add_group_btn, "clicked", 
                     G_CALLBACK(on_hierarchy_add_group_clicked), NULL);
    g_signal_connect(g_hierarchy_panel->delete_btn, "clicked", 
                     G_CALLBACK(on_hierarchy_delete_clicked), NULL);
    
    return toolbar;
}

GtkWidget *create_hierarchy_panel(void) {
    g_hierarchy_panel = g_malloc0(sizeof(HierarchyPanel));
    
    // Main container
    g_hierarchy_panel->container = gtk_box_new(GTK_ORIENTATION_VERTICAL, 0);
    gtk_widget_set_size_request(g_hierarchy_panel->container, 250, -1);
    gtk_style_context_add_class(gtk_widget_get_style_context(g_hierarchy_panel->container), 
                               "hierarchy-panel");
    
    // Header
    GtkWidget *header = gtk_label_new("Hierarchy");
    gtk_style_context_add_class(gtk_widget_get_style_context(header), "hierarchy-header");
    gtk_box_pack_start(GTK_BOX(g_hierarchy_panel->container), header, FALSE, FALSE, 0);
    
    // Toolbar
    g_hierarchy_panel->toolbar = create_hierarchy_toolbar();
    gtk_box_pack_start(GTK_BOX(g_hierarchy_panel->container), 
                      g_hierarchy_panel->toolbar, FALSE, FALSE, 0);
    
    // Tree store
    g_hierarchy_panel->tree_store = gtk_tree_store_new(HIERARCHY_N_COLUMNS,
                                                      G_TYPE_STRING,    // Name
                                                      G_TYPE_POINTER,   // Object
                                                      G_TYPE_STRING,    // Icon
                                                      G_TYPE_BOOLEAN);  // Visible
    
    // Tree view
    g_hierarchy_panel->tree_view = gtk_tree_view_new_with_model(
        GTK_TREE_MODEL(g_hierarchy_panel->tree_store));
    
    // Configure tree view
    gtk_tree_view_set_headers_visible(GTK_TREE_VIEW(g_hierarchy_panel->tree_view), FALSE);
    gtk_tree_view_set_show_expanders(GTK_TREE_VIEW(g_hierarchy_panel->tree_view), TRUE);
    gtk_tree_view_set_level_indentation(GTK_TREE_VIEW(g_hierarchy_panel->tree_view), 16);
    
    // Add columns
    // Icon column
    GtkCellRenderer *icon_renderer = gtk_cell_renderer_pixbuf_new();
    GtkTreeViewColumn *icon_column = gtk_tree_view_column_new_with_attributes(
        "", icon_renderer, "icon-name", HIERARCHY_COL_ICON, NULL);
    gtk_tree_view_column_set_sizing(icon_column, GTK_TREE_VIEW_COLUMN_FIXED);
    gtk_tree_view_column_set_fixed_width(icon_column, 24);
    gtk_tree_view_append_column(GTK_TREE_VIEW(g_hierarchy_panel->tree_view), icon_column);
    
    // Name column
    GtkCellRenderer *text_renderer = gtk_cell_renderer_text_new();
    g_object_set(text_renderer, "editable", TRUE, NULL);
    GtkTreeViewColumn *name_column = gtk_tree_view_column_new_with_attributes(
        "Name", text_renderer, "text", HIERARCHY_COL_NAME, NULL);
    gtk_tree_view_append_column(GTK_TREE_VIEW(g_hierarchy_panel->tree_view), name_column);
    
    // Visible column
    GtkCellRenderer *toggle_renderer = gtk_cell_renderer_toggle_new();
    GtkTreeViewColumn *visible_column = gtk_tree_view_column_new_with_attributes(
        "V", toggle_renderer, "active", HIERARCHY_COL_VISIBLE, NULL);
    gtk_tree_view_column_set_sizing(visible_column, GTK_TREE_VIEW_COLUMN_FIXED);
    gtk_tree_view_column_set_fixed_width(visible_column, 24);
    gtk_tree_view_append_column(GTK_TREE_VIEW(g_hierarchy_panel->tree_view), visible_column);
    
    // Scrolled window for tree view
    GtkWidget *scrolled = gtk_scrolled_window_new(NULL, NULL);
    gtk_scrolled_window_set_policy(GTK_SCROLLED_WINDOW(scrolled), 
                                  GTK_POLICY_AUTOMATIC, GTK_POLICY_AUTOMATIC);
    gtk_container_add(GTK_CONTAINER(scrolled), g_hierarchy_panel->tree_view);
    gtk_box_pack_start(GTK_BOX(g_hierarchy_panel->container), scrolled, TRUE, TRUE, 0);
    
    // Connect signals
    GtkTreeSelection *selection = gtk_tree_view_get_selection(
        GTK_TREE_VIEW(g_hierarchy_panel->tree_view));
    gtk_tree_selection_set_mode(selection, GTK_SELECTION_SINGLE);
    g_signal_connect(selection, "changed", 
                     G_CALLBACK(on_hierarchy_selection_changed), NULL);
    
    g_signal_connect(g_hierarchy_panel->tree_view, "row-activated", 
                     G_CALLBACK(on_hierarchy_row_activated), NULL);
    
    g_signal_connect(g_hierarchy_panel->tree_view, "button-press-event", 
                     G_CALLBACK(on_hierarchy_button_press), NULL);
    
    return g_hierarchy_panel->container;
}

void hierarchy_panel_refresh(void) {
    if (!g_hierarchy_panel) return;
    
    // Clear existing items
    gtk_tree_store_clear(g_hierarchy_panel->tree_store);
    
    if (!g_editor_state || !g_editor_state->objects) return;
    
    // Add all objects
    GList *current = g_editor_state->objects;
    while (current) {
        MapObject *object = (MapObject*)current->data;
        hierarchy_panel_add_object(object);
        current = current->next;
    }
}

void hierarchy_panel_add_object(MapObject *object) {
    if (!g_hierarchy_panel || !object) return;
    
    GtkTreeIter iter;
    gtk_tree_store_append(g_hierarchy_panel->tree_store, &iter, NULL);
    
    const char *icon_name;
    switch (object->type) {
        case OBJECT_CURVE:
            icon_name = "applications-graphics";
            break;
        case OBJECT_SURFACE:
            icon_name = "applications-graphics";
            break;
        case OBJECT_GROUP:
            icon_name = "folder";
            break;
        default:
            icon_name = "text-x-generic";
            break;
    }
    
    gtk_tree_store_set(g_hierarchy_panel->tree_store, &iter,
                      HIERARCHY_COL_NAME, object->name,
                      HIERARCHY_COL_OBJECT, object,
                      HIERARCHY_COL_ICON, icon_name,
                      HIERARCHY_COL_VISIBLE, object->visible,
                      -1);
}

void hierarchy_panel_remove_object(MapObject *object) {
    if (!g_hierarchy_panel || !object) return;
    
    GtkTreeModel *model = GTK_TREE_MODEL(g_hierarchy_panel->tree_store);
    GtkTreeIter iter;
    
    if (gtk_tree_model_get_iter_first(model, &iter)) {
        do {
            MapObject *obj;
            gtk_tree_model_get(model, &iter, HIERARCHY_COL_OBJECT, &obj, -1);
            
            if (obj == object) {
                gtk_tree_store_remove(g_hierarchy_panel->tree_store, &iter);
                break;
            }
        } while (gtk_tree_model_iter_next(model, &iter));
    }
}

void hierarchy_panel_select_object(MapObject *object) {
    if (!g_hierarchy_panel || !object) return;
    
    GtkTreeModel *model = GTK_TREE_MODEL(g_hierarchy_panel->tree_store);
    GtkTreeIter iter;
    
    if (gtk_tree_model_get_iter_first(model, &iter)) {
        do {
            MapObject *obj;
            gtk_tree_model_get(model, &iter, HIERARCHY_COL_OBJECT, &obj, -1);
            
            if (obj == object) {
                GtkTreeSelection *selection = gtk_tree_view_get_selection(
                    GTK_TREE_VIEW(g_hierarchy_panel->tree_view));
                gtk_tree_selection_select_iter(selection, &iter);
                
                GtkTreePath *path = gtk_tree_model_get_path(model, &iter);
                gtk_tree_view_scroll_to_cell(GTK_TREE_VIEW(g_hierarchy_panel->tree_view),
                                           path, NULL, FALSE, 0.0, 0.0);
                gtk_tree_path_free(path);
                break;
            }
        } while (gtk_tree_model_iter_next(model, &iter));
    }
}

// Signal callbacks
void on_hierarchy_selection_changed(GtkTreeSelection *selection, gpointer user_data) {
    GtkTreeModel *model;
    GtkTreeIter iter;
    
    if (gtk_tree_selection_get_selected(selection, &model, &iter)) {
        MapObject *object;
        gtk_tree_model_get(model, &iter, HIERARCHY_COL_OBJECT, &object, -1);
        
        // Update property panel
        property_panel_set_object(object);
        
        // Update selection state
        if (g_editor_state) {
            g_list_free(g_editor_state->selected_objects);
            g_editor_state->selected_objects = g_list_append(NULL, object);
            
            // Clear previous selection
            GList *current = g_editor_state->objects;
            while (current) {
                MapObject *obj = (MapObject*)current->data;
                obj->selected = FALSE;
                current = current->next;
            }
            
            object->selected = TRUE;
        }
    } else {
        property_panel_set_object(NULL);
    }
}

void on_hierarchy_row_activated(GtkTreeView *tree_view, GtkTreePath *path, 
                               GtkTreeViewColumn *column, gpointer user_data) {
    // Double-click behavior - could focus on object in viewport
    printf("Row activated\n");
}

void on_hierarchy_add_curve_clicked(GtkButton *button, gpointer user_data) {
    MapObject *object = map_object_new(OBJECT_CURVE, "NURBS Curve");
    
    // Create a simple NURBS curve
    NurbsCurve *curve = nurbs_curve_new(3, 4);
    
    // Set some default control points
    nurbs_curve_set_control_point(curve, 0, -3.0f, 0.0f, 0.0f, 1.0f);
    nurbs_curve_set_control_point(curve, 1, -1.0f, 2.0f, 0.0f, 1.0f);
    nurbs_curve_set_control_point(curve, 2, 1.0f, 2.0f, 0.0f, 1.0f);
    nurbs_curve_set_control_point(curve, 3, 3.0f, 0.0f, 0.0f, 1.0f);
    
    object->data.curve = curve;
    
    // Add to scene
    if (g_editor_state) {
        g_editor_state->objects = g_list_append(g_editor_state->objects, object);
        hierarchy_panel_add_object(object);
        hierarchy_panel_select_object(object);
    }
}

void on_hierarchy_add_surface_clicked(GtkButton *button, gpointer user_data) {
    MapObject *object = map_object_new(OBJECT_SURFACE, "NURBS Surface");
    
    // Create a simple NURBS surface
    NurbsSurface *surface = nurbs_surface_new(3, 3, 4, 4);
    
    // Set some default control points (simple grid)
    for (int i = 0; i < 4; i++) {
        for (int j = 0; j < 4; j++) {
            float x = (i - 1.5f) * 2.0f;
            float z = (j - 1.5f) * 2.0f;
            float y = sinf(x * 0.5f) * cosf(z * 0.5f) * 0.5f;
            surface->control_points[i][j].x = x;
            surface->control_points[i][j].y = y;
            surface->control_points[i][j].z = z;
            surface->control_points[i][j].w = 1.0f;
        }
    }
    
    object->data.surface = surface;
    
    // Add to scene
    if (g_editor_state) {
        g_editor_state->objects = g_list_append(g_editor_state->objects, object);
        hierarchy_panel_add_object(object);
        hierarchy_panel_select_object(object);
    }
}

void on_hierarchy_add_group_clicked(GtkButton *button, gpointer user_data) {
    MapObject *object = map_object_new(OBJECT_GROUP, "Group");
    
    // Add to scene
    if (g_editor_state) {
        g_editor_state->objects = g_list_append(g_editor_state->objects, object);
        hierarchy_panel_add_object(object);
        hierarchy_panel_select_object(object);
    }
}

void on_hierarchy_delete_clicked(GtkButton *button, gpointer user_data) {
    if (!g_editor_state || !g_editor_state->selected_objects) return;
    
    MapObject *object = (MapObject*)g_editor_state->selected_objects->data;
    
    // Remove from hierarchy panel
    hierarchy_panel_remove_object(object);
    
    // Remove from scene
    g_editor_state->objects = g_list_remove(g_editor_state->objects, object);
    g_list_free(g_editor_state->selected_objects);
    g_editor_state->selected_objects = NULL;
    
    // Free object
    map_object_free(object);
    
    // Clear property panel
    property_panel_set_object(NULL);
}

gboolean on_hierarchy_button_press(GtkWidget *widget, GdkEventButton *event, gpointer user_data) {
    if (event->type == GDK_BUTTON_PRESS && event->button == 3) {
        // Right-click context menu
        // TODO: Implement context menu
        printf("Right-click context menu\n");
        return TRUE;
    }
    
    return FALSE;
}