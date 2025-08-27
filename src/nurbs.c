#include "nurbs.h"
#include <string.h>
#include <math.h>
#include <stdio.h>
#include <stddef.h>
#include <stdbool.h>

// Define M_PI if not available (MSYS2/Windows compatibility)
#ifndef M_PI
#define M_PI 3.14159265358979323846
#endif

// OpenGL initialization tracking (Windows uses stub functions, no initialization needed)
static bool gl_ready = false;

// Helper function to ensure OpenGL is ready
static void ensure_gl_ready() {
    if (!gl_ready) {
#if defined(WIN32) || defined(_WIN32)
        // Windows: using stub OpenGL functions, no initialization needed
        printf("Using OpenGL compatibility layer for Windows\n");
#endif
        gl_ready = true;
    }
}

// Calculate B-spline basis function using Cox-de Boor recursion
float nurbs_basis_function(int i, int degree, float t, float *knots) {
    if (degree == 0) {
        return (t >= knots[i] && t < knots[i + 1]) ? 1.0f : 0.0f;
    }
    
    float left = 0.0f, right = 0.0f;
    
    // Left term
    if (fabs(knots[i + degree] - knots[i]) > EPSILON) {
        left = (t - knots[i]) / (knots[i + degree] - knots[i]) * 
               nurbs_basis_function(i, degree - 1, t, knots);
    }
    
    // Right term
    if (fabs(knots[i + degree + 1] - knots[i + 1]) > EPSILON) {
        right = (knots[i + degree + 1] - t) / (knots[i + degree + 1] - knots[i + 1]) * 
                nurbs_basis_function(i + 1, degree - 1, t, knots);
    }
    
    return left + right;
}

// Evaluate NURBS curve at parameter t
Vector3 evaluate_nurbs_curve(NURBSCurve *curve, float t) {
    Vector3 result = {0.0f, 0.0f, 0.0f};
    float weight_sum = 0.0f;
    
    for (int i = 0; i < curve->num_control_points; i++) {
        float basis = nurbs_basis_function(i, curve->degree, t, curve->knots);
        float weight = curve->control_points[i].w * basis;
        
        result.x += curve->control_points[i].x * weight;
        result.y += curve->control_points[i].y * weight;
        result.z += curve->control_points[i].z * weight;
        weight_sum += weight;
    }
    
    if (weight_sum > EPSILON) {
        result.x /= weight_sum;
        result.y /= weight_sum;
        result.z /= weight_sum;
    }
    
    return result;
}

// Evaluate NURBS surface at parameters (u, v)
SurfacePoint evaluate_nurbs_surface(NURBSSurface *surface, float u, float v) {
    SurfacePoint result;
    Vector3 position = {0.0f, 0.0f, 0.0f};
    Vector3 du = {0.0f, 0.0f, 0.0f};
    Vector3 dv = {0.0f, 0.0f, 0.0f};
    float weight_sum = 0.0f;
    float du_weight_sum = 0.0f;
    float dv_weight_sum = 0.0f;
    
    // Evaluate surface position and partial derivatives
    for (int i = 0; i < surface->num_control_points_u; i++) {
        for (int j = 0; j < surface->num_control_points_v; j++) {
            float basis_u = nurbs_basis_function(i, surface->degree_u, u, surface->knots_u);
            float basis_v = nurbs_basis_function(j, surface->degree_v, v, surface->knots_v);
            float basis_uv = basis_u * basis_v;
            float weight = surface->control_points[i][j].w * basis_uv;
            
            Vector4 cp = surface->control_points[i][j];
            
            // Surface position
            position.x += cp.x * weight;
            position.y += cp.y * weight;
            position.z += cp.z * weight;
            weight_sum += weight;
            
            // Partial derivatives (simplified for demonstration)
            // In practice, you'd calculate derivative basis functions
            if (i > 0) {
                float du_basis = (basis_u - nurbs_basis_function(i-1, surface->degree_u, u, surface->knots_u)) * basis_v;
                float du_weight = cp.w * du_basis;
                du.x += cp.x * du_weight;
                du.y += cp.y * du_weight;
                du.z += cp.z * du_weight;
                du_weight_sum += du_weight;
            }
            
            if (j > 0) {
                float dv_basis = basis_u * (basis_v - nurbs_basis_function(j-1, surface->degree_v, v, surface->knots_v));
                float dv_weight = cp.w * dv_basis;
                dv.x += cp.x * dv_weight;
                dv.y += cp.y * dv_weight;
                dv.z += cp.z * dv_weight;
                dv_weight_sum += dv_weight;
            }
        }
    }
    
    // Normalize by weights
    if (weight_sum > EPSILON) {
        position.x /= weight_sum;
        position.y /= weight_sum;
        position.z /= weight_sum;
    }
    
    if (du_weight_sum > EPSILON) {
        du.x /= du_weight_sum;
        du.y /= du_weight_sum;
        du.z /= du_weight_sum;
    }
    
    if (dv_weight_sum > EPSILON) {
        dv.x /= dv_weight_sum;
        dv.y /= dv_weight_sum;
        dv.z /= dv_weight_sum;
    }
    
    result.position = position;
    result.tangent_u = du;
    result.tangent_v = dv;
    result.normal = vector3_normalize(vector3_cross(du, dv));
    
    return result;
}

