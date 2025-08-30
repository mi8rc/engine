#include "property_panel.h"
#include "ui_builder.h"
#include "window.h"

static PropertyPanel *g_property_panel = NULL;

static GtkWidget *create_transform_section(void) {
    GtkWidget *frame = create_dark_frame("Transform");
    GtkWidget *grid = gtk_grid_new();
    gtk_grid_set_row_spacing(GTK_GRID(grid), 4);
    gtk_grid_set_column_spacing(GTK_GRID(grid), 8);
    gtk_container_add(GTK_CONTAINER(frame), grid);
    
    // Position
    GtkWidget *pos_label = gtk_label_new("Position:");
    gtk_label_set_xalign(GTK_LABEL(pos_label), 0.0);
    gtk_grid_attach(GTK_GRID(grid), pos_label, 0, 0, 3, 1);
    
    g_property_panel->pos_x_spin = gtk_spin_button_new_with_range(-1000.0, 1000.0, 0.1);
    g_property_panel->pos_y_spin = gtk_spin_button_new_with_range(-1000.0, 1000.0, 0.1);
    g_property_panel->pos_z_spin = gtk_spin_button_new_with_range(-1000.0, 1000.0, 0.1);
    
    gtk_grid_attach(GTK_GRID(grid), g_property_panel->pos_x_spin, 0, 1, 1, 1);
    gtk_grid_attach(GTK_GRID(grid), g_property_panel->pos_y_spin, 1, 1, 1, 1);
    gtk_grid_attach(GTK_GRID(grid), g_property_panel->pos_z_spin, 2, 1, 1, 1);
    
    // Rotation
    GtkWidget *rot_label = gtk_label_new("Rotation:");
    gtk_label_set_xalign(GTK_LABEL(rot_label), 0.0);
    gtk_grid_attach(GTK_GRID(grid), rot_label, 0, 2, 3, 1);
    
    g_property_panel->rot_x_spin = gtk_spin_button_new_with_range(-360.0, 360.0, 1.0);
    g_property_panel->rot_y_spin = gtk_spin_button_new_with_range(-360.0, 360.0, 1.0);
    g_property_panel->rot_z_spin = gtk_spin_button_new_with_range(-360.0, 360.0, 1.0);
    
    gtk_grid_attach(GTK_GRID(grid), g_property_panel->rot_x_spin, 0, 3, 1, 1);
    gtk_grid_attach(GTK_GRID(grid), g_property_panel->rot_y_spin, 1, 3, 1, 1);
    gtk_grid_attach(GTK_GRID(grid), g_property_panel->rot_z_spin, 2, 3, 1, 1);
    
    // Scale
    GtkWidget *scale_label = gtk_label_new("Scale:");
    gtk_label_set_xalign(GTK_LABEL(scale_label), 0.0);
    gtk_grid_attach(GTK_GRID(grid), scale_label, 0, 4, 3, 1);
    
    g_property_panel->scale_x_spin = gtk_spin_button_new_with_range(0.01, 100.0, 0.1);
    g_property_panel->scale_y_spin = gtk_spin_button_new_with_range(0.01, 100.0, 0.1);
    g_property_panel->scale_z_spin = gtk_spin_button_new_with_range(0.01, 100.0, 0.1);
    
    gtk_spin_button_set_value(GTK_SPIN_BUTTON(g_property_panel->scale_x_spin), 1.0);
    gtk_spin_button_set_value(GTK_SPIN_BUTTON(g_property_panel->scale_y_spin), 1.0);
    gtk_spin_button_set_value(GTK_SPIN_BUTTON(g_property_panel->scale_z_spin), 1.0);
    
    gtk_grid_attach(GTK_GRID(grid), g_property_panel->scale_x_spin, 0, 5, 1, 1);
    gtk_grid_attach(GTK_GRID(grid), g_property_panel->scale_y_spin, 1, 5, 1, 1);
    gtk_grid_attach(GTK_GRID(grid), g_property_panel->scale_z_spin, 2, 5, 1, 1);
    
    // Connect signals
    g_signal_connect(g_property_panel->pos_x_spin, "value-changed", 
                     G_CALLBACK(on_property_transform_changed), NULL);
    g_signal_connect(g_property_panel->pos_y_spin, "value-changed", 
                     G_CALLBACK(on_property_transform_changed), NULL);
    g_signal_connect(g_property_panel->pos_z_spin, "value-changed", 
                     G_CALLBACK(on_property_transform_changed), NULL);
    
    g_signal_connect(g_property_panel->rot_x_spin, "value-changed", 
                     G_CALLBACK(on_property_transform_changed), NULL);
    g_signal_connect(g_property_panel->rot_y_spin, "value-changed", 
                     G_CALLBACK(on_property_transform_changed), NULL);
    g_signal_connect(g_property_panel->rot_z_spin, "value-changed", 
                     G_CALLBACK(on_property_transform_changed), NULL);
    
    g_signal_connect(g_property_panel->scale_x_spin, "value-changed", 
                     G_CALLBACK(on_property_transform_changed), NULL);
    g_signal_connect(g_property_panel->scale_y_spin, "value-changed", 
                     G_CALLBACK(on_property_transform_changed), NULL);
    g_signal_connect(g_property_panel->scale_z_spin, "value-changed", 
                     G_CALLBACK(on_property_transform_changed), NULL);
    
    return frame;
}

