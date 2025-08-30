#include "ui_builder.h"

const char *ROBLOX_STUDIO_CSS = 
"* {\n"
"    color: #FFFFFF;\n"
"    font-family: 'Segoe UI', Arial, sans-serif;\n"
"    font-size: 12px;\n"
"}\n"
"\n"
"window {\n"
"    background-color: #2D2D30;\n"
"}\n"
"\n"
"headerbar {\n"
"    background-color: #3E3E42;\n"
"    border-bottom: 1px solid #464647;\n"
"}\n"
"\n"
"menubar {\n"
"    background-color: #3E3E42;\n"
"    border-bottom: 1px solid #464647;\n"
"}\n"
"\n"
"menubar > menuitem {\n"
"    padding: 6px 12px;\n"
"    color: #FFFFFF;\n"
"}\n"
"\n"
"menubar > menuitem:hover {\n"
"    background-color: #007ACC;\n"
"}\n"
"\n"
"toolbar {\n"
"    background-color: #4D4D50;\n"
"    border-bottom: 1px solid #464647;\n"
"    padding: 4px;\n"
"}\n"
"\n"
"toolbutton {\n"
"    padding: 4px 8px;\n"
"    margin: 2px;\n"
"    border-radius: 3px;\n"
"}\n"
"\n"
"toolbutton:hover {\n"
"    background-color: #007ACC;\n"
"}\n"
"\n"
"paned {\n"
"    background-color: #2D2D30;\n"
"}\n"
"\n"
"paned > separator {\n"
"    background-color: #464647;\n"
"    min-width: 1px;\n"
"    min-height: 1px;\n"
"}\n"
"\n"
"frame {\n"
"    background-color: #3E3E42;\n"
"    border: 1px solid #464647;\n"
"    border-radius: 4px;\n"
"    padding: 8px;\n"
"}\n"
"\n"
"frame > label {\n"
"    background-color: #3E3E42;\n"
"    color: #FFFFFF;\n"
"    font-weight: bold;\n"
"    padding: 0 8px;\n"
"}\n"
"\n"
"treeview {\n"
"    background-color: #2D2D30;\n"
"    color: #FFFFFF;\n"
"    border: 1px solid #464647;\n"
"}\n"
"\n"
"treeview:selected {\n"
"    background-color: #007ACC;\n"
"}\n"
"\n"
"treeview header {\n"
"    background-color: #4D4D50;\n"
"    border-bottom: 1px solid #464647;\n"
"}\n"
"\n"
"scrolledwindow {\n"
"    background-color: #2D2D30;\n"
"}\n"
"\n"
"scrollbar {\n"
"    background-color: #3E3E42;\n"
"    border: 1px solid #464647;\n"
"}\n"
"\n"
"scrollbar slider {\n"
"    background-color: #686868;\n"
"    border-radius: 4px;\n"
"    min-width: 12px;\n"
"    min-height: 12px;\n"
"}\n"
"\n"
"scrollbar slider:hover {\n"
"    background-color: #9E9E9E;\n"
"}\n"
"\n"
"entry {\n"
"    background-color: #1E1E1E;\n"
"    color: #FFFFFF;\n"
"    border: 1px solid #464647;\n"
"    border-radius: 3px;\n"
"    padding: 4px 8px;\n"
"}\n"
"\n"
"entry:focus {\n"
"    border-color: #007ACC;\n"
"    box-shadow: 0 0 0 1px #007ACC;\n"
"}\n"
"\n"
"spinbutton {\n"
"    background-color: #1E1E1E;\n"
"    color: #FFFFFF;\n"
"    border: 1px solid #464647;\n"
"    border-radius: 3px;\n"
"}\n"
"\n"
"spinbutton:focus {\n"
"    border-color: #007ACC;\n"
"}\n"
"\n"
"button {\n"
"    background-color: #0E639C;\n"
"    color: #FFFFFF;\n"
"    border: 1px solid #007ACC;\n"
"    border-radius: 3px;\n"
"    padding: 6px 12px;\n"
"}\n"
"\n"
"button:hover {\n"
"    background-color: #1177BB;\n"
"}\n"
"\n"
"button:active {\n"
"    background-color: #005A9E;\n"
"}\n"
"\n"
"checkbutton {\n"
"    color: #FFFFFF;\n"
"}\n"
"\n"
"checkbutton check {\n"
"    background-color: #1E1E1E;\n"
"    border: 1px solid #464647;\n"
"    border-radius: 2px;\n"
"}\n"
"\n"
"checkbutton check:checked {\n"
"    background-color: #007ACC;\n"
"    border-color: #007ACC;\n"
"}\n"
"\n"
"statusbar {\n"
"    background-color: #007ACC;\n"
"    color: #FFFFFF;\n"
"    border-top: 1px solid #464647;\n"
"    padding: 4px 8px;\n"
"}\n"
"\n"
"/* GL Area styling */\n"
"glarea {\n"
"    background-color: #2D2D30;\n"
"    border: 1px solid #464647;\n"
"}\n"
"\n"
"/* Property panel specific styling */\n"
".property-panel {\n"
"    background-color: #3E3E42;\n"
"    padding: 8px;\n"
"}\n"
"\n"
".property-row {\n"
"    margin: 4px 0;\n"
"}\n"
"\n"
".property-label {\n"
"    color: #CCCCCC;\n"
"    font-size: 11px;\n"
"    margin-bottom: 2px;\n"
"}\n"
"\n"
"/* Hierarchy panel styling */\n"
".hierarchy-panel {\n"
"    background-color: #3E3E42;\n"
"}\n"
"\n"
".hierarchy-header {\n"
"    background-color: #4D4D50;\n"
"    padding: 8px;\n"
"    border-bottom: 1px solid #464647;\n"
"    font-weight: bold;\n"
"}\n";