// Tessellate NURBS surface into triangles for rendering
TessellatedSurface* tessellate_nurbs_surface(NURBSSurface *surface, int res_u, int res_v) {
    // Ensure OpenGL is ready
    ensure_gl_ready();
    
    TessellatedSurface *tess = malloc(sizeof(TessellatedSurface));
    tess->resolution_u = res_u;
    tess->resolution_v = res_v;
    tess->points = malloc(sizeof(SurfacePoint) * res_u * res_v);
    tess->num_triangles = (res_u - 1) * (res_v - 1) * 2;
    tess->indices = malloc(sizeof(unsigned int) * tess->num_triangles * 3);
    
    // Generate surface points
    for (int i = 0; i < res_u; i++) {
        for (int j = 0; j < res_v; j++) {
            float u = (float)i / (res_u - 1);
            float v = (float)j / (res_v - 1);
            tess->points[i * res_v + j] = evaluate_nurbs_surface(surface, u, v);
        }
    }
    
    // Generate triangle indices
    int index = 0;
    for (int i = 0; i < res_u - 1; i++) {
        for (int j = 0; j < res_v - 1; j++) {
            int base = i * res_v + j;
            
            // First triangle
            tess->indices[index++] = base;
            tess->indices[index++] = base + res_v;
            tess->indices[index++] = base + 1;
            
            // Second triangle
            tess->indices[index++] = base + 1;
            tess->indices[index++] = base + res_v;
            tess->indices[index++] = base + res_v + 1;
        }
    }
    
    // Generate OpenGL buffers
    glGenVertexArrays(1, &tess->vao);
    glGenBuffers(1, &tess->vbo);
    glGenBuffers(1, &tess->ebo);
    
    glBindVertexArray(tess->vao);
    
    // Vertex buffer
    glBindBuffer(GL_ARRAY_BUFFER, tess->vbo);
    glBufferData(GL_ARRAY_BUFFER, sizeof(SurfacePoint) * res_u * res_v, tess->points, GL_STATIC_DRAW);
    
    // Position attribute
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(SurfacePoint), (void*)offsetof(SurfacePoint, position));
    glEnableVertexAttribArray(0);
    
    // Normal attribute
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, sizeof(SurfacePoint), (void*)offsetof(SurfacePoint, normal));
    glEnableVertexAttribArray(1);
    
    // Element buffer
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, tess->ebo);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(unsigned int) * tess->num_triangles * 3, tess->indices, GL_STATIC_DRAW);
    
    glBindVertexArray(0);
    
    return tess;
}

// Ray-NURBS surface intersection for collision detection
CollisionResult ray_nurbs_surface_intersection(Vector3 ray_origin, Vector3 ray_direction, NURBSSurface *surface) {
    CollisionResult result = {0};
    result.hit = 0;
    result.distance = INFINITY;
    
    // Tessellate surface at high resolution for collision
    TessellatedSurface *tess = tessellate_nurbs_surface(surface, 50, 50);
    
    // Check intersection with each triangle
    for (int i = 0; i < tess->num_triangles; i++) {
        int i0 = tess->indices[i * 3];
        int i1 = tess->indices[i * 3 + 1];
        int i2 = tess->indices[i * 3 + 2];
        
        Vector3 v0 = tess->points[i0].position;
        Vector3 v1 = tess->points[i1].position;
        Vector3 v2 = tess->points[i2].position;
        
        // Möller-Trumbore ray-triangle intersection
        Vector3 edge1 = vector3_subtract(v1, v0);
        Vector3 edge2 = vector3_subtract(v2, v0);
        Vector3 h = vector3_cross(ray_direction, edge2);
        float a = vector3_dot(edge1, h);
        
        if (fabs(a) < EPSILON) continue; // Ray parallel to triangle
        
        float f = 1.0f / a;
        Vector3 s = vector3_subtract(ray_origin, v0);
        float u = f * vector3_dot(s, h);
        
        if (u < 0.0f || u > 1.0f) continue;
        
        Vector3 q = vector3_cross(s, edge1);
        float v = f * vector3_dot(ray_direction, q);
        
        if (v < 0.0f || u + v > 1.0f) continue;
        
        float t = f * vector3_dot(edge2, q);
        
        if (t > EPSILON && t < result.distance) {
            result.hit = 1;
            result.distance = t;
            result.point = vector3_add(ray_origin, vector3_multiply(ray_direction, t));
            result.normal = vector3_normalize(vector3_cross(edge1, edge2));
        }
    }
    
    free_tessellated_surface(tess);
    return result;
}

