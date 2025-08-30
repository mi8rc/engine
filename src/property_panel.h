#ifndef PROPERTY_PANEL_H
#define PROPERTY_PANEL_H

#include "main.h"
#include "nurbs.h"

typedef struct {
    GtkWidget *container;
    GtkWidget *scrolled_window;
    GtkWidget *content_box;
    
    // Current object being edited
    MapObject *current_object;
    
    // Transform widgets
    GtkWidget *pos_x_spin, *pos_y_spin, *pos_z_spin;
    GtkWidget *rot_x_spin, *rot_y_spin, *rot_z_spin;
    GtkWidget *scale_x_spin, *scale_y_spin, *scale_z_spin;
    
    // Object-specific widgets
    GtkWidget *name_entry;
    GtkWidget *visible_check;
    
    // NURBS-specific widgets
    GtkWidget *degree_spin;
    GtkWidget *control_points_list;
    GtkWidget *knots_list;
    
} PropertyPanel;

// Function declarations
GtkWidget *create_property_panel(void);
void property_panel_set_object(MapObject *object);
void property_panel_clear(void);
void property_panel_update_transform(void);
void property_panel_update_nurbs_data(void);

// Signal callbacks
void on_property_name_changed(GtkEntry *entry, gpointer user_data);
void on_property_visible_toggled(GtkToggleButton *button, gpointer user_data);
void on_property_transform_changed(GtkSpinButton *spin, gpointer user_data);
void on_property_degree_changed(GtkSpinButton *spin, gpointer user_data);

#endif // PROPERTY_PANEL_H