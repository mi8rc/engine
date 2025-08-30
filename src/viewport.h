#ifndef VIEWPORT_H
#define VIEWPORT_H

#include "main.h"

typedef struct {
    GtkWidget *gl_area;
    GtkWidget *container;
    
    // OpenGL context
    gboolean gl_initialized;
    GLuint shader_program;
    GLuint vao, vbo;
    
    // Camera controls
    gboolean mouse_dragging;
    gdouble last_mouse_x, last_mouse_y;
    
    // Grid rendering
    GLuint grid_vao, grid_vbo;
    gboolean show_grid;
    
} Viewport;

// Function declarations
GtkWidget *create_viewport(void);
void viewport_render(Viewport *viewport);
void viewport_init_gl(Viewport *viewport);
void viewport_cleanup_gl(Viewport *viewport);
void viewport_update_camera(Viewport *viewport);

// OpenGL callbacks
gboolean on_gl_area_render(GtkGLArea *area, GdkGLContext *context, gpointer user_data);
void on_gl_area_realize(GtkGLArea *area, gpointer user_data);
void on_gl_area_unrealize(GtkGLArea *area, gpointer user_data);
gboolean on_gl_area_button_press(GtkWidget *widget, GdkEventButton *event, gpointer user_data);
gboolean on_gl_area_button_release(GtkWidget *widget, GdkEventButton *event, gpointer user_data);
gboolean on_gl_area_motion_notify(GtkWidget *widget, GdkEventMotion *event, gpointer user_data);
gboolean on_gl_area_scroll(GtkWidget *widget, GdkEventScroll *event, gpointer user_data);

#endif // VIEWPORT_H