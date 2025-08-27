/*
 * NURBS Primitives Implementation
 * Basic NURBS shape creation functions for the map editor
 */

#include "win_compat.h"
#include "nurbs_primitives.h"
#include <stdlib.h>
#include <string.h>
#include <math.h>
#include <stdbool.h>

// Helper macros
#ifndef M_PI
#define M_PI 3.14159265358979323846
#endif

// Basic primitive creation functions

NURBSSurface* nurbs_create_sphere(float radius) {
    if (radius <= 0.0f) return NULL;
    
    NURBSSurface *surface = calloc(1, sizeof(NURBSSurface));
    if (!surface) return NULL;
    
    // Create a NURBS sphere using standard parameterization
    // This is a simplified implementation - a full sphere would require more complex math
    surface->degree_u = 2;
    surface->degree_v = 2;
    surface->num_control_points_u = 7; // For a full circle in U
    surface->num_control_points_v = 5; // For a semicircle in V
    
    // Generate open uniform knot vectors
    surface->num_knots_u = nurbs_generate_open_uniform_knots(surface->degree_u, 
                                                           surface->num_control_points_u, 
                                                           surface->knots_u);
    surface->num_knots_v = nurbs_generate_open_uniform_knots(surface->degree_v, 
                                                           surface->num_control_points_v, 
                                                           surface->knots_v);
    
    // TODO: Calculate proper control points for sphere
    // For now, create a simple grid
    for (int j = 0; j < surface->num_control_points_v; j++) {
        for (int i = 0; i < surface->num_control_points_u; i++) {
            float u = (float)i / (surface->num_control_points_u - 1);
            float v = (float)j / (surface->num_control_points_v - 1);
            
            float theta = u * 2.0f * M_PI;
            float phi = v * M_PI;
            
            surface->control_points[i][j].x = radius * sin(phi) * cos(theta);
            surface->control_points[i][j].y = radius * cos(phi);
            surface->control_points[i][j].z = radius * sin(phi) * sin(theta);
            surface->control_points[i][j].w = 1.0f;
        }
    }
    
    return surface;
}

NURBSSurface* nurbs_create_plane(float width, float height) {
    if (width <= 0.0f || height <= 0.0f) return NULL;
    
    NURBSSurface *surface = calloc(1, sizeof(NURBSSurface));
    if (!surface) return NULL;
    
    // Create a simple bilinear plane
    surface->degree_u = 1;
    surface->degree_v = 1;
    surface->num_control_points_u = 2;
    surface->num_control_points_v = 2;
    
    // Generate knot vectors
    surface->num_knots_u = nurbs_generate_open_uniform_knots(surface->degree_u, 
                                                           surface->num_control_points_u, 
                                                           surface->knots_u);
    surface->num_knots_v = nurbs_generate_open_uniform_knots(surface->degree_v, 
                                                           surface->num_control_points_v, 
                                                           surface->knots_v);
    
    // Set corner control points
    surface->control_points[0][0] = (Vector4){-width/2, 0, -height/2, 1.0f};
    surface->control_points[1][0] = (Vector4){width/2, 0, -height/2, 1.0f};
    surface->control_points[0][1] = (Vector4){-width/2, 0, height/2, 1.0f};
    surface->control_points[1][1] = (Vector4){width/2, 0, height/2, 1.0f};
    
    return surface;
}

NURBSSurface* nurbs_create_cylinder(float radius, float height) {
    if (radius <= 0.0f || height <= 0.0f) return NULL;
    
    NURBSSurface *surface = calloc(1, sizeof(NURBSSurface));
    if (!surface) return NULL;
    
    // Create a cylinder using circular cross-section
    surface->degree_u = 2; // Circular in U direction
    surface->degree_v = 1; // Linear in V direction (height)
    surface->num_control_points_u = 7; // For full circle
    surface->num_control_points_v = 2; // Two levels (bottom and top)
    
    // Generate knot vectors
    surface->num_knots_u = nurbs_generate_open_uniform_knots(surface->degree_u, 
                                                           surface->num_control_points_u, 
                                                           surface->knots_u);
    surface->num_knots_v = nurbs_generate_open_uniform_knots(surface->degree_v, 
                                                           surface->num_control_points_v, 
                                                           surface->knots_v);
    
    // Create control points for cylinder
    for (int j = 0; j < surface->num_control_points_v; j++) {
        float y = (j == 0) ? -height/2 : height/2;
        
        for (int i = 0; i < surface->num_control_points_u; i++) {
            float angle = (float)i / (surface->num_control_points_u - 1) * 2.0f * M_PI;
            
            surface->control_points[i][j].x = radius * cos(angle);
            surface->control_points[i][j].y = y;
            surface->control_points[i][j].z = radius * sin(angle);
            surface->control_points[i][j].w = 1.0f;
        }
    }
    
    return surface;
}