// Vector operations
Vector3 vector3_add(Vector3 a, Vector3 b) {
    return (Vector3){a.x + b.x, a.y + b.y, a.z + b.z};
}

Vector3 vector3_subtract(Vector3 a, Vector3 b) {
    return (Vector3){a.x - b.x, a.y - b.y, a.z - b.z};
}

Vector3 vector3_multiply(Vector3 v, float scalar) {
    return (Vector3){v.x * scalar, v.y * scalar, v.z * scalar};
}

Vector3 vector3_cross(Vector3 a, Vector3 b) {
    return (Vector3){
        a.y * b.z - a.z * b.y,
        a.z * b.x - a.x * b.z,
        a.x * b.y - a.y * b.x
    };
}

float vector3_dot(Vector3 a, Vector3 b) {
    return a.x * b.x + a.y * b.y + a.z * b.z;
}

float vector3_length(Vector3 v) {
    return sqrtf(v.x * v.x + v.y * v.y + v.z * v.z);
}

Vector3 vector3_normalize(Vector3 v) {
    float length = vector3_length(v);
    if (length > EPSILON) {
        return vector3_multiply(v, 1.0f / length);
    }
    return (Vector3){0.0f, 0.0f, 1.0f}; // Default up vector
}

// NURBS primitive creation functions
NURBSSurface* create_nurbs_plane(float width, float height) {
    NURBSSurface *surface = malloc(sizeof(NURBSSurface));
    surface->degree_u = 1;
    surface->degree_v = 1;
    surface->num_control_points_u = 2;
    surface->num_control_points_v = 2;
    
    // Control points for a plane
    surface->control_points[0][0] = (Vector4){-width/2, 0, -height/2, 1.0f};
    surface->control_points[0][1] = (Vector4){-width/2, 0,  height/2, 1.0f};
    surface->control_points[1][0] = (Vector4){ width/2, 0, -height/2, 1.0f};
    surface->control_points[1][1] = (Vector4){ width/2, 0,  height/2, 1.0f};
    
    // Knot vectors for linear surfaces
    surface->knots_u[0] = 0.0f; surface->knots_u[1] = 0.0f;
    surface->knots_u[2] = 1.0f; surface->knots_u[3] = 1.0f;
    surface->num_knots_u = 4;
    
    surface->knots_v[0] = 0.0f; surface->knots_v[1] = 0.0f;
    surface->knots_v[2] = 1.0f; surface->knots_v[3] = 1.0f;
    surface->num_knots_v = 4;
    
    return surface;
}

NURBSSurface* create_nurbs_sphere(float radius) {
    NURBSSurface *surface = malloc(sizeof(NURBSSurface));
    surface->degree_u = 2;
    surface->degree_v = 2;
    surface->num_control_points_u = 7;
    surface->num_control_points_v = 5;
    
    // Simplified sphere control points (quarter sphere)
    // Create control points for a sphere using rational Bézier patches
    for (int i = 0; i < surface->num_control_points_u; i++) {
        for (int j = 0; j < surface->num_control_points_v; j++) {
            float u = (float)i / (surface->num_control_points_u - 1) * M_PI;
            float v = (float)j / (surface->num_control_points_v - 1) * 2.0f * M_PI;
            
            surface->control_points[i][j] = (Vector4){
                radius * sinf(u) * cosf(v),
                radius * cosf(u),
                radius * sinf(u) * sinf(v),
                1.0f
            };
        }
    }
    
    // Set up knot vectors for periodic surface
    surface->num_knots_u = surface->num_control_points_u + surface->degree_u + 1;
    surface->num_knots_v = surface->num_control_points_v + surface->degree_v + 1;
    
    for (int i = 0; i < surface->num_knots_u; i++) {
        surface->knots_u[i] = (float)i / (surface->num_knots_u - 1);
    }
    for (int i = 0; i < surface->num_knots_v; i++) {
        surface->knots_v[i] = (float)i / (surface->num_knots_v - 1);
    }
    
    return surface;
}