static GtkWidget *create_general_section(void) {
    GtkWidget *frame = create_dark_frame("General");
    GtkWidget *vbox = gtk_box_new(GTK_ORIENTATION_VERTICAL, 4);
    gtk_container_add(GTK_CONTAINER(frame), vbox);
    
    // Name
    g_property_panel->name_entry = gtk_entry_new();
    GtkWidget *name_row = create_property_row("Name:", g_property_panel->name_entry);
    gtk_box_pack_start(GTK_BOX(vbox), name_row, FALSE, FALSE, 0);
    
    // Visible
    g_property_panel->visible_check = gtk_check_button_new_with_label("Visible");
    gtk_box_pack_start(GTK_BOX(vbox), g_property_panel->visible_check, FALSE, FALSE, 0);
    
    // Connect signals
    g_signal_connect(g_property_panel->name_entry, "changed", 
                     G_CALLBACK(on_property_name_changed), NULL);
    g_signal_connect(g_property_panel->visible_check, "toggled", 
                     G_CALLBACK(on_property_visible_toggled), NULL);
    
    return frame;
}

static GtkWidget *create_nurbs_section(void) {
    GtkWidget *frame = create_dark_frame("NURBS Properties");
    GtkWidget *vbox = gtk_box_new(GTK_ORIENTATION_VERTICAL, 4);
    gtk_container_add(GTK_CONTAINER(frame), vbox);
    
    // Degree
    g_property_panel->degree_spin = gtk_spin_button_new_with_range(1, 10, 1);
    gtk_spin_button_set_value(GTK_SPIN_BUTTON(g_property_panel->degree_spin), 3);
    GtkWidget *degree_row = create_property_row("Degree:", g_property_panel->degree_spin);
    gtk_box_pack_start(GTK_BOX(vbox), degree_row, FALSE, FALSE, 0);
    
    // Control Points
    GtkWidget *cp_label = gtk_label_new("Control Points:");
    gtk_label_set_xalign(GTK_LABEL(cp_label), 0.0);
    gtk_box_pack_start(GTK_BOX(vbox), cp_label, FALSE, FALSE, 0);
    
    g_property_panel->control_points_list = gtk_list_box_new();
    GtkWidget *cp_scrolled = gtk_scrolled_window_new(NULL, NULL);
    gtk_scrolled_window_set_policy(GTK_SCROLLED_WINDOW(cp_scrolled), 
                                  GTK_POLICY_NEVER, GTK_POLICY_AUTOMATIC);
    gtk_widget_set_size_request(cp_scrolled, -1, 150);
    gtk_container_add(GTK_CONTAINER(cp_scrolled), g_property_panel->control_points_list);
    gtk_box_pack_start(GTK_BOX(vbox), cp_scrolled, FALSE, FALSE, 0);
    
    // Knots
    GtkWidget *knots_label = gtk_label_new("Knot Vector:");
    gtk_label_set_xalign(GTK_LABEL(knots_label), 0.0);
    gtk_box_pack_start(GTK_BOX(vbox), knots_label, FALSE, FALSE, 0);
    
    g_property_panel->knots_list = gtk_list_box_new();
    GtkWidget *knots_scrolled = gtk_scrolled_window_new(NULL, NULL);
    gtk_scrolled_window_set_policy(GTK_SCROLLED_WINDOW(knots_scrolled), 
                                  GTK_POLICY_NEVER, GTK_POLICY_AUTOMATIC);
    gtk_widget_set_size_request(knots_scrolled, -1, 100);
    gtk_container_add(GTK_CONTAINER(knots_scrolled), g_property_panel->knots_list);
    gtk_box_pack_start(GTK_BOX(vbox), knots_scrolled, FALSE, FALSE, 0);
    
    // Connect signals
    g_signal_connect(g_property_panel->degree_spin, "value-changed", 
                     G_CALLBACK(on_property_degree_changed), NULL);
    
    return frame;
}