NURBSSurface* nurbs_create_torus(float major_radius, float minor_radius) {
    if (major_radius <= 0.0f || minor_radius <= 0.0f) return NULL;
    
    NURBSSurface *surface = calloc(1, sizeof(NURBSSurface));
    if (!surface) return NULL;
    
    // Create a torus
    surface->degree_u = 2; // Circular in both directions
    surface->degree_v = 2;
    surface->num_control_points_u = 7; // Major circle
    surface->num_control_points_v = 7; // Minor circle
    
    // Generate knot vectors
    surface->num_knots_u = nurbs_generate_open_uniform_knots(surface->degree_u, 
                                                           surface->num_control_points_u, 
                                                           surface->knots_u);
    surface->num_knots_v = nurbs_generate_open_uniform_knots(surface->degree_v, 
                                                           surface->num_control_points_v, 
                                                           surface->knots_v);
    
    // Create control points for torus
    for (int j = 0; j < surface->num_control_points_v; j++) {
        float v_angle = (float)j / (surface->num_control_points_v - 1) * 2.0f * M_PI;
        
        for (int i = 0; i < surface->num_control_points_u; i++) {
            float u_angle = (float)i / (surface->num_control_points_u - 1) * 2.0f * M_PI;
            
            float r = major_radius + minor_radius * cos(v_angle);
            
            surface->control_points[i][j].x = r * cos(u_angle);
            surface->control_points[i][j].y = minor_radius * sin(v_angle);
            surface->control_points[i][j].z = r * sin(u_angle);
            surface->control_points[i][j].w = 1.0f;
        }
    }
    
    return surface;
}

NURBSSurface* nurbs_create_cone(float bottom_radius, float top_radius, float height) {
    if (bottom_radius < 0.0f || top_radius < 0.0f || height <= 0.0f) return NULL;
    if (bottom_radius == 0.0f && top_radius == 0.0f) return NULL;
    
    NURBSSurface *surface = calloc(1, sizeof(NURBSSurface));
    if (!surface) return NULL;
    
    // Create a cone/truncated cone
    surface->degree_u = 2; // Circular in U direction
    surface->degree_v = 1; // Linear in V direction (height)
    surface->num_control_points_u = 7; // For full circle
    surface->num_control_points_v = 2; // Two levels (bottom and top)
    
    // Generate knot vectors
    surface->num_knots_u = nurbs_generate_open_uniform_knots(surface->degree_u, 
                                                           surface->num_control_points_u, 
                                                           surface->knots_u);
    surface->num_knots_v = nurbs_generate_open_uniform_knots(surface->degree_v, 
                                                           surface->num_control_points_v, 
                                                           surface->knots_v);
    
    // Create control points for cone
    for (int j = 0; j < surface->num_control_points_v; j++) {
        float y = (j == 0) ? -height/2 : height/2;
        float radius = (j == 0) ? bottom_radius : top_radius;
        
        for (int i = 0; i < surface->num_control_points_u; i++) {
            float angle = (float)i / (surface->num_control_points_u - 1) * 2.0f * M_PI;
            
            surface->control_points[i][j].x = radius * cos(angle);
            surface->control_points[i][j].y = y;
            surface->control_points[i][j].z = radius * sin(angle);
            surface->control_points[i][j].w = 1.0f;
        }
    }
    
    return surface;
}

NURBSSurface** nurbs_create_cube(float width, float height, float depth) {
    if (width <= 0.0f || height <= 0.0f || depth <= 0.0f) return NULL;
    
    NURBSSurface **faces = calloc(6, sizeof(NURBSSurface*));
    if (!faces) return NULL;
    
    // Create 6 faces of the cube
    // Front face (Z+)
    faces[0] = nurbs_create_plane(width, height);
    if (faces[0]) {
        nurbs_transform_surface(faces[0], (Vector3){0, 0, depth/2}, (Vector3){0, 0, 0}, (Vector3){1, 1, 1});
    }
    
    // Back face (Z-)
    faces[1] = nurbs_create_plane(width, height);
    if (faces[1]) {
        nurbs_transform_surface(faces[1], (Vector3){0, 0, -depth/2}, (Vector3){0, M_PI, 0}, (Vector3){1, 1, 1});
    }
    
    // Left face (X-)
    faces[2] = nurbs_create_plane(depth, height);
    if (faces[2]) {
        nurbs_transform_surface(faces[2], (Vector3){-width/2, 0, 0}, (Vector3){0, -M_PI/2, 0}, (Vector3){1, 1, 1});
    }
    
    // Right face (X+)
    faces[3] = nurbs_create_plane(depth, height);
    if (faces[3]) {
        nurbs_transform_surface(faces[3], (Vector3){width/2, 0, 0}, (Vector3){0, M_PI/2, 0}, (Vector3){1, 1, 1});
    }
    
    // Top face (Y+)
    faces[4] = nurbs_create_plane(width, depth);
    if (faces[4]) {
        nurbs_transform_surface(faces[4], (Vector3){0, height/2, 0}, (Vector3){M_PI/2, 0, 0}, (Vector3){1, 1, 1});
    }
    
    // Bottom face (Y-)
    faces[5] = nurbs_create_plane(width, depth);
    if (faces[5]) {
        nurbs_transform_surface(faces[5], (Vector3){0, -height/2, 0}, (Vector3){-M_PI/2, 0, 0}, (Vector3){1, 1, 1});
    }
    
    return faces;
}

