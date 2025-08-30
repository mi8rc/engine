#include "renderer.h"
#include "nurbs.h"
#include "window.h"
#include <math.h>

void create_identity_matrix(float *matrix) {
    for (int i = 0; i < 16; i++) {
        matrix[i] = 0.0f;
    }
    matrix[0] = matrix[5] = matrix[10] = matrix[15] = 1.0f;
}

void create_translation_matrix(float *matrix, float x, float y, float z) {
    create_identity_matrix(matrix);
    matrix[12] = x;
    matrix[13] = y;
    matrix[14] = z;
}

void create_rotation_matrix(float *matrix, float x, float y, float z) {
    float cx = cosf(x), sx = sinf(x);
    float cy = cosf(y), sy = sinf(y);
    float cz = cosf(z), sz = sinf(z);
    
    matrix[0] = cy * cz;
    matrix[1] = cx * sz + sx * sy * cz;
    matrix[2] = sx * sz - cx * sy * cz;
    matrix[3] = 0.0f;
    
    matrix[4] = -cy * sz;
    matrix[5] = cx * cz - sx * sy * sz;
    matrix[6] = sx * cz + cx * sy * sz;
    matrix[7] = 0.0f;
    
    matrix[8] = sy;
    matrix[9] = -sx * cy;
    matrix[10] = cx * cy;
    matrix[11] = 0.0f;
    
    matrix[12] = matrix[13] = matrix[14] = 0.0f;
    matrix[15] = 1.0f;
}

void create_scale_matrix(float *matrix, float x, float y, float z) {
    create_identity_matrix(matrix);
    matrix[0] = x;
    matrix[5] = y;
    matrix[10] = z;
}

void create_view_matrix(float *matrix, float *position, float *rotation) {
    float translation[16], rotation_mat[16];
    
    create_translation_matrix(translation, -position[0], -position[1], -position[2]);
    create_rotation_matrix(rotation_mat, -rotation[0], -rotation[1], -rotation[2]);
    
    multiply_matrices(matrix, rotation_mat, translation);
}

void create_projection_matrix(float *matrix, float fov, float aspect, float near, float far) {
    float f = 1.0f / tanf(fov * M_PI / 360.0f);
    
    for (int i = 0; i < 16; i++) {
        matrix[i] = 0.0f;
    }
    
    matrix[0] = f / aspect;
    matrix[5] = f;
    matrix[10] = (far + near) / (near - far);
    matrix[11] = -1.0f;
    matrix[14] = (2.0f * far * near) / (near - far);
}

void multiply_matrices(float *result, const float *a, const float *b) {
    for (int i = 0; i < 4; i++) {
        for (int j = 0; j < 4; j++) {
            result[i * 4 + j] = 0.0f;
            for (int k = 0; k < 4; k++) {
                result[i * 4 + j] += a[i * 4 + k] * b[k * 4 + j];
            }
        }
    }
}

void render_nurbs_objects(void) {
    if (!g_editor_state || !g_editor_state->objects) return;
    
    GList *current = g_editor_state->objects;
    while (current) {
        MapObject *object = (MapObject*)current->data;
        
        if (!object->visible) {
            current = current->next;
            continue;
        }
        
        // Apply object transform
        float model[16];
        float translation[16], rotation_mat[16], scale_mat[16], temp[16];
        
        create_translation_matrix(translation, 
                                 object->position[0], 
                                 object->position[1], 
                                 object->position[2]);
        create_rotation_matrix(rotation_mat, 
                              object->rotation[0], 
                              object->rotation[1], 
                              object->rotation[2]);
        create_scale_matrix(scale_mat, 
                           object->scale[0], 
                           object->scale[1], 
                           object->scale[2]);
        
        multiply_matrices(temp, translation, rotation_mat);
        multiply_matrices(model, temp, scale_mat);
        
        // TODO: Set model matrix uniform in shader
        
        // Render based on object type
        switch (object->type) {
            case OBJECT_CURVE:
                if (object->data.curve) {
                    nurbs_curve_render(object->data.curve);
                }
                break;
            case OBJECT_SURFACE:
                if (object->data.surface) {
                    nurbs_surface_render(object->data.surface);
                }
                break;
            case OBJECT_GROUP:
                // Groups don't render themselves
                break;
        }
        
        current = current->next;
    }
}