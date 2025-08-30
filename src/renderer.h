#ifndef RENDERER_H
#define RENDERER_H

#include "main.h"

// Matrix operations
void create_identity_matrix(float *matrix);
void create_translation_matrix(float *matrix, float x, float y, float z);
void create_rotation_matrix(float *matrix, float x, float y, float z);
void create_scale_matrix(float *matrix, float x, float y, float z);
void create_view_matrix(float *matrix, float *position, float *rotation);
void create_projection_matrix(float *matrix, float fov, float aspect, float near, float far);
void multiply_matrices(float *result, const float *a, const float *b);

// Rendering functions
void render_nurbs_objects(void);
void render_grid(void);
void render_axes(void);
void render_selection_outline(void);

// Utility functions
void screen_to_world(int screen_x, int screen_y, float *world_pos);
void world_to_screen(float *world_pos, int *screen_x, int *screen_y);

#endif // RENDERER_H