// Advanced primitive functions (stub implementations)

NURBSSurface* nurbs_create_surface_of_revolution(NURBSCurve *profile_curve, 
                                                Vector3 axis_start, Vector3 axis_end,
                                                float start_angle, float end_angle) {
    (void)profile_curve; (void)axis_start; (void)axis_end; (void)start_angle; (void)end_angle;
    // TODO: Implement surface of revolution
    return NULL;
}

NURBSSurface* nurbs_create_ruled_surface(NURBSCurve *curve1, NURBSCurve *curve2) {
    (void)curve1; (void)curve2;
    // TODO: Implement ruled surface
    return NULL;
}

NURBSSurface* nurbs_create_swept_surface(NURBSCurve *profile_curve, NURBSCurve *path_curve, float scale_factor) {
    (void)profile_curve; (void)path_curve; (void)scale_factor;
    // TODO: Implement swept surface
    return NULL;
}

// Curve primitives

NURBSCurve* nurbs_create_circle_curve(Vector3 center, float radius, Vector3 normal) {
    if (radius <= 0.0f) return NULL;
    
    NURBSCurve *curve = calloc(1, sizeof(NURBSCurve));
    if (!curve) return NULL;
    
    // Create a circular NURBS curve
    curve->degree = 2;
    curve->num_control_points = 7; // For a full circle
    
    // Generate knot vector
    curve->num_knots = nurbs_generate_open_uniform_knots(curve->degree, 
                                                        curve->num_control_points, 
                                                        curve->knots);
    
    // TODO: Calculate proper control points for circle with given normal
    // For now, create a circle in XZ plane
    (void)center; (void)normal; // Suppress warnings
    
    for (int i = 0; i < curve->num_control_points; i++) {
        float angle = (float)i / (curve->num_control_points - 1) * 2.0f * M_PI;
        curve->control_points[i].x = radius * cos(angle);
        curve->control_points[i].y = 0.0f;
        curve->control_points[i].z = radius * sin(angle);
        curve->control_points[i].w = 1.0f;
    }
    
    return curve;
}

NURBSCurve* nurbs_create_line_curve(Vector3 start, Vector3 end) {
    NURBSCurve *curve = calloc(1, sizeof(NURBSCurve));
    if (!curve) return NULL;
    
    // Create a linear NURBS curve
    curve->degree = 1;
    curve->num_control_points = 2;
    
    // Generate knot vector
    curve->num_knots = nurbs_generate_open_uniform_knots(curve->degree, 
                                                        curve->num_control_points, 
                                                        curve->knots);
    
    // Set control points
    curve->control_points[0] = (Vector4){start.x, start.y, start.z, 1.0f};
    curve->control_points[1] = (Vector4){end.x, end.y, end.z, 1.0f};
    
    return curve;
}

NURBSCurve* nurbs_create_arc_curve(Vector3 center, float radius, 
                                  float start_angle, float end_angle, Vector3 normal) {
    (void)center; (void)radius; (void)start_angle; (void)end_angle; (void)normal;
    // TODO: Implement arc curve
    return NULL;
}

NURBSCurve* nurbs_create_ellipse_curve(Vector3 center, float major_radius, float minor_radius, Vector3 normal) {
    (void)center; (void)major_radius; (void)minor_radius; (void)normal;
    // TODO: Implement ellipse curve
    return NULL;
}

// Utility functions

int nurbs_generate_uniform_knots(int degree, int num_control_points, float *knots) {
    if (!knots || degree < 1 || num_control_points < degree + 1) return 0;
    
    int num_knots = num_control_points + degree + 1;
    
    for (int i = 0; i < num_knots; i++) {
        knots[i] = (float)i / (num_knots - 1);
    }
    
    return num_knots;
}