GtkWidget *create_property_panel(void) {
    g_property_panel = g_malloc0(sizeof(PropertyPanel));
    
    // Main container
    g_property_panel->container = gtk_box_new(GTK_ORIENTATION_VERTICAL, 0);
    gtk_widget_set_size_request(g_property_panel->container, 280, -1);
    gtk_style_context_add_class(gtk_widget_get_style_context(g_property_panel->container), 
                               "property-panel");
    
    // Header
    GtkWidget *header = gtk_label_new("Properties");
    gtk_style_context_add_class(gtk_widget_get_style_context(header), "hierarchy-header");
    gtk_box_pack_start(GTK_BOX(g_property_panel->container), header, FALSE, FALSE, 0);
    
    // Scrolled window for content
    g_property_panel->scrolled_window = gtk_scrolled_window_new(NULL, NULL);
    gtk_scrolled_window_set_policy(GTK_SCROLLED_WINDOW(g_property_panel->scrolled_window), 
                                  GTK_POLICY_NEVER, GTK_POLICY_AUTOMATIC);
    gtk_box_pack_start(GTK_BOX(g_property_panel->container), 
                      g_property_panel->scrolled_window, TRUE, TRUE, 0);
    
    // Content box
    g_property_panel->content_box = gtk_box_new(GTK_ORIENTATION_VERTICAL, 8);
    gtk_widget_set_margin_top(g_property_panel->content_box, 8);
    gtk_widget_set_margin_bottom(g_property_panel->content_box, 8);
    gtk_widget_set_margin_start(g_property_panel->content_box, 8);
    gtk_widget_set_margin_end(g_property_panel->content_box, 8);
    gtk_container_add(GTK_CONTAINER(g_property_panel->scrolled_window), 
                     g_property_panel->content_box);
    
    // Create sections
    GtkWidget *general_section = create_general_section();
    GtkWidget *transform_section = create_transform_section();
    GtkWidget *nurbs_section = create_nurbs_section();
    
    gtk_box_pack_start(GTK_BOX(g_property_panel->content_box), general_section, FALSE, FALSE, 0);
    gtk_box_pack_start(GTK_BOX(g_property_panel->content_box), transform_section, FALSE, FALSE, 0);
    gtk_box_pack_start(GTK_BOX(g_property_panel->content_box), nurbs_section, FALSE, FALSE, 0);
    
    // Initially hide NURBS section
    gtk_widget_set_visible(nurbs_section, FALSE);
    
    g_property_panel->current_object = NULL;
    
    return g_property_panel->container;
}

void property_panel_set_object(MapObject *object) {
    g_property_panel->current_object = object;
    
    if (!object) {
        property_panel_clear();
        return;
    }
    
    // Update general properties
    gtk_entry_set_text(GTK_ENTRY(g_property_panel->name_entry), object->name);
    gtk_toggle_button_set_active(GTK_TOGGLE_BUTTON(g_property_panel->visible_check), 
                                object->visible);
    
    // Update transform
    property_panel_update_transform();
    
    // Update NURBS data if applicable
    if (object->type == OBJECT_CURVE || object->type == OBJECT_SURFACE) {
        property_panel_update_nurbs_data();
        // Show NURBS section
        GtkWidget *nurbs_frame = gtk_widget_get_parent(
            gtk_widget_get_parent(g_property_panel->degree_spin));
        gtk_widget_set_visible(nurbs_frame, TRUE);
    } else {
        // Hide NURBS section
        GtkWidget *nurbs_frame = gtk_widget_get_parent(
            gtk_widget_get_parent(g_property_panel->degree_spin));
        gtk_widget_set_visible(nurbs_frame, FALSE);
    }
}

void property_panel_clear(void) {
    gtk_entry_set_text(GTK_ENTRY(g_property_panel->name_entry), "");
    gtk_toggle_button_set_active(GTK_TOGGLE_BUTTON(g_property_panel->visible_check), FALSE);
    
    // Clear transform values
    gtk_spin_button_set_value(GTK_SPIN_BUTTON(g_property_panel->pos_x_spin), 0.0);
    gtk_spin_button_set_value(GTK_SPIN_BUTTON(g_property_panel->pos_y_spin), 0.0);
    gtk_spin_button_set_value(GTK_SPIN_BUTTON(g_property_panel->pos_z_spin), 0.0);
    gtk_spin_button_set_value(GTK_SPIN_BUTTON(g_property_panel->rot_x_spin), 0.0);
    gtk_spin_button_set_value(GTK_SPIN_BUTTON(g_property_panel->rot_y_spin), 0.0);
    gtk_spin_button_set_value(GTK_SPIN_BUTTON(g_property_panel->rot_z_spin), 0.0);
    gtk_spin_button_set_value(GTK_SPIN_BUTTON(g_property_panel->scale_x_spin), 1.0);
    gtk_spin_button_set_value(GTK_SPIN_BUTTON(g_property_panel->scale_y_spin), 1.0);
    gtk_spin_button_set_value(GTK_SPIN_BUTTON(g_property_panel->scale_z_spin), 1.0);
    
    // Hide NURBS section
    GtkWidget *nurbs_frame = gtk_widget_get_parent(
        gtk_widget_get_parent(g_property_panel->degree_spin));
    gtk_widget_set_visible(nurbs_frame, FALSE);
}

