#include "main.h"
#include "window.h"
#include "ui_builder.h"
#include "tools.h"

EditorState *g_editor_state = NULL;

static void activate(GtkApplication *app, gpointer user_data) {
    // Initialize editor state
    g_editor_state = editor_state_new();
    
    // Initialize tools
    tools_init();
    
    // Create main window
    GtkWidget *window = create_main_window(app);
    
    // Apply custom CSS styling
    apply_custom_styling();
    
    // Show the window
    gtk_widget_show_all(window);
}

int main(int argc, char **argv) {
    GtkApplication *app;
    int status;

    app = gtk_application_new("com.nurbsmapeditor.app", G_APPLICATION_FLAGS_NONE);
    g_signal_connect(app, "activate", G_CALLBACK(activate), NULL);
    status = g_application_run(G_APPLICATION(app), argc, argv);
    g_object_unref(app);

    // Cleanup
    if (g_editor_state) {
        editor_state_free(g_editor_state);
    }
    
    tools_cleanup();

    return status;
}