int nurbs_generate_open_uniform_knots(int degree, int num_control_points, float *knots) {
    if (!knots || degree < 1 || num_control_points < degree + 1) return 0;
    
    int num_knots = num_control_points + degree + 1;
    
    // Clamp first degree+1 knots to 0
    for (int i = 0; i <= degree; i++) {
        knots[i] = 0.0f;
    }
    
    // Uniform distribution in the middle
    int middle_knots = num_knots - 2 * (degree + 1);
    for (int i = 0; i < middle_knots; i++) {
        knots[degree + 1 + i] = (float)(i + 1) / (middle_knots + 1);
    }
    
    // Clamp last degree+1 knots to 1
    for (int i = num_knots - degree - 1; i < num_knots; i++) {
        knots[i] = 1.0f;
    }
    
    return num_knots;
}

int nurbs_calculate_arc_control_points(Vector3 center, float radius,
                                      float start_angle, float end_angle, Vector3 normal,
                                      Vector4 *control_points, float *weights) {
    (void)center; (void)radius; (void)start_angle; (void)end_angle; (void)normal;
    (void)control_points; (void)weights;
    // TODO: Implement arc control point calculation
    return 0;
}

void nurbs_transform_surface(NURBSSurface *surface, Vector3 translation, Vector3 rotation, Vector3 scale) {
    if (!surface) return;
    
    // Apply transformation to all control points
    // This is a simplified transformation - proper implementation would use matrices
    for (int j = 0; j < surface->num_control_points_v; j++) {
        for (int i = 0; i < surface->num_control_points_u; i++) {
            Vector4 *cp = &surface->control_points[i][j];
            
            // Apply scaling
            cp->x *= scale.x;
            cp->y *= scale.y;
            cp->z *= scale.z;
            
            // Apply rotation (simplified - just around Y axis for now)
            if (rotation.y != 0.0f) {
                float cos_y = cos(rotation.y);
                float sin_y = sin(rotation.y);
                float new_x = cp->x * cos_y - cp->z * sin_y;
                float new_z = cp->x * sin_y + cp->z * cos_y;
                cp->x = new_x;
                cp->z = new_z;
            }
            
            // Apply translation
            cp->x += translation.x;
            cp->y += translation.y;
            cp->z += translation.z;
        }
    }
}

void nurbs_transform_curve(NURBSCurve *curve, Vector3 translation, Vector3 rotation, Vector3 scale) {
    if (!curve) return;
    
    // Apply transformation to all control points
    for (int i = 0; i < curve->num_control_points; i++) {
        Vector4 *cp = &curve->control_points[i];
        
        // Apply scaling
        cp->x *= scale.x;
        cp->y *= scale.y;
        cp->z *= scale.z;
        
        // Apply rotation (simplified - just around Y axis for now)
        if (rotation.y != 0.0f) {
            float cos_y = cos(rotation.y);
            float sin_y = sin(rotation.y);
            float new_x = cp->x * cos_y - cp->z * sin_y;
            float new_z = cp->x * sin_y + cp->z * cos_y;
            cp->x = new_x;
            cp->z = new_z;
        }
        
        // Apply translation
        cp->x += translation.x;
        cp->y += translation.y;
        cp->z += translation.z;
    }
}

// Validation functions

bool nurbs_validate_surface(NURBSSurface *surface) {
    if (!surface) return false;
    
    // Basic validation checks
    if (surface->degree_u < 1 || surface->degree_v < 1) return false;
    if (surface->num_control_points_u < surface->degree_u + 1) return false;
    if (surface->num_control_points_v < surface->degree_v + 1) return false;
    if (surface->num_knots_u != surface->num_control_points_u + surface->degree_u + 1) return false;
    if (surface->num_knots_v != surface->num_control_points_v + surface->degree_v + 1) return false;
    
    // Check knot vector monotonicity
    for (int i = 1; i < surface->num_knots_u; i++) {
        if (surface->knots_u[i] < surface->knots_u[i-1]) return false;
    }
    for (int i = 1; i < surface->num_knots_v; i++) {
        if (surface->knots_v[i] < surface->knots_v[i-1]) return false;
    }
    
    return true;
}

bool nurbs_validate_curve(NURBSCurve *curve) {
    if (!curve) return false;
    
    // Basic validation checks
    if (curve->degree < 1) return false;
    if (curve->num_control_points < curve->degree + 1) return false;
    if (curve->num_knots != curve->num_control_points + curve->degree + 1) return false;
    
    // Check knot vector monotonicity
    for (int i = 1; i < curve->num_knots; i++) {
        if (curve->knots[i] < curve->knots[i-1]) return false;
    }
    
    return true;
}