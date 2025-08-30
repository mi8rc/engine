#ifndef HIERARCHY_PANEL_H
#define HIERARCHY_PANEL_H

#include "main.h"
#include "nurbs.h"

typedef struct {
    GtkWidget *container;
    GtkWidget *tree_view;
    GtkTreeStore *tree_store;
    GtkWidget *toolbar;
    
    // Toolbar buttons
    GtkWidget *add_curve_btn;
    GtkWidget *add_surface_btn;
    GtkWidget *add_group_btn;
    GtkWidget *delete_btn;
    
} HierarchyPanel;

// Tree store columns
enum {
    HIERARCHY_COL_NAME = 0,
    HIERARCHY_COL_OBJECT,
    HIERARCHY_COL_ICON,
    HIERARCHY_COL_VISIBLE,
    HIERARCHY_N_COLUMNS
};

// Function declarations
GtkWidget *create_hierarchy_panel(void);
void hierarchy_panel_refresh(void);
void hierarchy_panel_add_object(MapObject *object);
void hierarchy_panel_remove_object(MapObject *object);
void hierarchy_panel_select_object(MapObject *object);

// Signal callbacks
void on_hierarchy_selection_changed(GtkTreeSelection *selection, gpointer user_data);
void on_hierarchy_row_activated(GtkTreeView *tree_view, GtkTreePath *path, 
                               GtkTreeViewColumn *column, gpointer user_data);
void on_hierarchy_add_curve_clicked(GtkButton *button, gpointer user_data);
void on_hierarchy_add_surface_clicked(GtkButton *button, gpointer user_data);
void on_hierarchy_add_group_clicked(GtkButton *button, gpointer user_data);
void on_hierarchy_delete_clicked(GtkButton *button, gpointer user_data);
gboolean on_hierarchy_button_press(GtkWidget *widget, GdkEventButton *event, gpointer user_data);

#endif // HIERARCHY_PANEL_H