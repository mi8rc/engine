/*
 * NURBS Map Editor - Main Entry Point
 * A Roblox Studio-like editor for NURBS-based FPS games with full IGES support
 * 
 * Features:
 * - Interactive 3D viewport with OpenGL rendering
 * - Scene hierarchy management
 * - Properties panel for object manipulation
 * - NURBS primitive creation (sphere, plane, cylinder, torus)
 * - Full IGES file format support
 * - Material and lighting system
 * - Undo/Redo functionality
 * - Save/Load project files
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <gtk/gtk.h>
#include <GL/gl.h>
#include <GL/glu.h>
#include <gtk/gtkgl.h>

#include "editor.h"
#include "iges_loader.h"
#include "nurbs_primitives.h"

// Global editor instance
MapEditor *g_editor = NULL;

// Application info
#define APP_NAME "NURBS Map Editor"
#define APP_VERSION "1.0"
#define APP_DESCRIPTION "A Roblox Studio-like editor for NURBS-based FPS games"

static void on_activate(GtkApplication *app, gpointer user_data) {
    printf("Starting %s v%s\n", APP_NAME, APP_VERSION);
    printf("%s\n", APP_DESCRIPTION);
    printf("=" * 60);
    printf("\n");

    // Initialize the map editor
    g_editor = editor_create(app);
    if (!g_editor) {
        fprintf(stderr, "Failed to create map editor\n");
        return;
    }

    // Show the main window
    editor_show(g_editor);
}

static void on_shutdown(GtkApplication *app, gpointer user_data) {
    printf("Shutting down %s...\n", APP_NAME);
    
    if (g_editor) {
        editor_destroy(g_editor);
        g_editor = NULL;
    }
}

int main(int argc, char **argv) {
    GtkApplication *app;
    int status;

    // Create GTK application
    app = gtk_application_new("com.nurbseditor.mapeditor", G_APPLICATION_DEFAULT_FLAGS);
    
    // Connect signals
    g_signal_connect(app, "activate", G_CALLBACK(on_activate), NULL);
    g_signal_connect(app, "shutdown", G_CALLBACK(on_shutdown), NULL);

    // Run the application
    status = g_application_run(G_APPLICATION(app), argc, argv);
    
    // Cleanup
    g_object_unref(app);
    
    return status;
}