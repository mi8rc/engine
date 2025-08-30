#ifndef NURBS_H
#define NURBS_H

#include "main.h"

// NURBS data structures
typedef struct {
    float x, y, z, w; // Homogeneous coordinates
} ControlPoint;

typedef struct {
    int degree;
    int num_control_points;
    int num_knots;
    ControlPoint *control_points;
    float *knots;
    
    // Rendering data
    GLuint vao, vbo;
    int num_vertices;
    float *vertices;
    gboolean dirty; // Needs re-tessellation
    
} NurbsCurve;

typedef struct {
    int degree_u, degree_v;
    int num_control_points_u, num_control_points_v;
    int num_knots_u, num_knots_v;
    ControlPoint **control_points; // 2D array [u][v]
    float *knots_u;
    float *knots_v;
    
    // Rendering data
    GLuint vao, vbo, ebo;
    int num_vertices;
    int num_indices;
    float *vertices;
    unsigned int *indices;
    gboolean dirty; // Needs re-tessellation
    
} NurbsSurface;

typedef enum {
    OBJECT_CURVE,
    OBJECT_SURFACE,
    OBJECT_GROUP
} ObjectType;

typedef struct MapObject {
    ObjectType type;
    char *name;
    gboolean visible;
    gboolean selected;
    
    // Transform
    float position[3];
    float rotation[3];
    float scale[3];
    
    // Object data
    union {
        NurbsCurve *curve;
        NurbsSurface *surface;
        GList *children; // For groups
    } data;
    
    struct MapObject *parent;
    
} MapObject;

// Function declarations
NurbsCurve *nurbs_curve_new(int degree, int num_control_points);
void nurbs_curve_free(NurbsCurve *curve);
void nurbs_curve_set_control_point(NurbsCurve *curve, int index, float x, float y, float z, float w);
void nurbs_curve_set_knot(NurbsCurve *curve, int index, float value);
void nurbs_curve_tessellate(NurbsCurve *curve, int resolution);
void nurbs_curve_render(NurbsCurve *curve);

NurbsSurface *nurbs_surface_new(int degree_u, int degree_v, int num_u, int num_v);
void nurbs_surface_free(NurbsSurface *surface);
void nurbs_surface_set_control_point(NurbsSurface *surface, int u, int v, float x, float y, float z, float w);
void nurbs_surface_set_knot_u(NurbsSurface *surface, int index, float value);
void nurbs_surface_set_knot_v(NurbsSurface *surface, int index, float value);
void nurbs_surface_tessellate(NurbsSurface *surface, int resolution_u, int resolution_v);
void nurbs_surface_render(NurbsSurface *surface);

MapObject *map_object_new(ObjectType type, const char *name);
void map_object_free(MapObject *object);
void map_object_add_child(MapObject *parent, MapObject *child);
void map_object_remove_child(MapObject *parent, MapObject *child);

// NURBS evaluation functions
void evaluate_nurbs_curve(NurbsCurve *curve, float t, float *point);
void evaluate_nurbs_surface(NurbsSurface *surface, float u, float v, float *point);
float nurbs_basis_function(int i, int degree, float t, float *knots);

// Utility functions
void generate_uniform_knots(float *knots, int num_knots, int degree);
void calculate_curve_bounds(NurbsCurve *curve, float *min_bounds, float *max_bounds);
void calculate_surface_bounds(NurbsSurface *surface, float *min_bounds, float *max_bounds);

#endif // NURBS_H