void property_panel_update_transform(void) {
    if (!g_property_panel->current_object) return;
    
    MapObject *obj = g_property_panel->current_object;
    
    gtk_spin_button_set_value(GTK_SPIN_BUTTON(g_property_panel->pos_x_spin), obj->position[0]);
    gtk_spin_button_set_value(GTK_SPIN_BUTTON(g_property_panel->pos_y_spin), obj->position[1]);
    gtk_spin_button_set_value(GTK_SPIN_BUTTON(g_property_panel->pos_z_spin), obj->position[2]);
    
    gtk_spin_button_set_value(GTK_SPIN_BUTTON(g_property_panel->rot_x_spin), 
                             obj->rotation[0] * 180.0f / M_PI);
    gtk_spin_button_set_value(GTK_SPIN_BUTTON(g_property_panel->rot_y_spin), 
                             obj->rotation[1] * 180.0f / M_PI);
    gtk_spin_button_set_value(GTK_SPIN_BUTTON(g_property_panel->rot_z_spin), 
                             obj->rotation[2] * 180.0f / M_PI);
    
    gtk_spin_button_set_value(GTK_SPIN_BUTTON(g_property_panel->scale_x_spin), obj->scale[0]);
    gtk_spin_button_set_value(GTK_SPIN_BUTTON(g_property_panel->scale_y_spin), obj->scale[1]);
    gtk_spin_button_set_value(GTK_SPIN_BUTTON(g_property_panel->scale_z_spin), obj->scale[2]);
}

void property_panel_update_nurbs_data(void) {
    if (!g_property_panel->current_object) return;
    
    MapObject *obj = g_property_panel->current_object;
    
    if (obj->type == OBJECT_CURVE && obj->data.curve) {
        NurbsCurve *curve = obj->data.curve;
        gtk_spin_button_set_value(GTK_SPIN_BUTTON(g_property_panel->degree_spin), curve->degree);
        
        // TODO: Update control points and knots lists
    }
}

// Signal callbacks
void on_property_name_changed(GtkEntry *entry, gpointer user_data) {
    if (!g_property_panel->current_object) return;
    
    const char *new_name = gtk_entry_get_text(entry);
    g_free(g_property_panel->current_object->name);
    g_property_panel->current_object->name = g_strdup(new_name);
    
    // TODO: Update hierarchy panel
}

void on_property_visible_toggled(GtkToggleButton *button, gpointer user_data) {
    if (!g_property_panel->current_object) return;
    
    g_property_panel->current_object->visible = gtk_toggle_button_get_active(button);
    
    // TODO: Refresh viewport
}

void on_property_transform_changed(GtkSpinButton *spin, gpointer user_data) {
    if (!g_property_panel->current_object) return;
    
    MapObject *obj = g_property_panel->current_object;
    
    obj->position[0] = gtk_spin_button_get_value(GTK_SPIN_BUTTON(g_property_panel->pos_x_spin));
    obj->position[1] = gtk_spin_button_get_value(GTK_SPIN_BUTTON(g_property_panel->pos_y_spin));
    obj->position[2] = gtk_spin_button_get_value(GTK_SPIN_BUTTON(g_property_panel->pos_z_spin));
    
    obj->rotation[0] = gtk_spin_button_get_value(GTK_SPIN_BUTTON(g_property_panel->rot_x_spin)) * M_PI / 180.0f;
    obj->rotation[1] = gtk_spin_button_get_value(GTK_SPIN_BUTTON(g_property_panel->rot_y_spin)) * M_PI / 180.0f;
    obj->rotation[2] = gtk_spin_button_get_value(GTK_SPIN_BUTTON(g_property_panel->rot_z_spin)) * M_PI / 180.0f;
    
    obj->scale[0] = gtk_spin_button_get_value(GTK_SPIN_BUTTON(g_property_panel->scale_x_spin));
    obj->scale[1] = gtk_spin_button_get_value(GTK_SPIN_BUTTON(g_property_panel->scale_y_spin));
    obj->scale[2] = gtk_spin_button_get_value(GTK_SPIN_BUTTON(g_property_panel->scale_z_spin));
    
    // TODO: Refresh viewport
}

void on_property_degree_changed(GtkSpinButton *spin, gpointer user_data) {
    if (!g_property_panel->current_object || 
        g_property_panel->current_object->type != OBJECT_CURVE) return;
    
    NurbsCurve *curve = g_property_panel->current_object->data.curve;
    if (!curve) return;
    
    int new_degree = gtk_spin_button_get_value_as_int(spin);
    if (new_degree != curve->degree) {
        curve->degree = new_degree;
        curve->dirty = TRUE;
        
        // TODO: Regenerate knot vector and refresh viewport
    }
}