void apply_custom_styling(void) {
    load_css_from_string(ROBLOX_STUDIO_CSS);
}

void load_css_from_string(const char *css_data) {
    GtkCssProvider *provider = gtk_css_provider_new();
    GdkDisplay *display = gdk_display_get_default();
    GdkScreen *screen = gdk_display_get_default_screen(display);
    
    gtk_style_context_add_provider_for_screen(screen,
                                             GTK_STYLE_PROVIDER(provider),
                                             GTK_STYLE_PROVIDER_PRIORITY_APPLICATION);
    
    GError *error = NULL;
    gtk_css_provider_load_from_data(provider, css_data, -1, &error);
    
    if (error) {
        g_warning("Failed to load CSS: %s", error->message);
        g_error_free(error);
    }
    
    g_object_unref(provider);
}

GtkWidget *create_dark_frame(const char *title) {
    GtkWidget *frame = gtk_frame_new(title);
    gtk_widget_set_margin_top(frame, 4);
    gtk_widget_set_margin_bottom(frame, 4);
    gtk_widget_set_margin_start(frame, 4);
    gtk_widget_set_margin_end(frame, 4);
    
    return frame;
}

GtkWidget *create_property_row(const char *label, GtkWidget *widget) {
    GtkWidget *vbox = gtk_box_new(GTK_ORIENTATION_VERTICAL, 2);
    gtk_style_context_add_class(gtk_widget_get_style_context(vbox), "property-row");
    
    GtkWidget *label_widget = gtk_label_new(label);
    gtk_label_set_xalign(GTK_LABEL(label_widget), 0.0);
    gtk_style_context_add_class(gtk_widget_get_style_context(label_widget), "property-label");
    
    gtk_box_pack_start(GTK_BOX(vbox), label_widget, FALSE, FALSE, 0);
    gtk_box_pack_start(GTK_BOX(vbox), widget, FALSE, FALSE, 0);
    
    return vbox;
}

GtkWidget *create_toolbar_button(const char *icon_name, const char *tooltip) {
    GtkWidget *button = gtk_button_new();
    
    if (icon_name) {
        GtkWidget *icon = gtk_image_new_from_icon_name(icon_name, GTK_ICON_SIZE_SMALL_TOOLBAR);
        gtk_button_set_image(GTK_BUTTON(button), icon);
    }
    
    if (tooltip) {
        gtk_widget_set_tooltip_text(button, tooltip);
    }
    
    gtk_button_set_relief(GTK_BUTTON(button), GTK_RELIEF_NONE);
    
    return button;
}

GtkWidget *create_tree_view_with_model(GtkTreeModel *model) {
    GtkWidget *tree_view = gtk_tree_view_new_with_model(model);
    
    // Configure tree view appearance
    gtk_tree_view_set_headers_visible(GTK_TREE_VIEW(tree_view), FALSE);
    gtk_tree_view_set_show_expanders(GTK_TREE_VIEW(tree_view), TRUE);
    gtk_tree_view_set_level_indentation(GTK_TREE_VIEW(tree_view), 12);
    
    // Add columns
    GtkCellRenderer *renderer = gtk_cell_renderer_text_new();
    GtkTreeViewColumn *column = gtk_tree_view_column_new_with_attributes(
        "Name", renderer, "text", 0, NULL);
    gtk_tree_view_append_column(GTK_TREE_VIEW(tree_view), column);
    
    return tree_view;
}