NURBSSurface* create_nurbs_cylinder(float radius, float height) {
    NURBSSurface *surface = malloc(sizeof(NURBSSurface));
    surface->degree_u = 2;
    surface->degree_v = 1;
    surface->num_control_points_u = 9; // For full circle
    surface->num_control_points_v = 2; // For height
    
    // Create cylindrical control points
    for (int i = 0; i < surface->num_control_points_u; i++) {
        float angle = (float)i / (surface->num_control_points_u - 1) * 2.0f * M_PI;
        float weight = (i % 2 == 0) ? 1.0f : 1.0f / sqrtf(2.0f); // Rational weights for circle
        
        // Bottom circle
        surface->control_points[i][0] = (Vector4){
            radius * cosf(angle),
            -height / 2.0f,
            radius * sinf(angle),
            weight
        };
        
        // Top circle
        surface->control_points[i][1] = (Vector4){
            radius * cosf(angle),
            height / 2.0f,
            radius * sinf(angle),
            weight
        };
    }
    
    // Set up knot vectors
    surface->num_knots_u = surface->num_control_points_u + surface->degree_u + 1;
    surface->num_knots_v = surface->num_control_points_v + surface->degree_v + 1;
    
    // U direction (circular)
    for (int i = 0; i < surface->num_knots_u; i++) {
        surface->knots_u[i] = (float)i / (surface->num_knots_u - 1);
    }
    
    // V direction (linear)
    surface->knots_v[0] = 0.0f; surface->knots_v[1] = 0.0f;
    surface->knots_v[2] = 1.0f; surface->knots_v[3] = 1.0f;
    
    return surface;
}

NURBSSurface* create_nurbs_torus(float major_radius, float minor_radius) {
    NURBSSurface *surface = malloc(sizeof(NURBSSurface));
    surface->degree_u = 2;
    surface->degree_v = 2;
    surface->num_control_points_u = 9; // For major circle
    surface->num_control_points_v = 9; // For minor circle
    
    // Create toroidal control points
    for (int i = 0; i < surface->num_control_points_u; i++) {
        float u_angle = (float)i / (surface->num_control_points_u - 1) * 2.0f * M_PI;
        float u_weight = (i % 2 == 0) ? 1.0f : 1.0f / sqrtf(2.0f);
        
        for (int j = 0; j < surface->num_control_points_v; j++) {
            float v_angle = (float)j / (surface->num_control_points_v - 1) * 2.0f * M_PI;
            float v_weight = (j % 2 == 0) ? 1.0f : 1.0f / sqrtf(2.0f);
            
            float center_x = (major_radius + minor_radius * cosf(v_angle)) * cosf(u_angle);
            float center_y = minor_radius * sinf(v_angle);
            float center_z = (major_radius + minor_radius * cosf(v_angle)) * sinf(u_angle);
            
            surface->control_points[i][j] = (Vector4){
                center_x, center_y, center_z,
                u_weight * v_weight
            };
        }
    }
    
    // Set up knot vectors (both periodic)
    surface->num_knots_u = surface->num_control_points_u + surface->degree_u + 1;
    surface->num_knots_v = surface->num_control_points_v + surface->degree_v + 1;
    
    for (int i = 0; i < surface->num_knots_u; i++) {
        surface->knots_u[i] = (float)i / (surface->num_knots_u - 1);
    }
    for (int i = 0; i < surface->num_knots_v; i++) {
        surface->knots_v[i] = (float)i / (surface->num_knots_v - 1);
    }
    
    return surface;
}

// Memory management
void free_tessellated_surface(TessellatedSurface *surface) {
    if (surface) {
        if (surface->points) free(surface->points);
        if (surface->indices) free(surface->indices);
        glDeleteVertexArrays(1, &surface->vao);
        glDeleteBuffers(1, &surface->vbo);
        glDeleteBuffers(1, &surface->ebo);
        free(surface);
    }
}

void free_nurbs_surface(NURBSSurface *surface) {
    if (surface) {
        free(surface);
    }
}