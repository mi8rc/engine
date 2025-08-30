#ifndef UI_BUILDER_H
#define UI_BUILDER_H

#include "main.h"

// CSS styling
void apply_custom_styling(void);
void load_css_from_string(const char *css_data);

// Widget creation helpers
GtkWidget *create_dark_frame(const char *title);
GtkWidget *create_property_row(const char *label, GtkWidget *widget);
GtkWidget *create_toolbar_button(const char *icon_name, const char *tooltip);
GtkWidget *create_tree_view_with_model(GtkTreeModel *model);

// Roblox Studio style constants
extern const char *ROBLOX_STUDIO_CSS;

#endif // UI_BUILDER_H