#ifndef NURBS_H
#define NURBS_H

#include <stdio.h>
#include <stdlib.h>
#include <math.h>

// OpenGL headers - include GLEW first to avoid conflicts
#ifdef LINUX
    #include <GL/glew.h>
    #include <GL/gl.h>
    #include <GL/glu.h>
#elif defined(WIN32) || defined(_WIN32)
    #include <windows.h>
    #include <GL/glew.h>
    #include <GL/gl.h>
    #include <GL/glu.h>
#else
    #include <OpenGL/gl.h>
    #include <OpenGL/glu.h>
    #include <OpenGL/glext.h>
#endif

// OpenGL 3.0+ compatibility for NURBS functions
#ifndef GL_VERSION_3_0

// Define APIENTRY if not available
#ifndef APIENTRY
#ifdef _WIN32
#define APIENTRY __stdcall
#else
#define APIENTRY
#endif
#endif

// Define OpenGL types if not available
#ifndef GL_VERSION_1_5
#if !defined(GLsizeiptr)
#if defined(_WIN64) || defined(__LP64__)
typedef long long GLsizeiptr;
typedef long long GLintptr;
#else
typedef long GLsizeiptr;
typedef long GLintptr;
#endif
#endif
#endif

// Function pointer types for VAO and VBO functions
typedef void (APIENTRY *PFNGLGENVERTEXARRAYSPROC)(GLsizei n, GLuint *arrays);
typedef void (APIENTRY *PFNGLBINDVERTEXARRAYPROC)(GLuint array);
typedef void (APIENTRY *PFNGLDELETEVERTEXARRAYSPROC)(GLsizei n, const GLuint *arrays);
typedef void (APIENTRY *PFNGLGENBUFFERSPROC)(GLsizei n, GLuint *buffers);
typedef void (APIENTRY *PFNGLBINDBUFFERPROC)(GLenum target, GLuint buffer);
typedef void (APIENTRY *PFNGLBUFFERDATAPROC)(GLenum target, GLsizeiptr size, const void *data, GLenum usage);
typedef void (APIENTRY *PFNGLDELETEBUFFERSPROC)(GLsizei n, const GLuint *buffers);
typedef void (APIENTRY *PFNGLVERTEXATTRIBPOINTERPROC)(GLuint index, GLint size, GLenum type, GLboolean normalized, GLsizei stride, const void *pointer);
typedef void (APIENTRY *PFNGLENABLEVERTEXATTRIBARRAYPROC)(GLuint index);

// External function pointers (defined in fps_engine.c)
extern PFNGLGENVERTEXARRAYSPROC glGenVertexArrays;
extern PFNGLBINDVERTEXARRAYPROC glBindVertexArray;
extern PFNGLDELETEVERTEXARRAYSPROC glDeleteVertexArrays;
extern PFNGLGENBUFFERSPROC glGenBuffers;
extern PFNGLBINDBUFFERPROC glBindBuffer;
extern PFNGLBUFFERDATAPROC glBufferData;
extern PFNGLDELETEBUFFERSPROC glDeleteBuffers;
extern PFNGLVERTEXATTRIBPOINTERPROC glVertexAttribPointer;
extern PFNGLENABLEVERTEXATTRIBARRAYPROC glEnableVertexAttribArray;

// GL constants
#ifndef GL_ARRAY_BUFFER
#define GL_ARRAY_BUFFER 0x8892
#endif
#ifndef GL_ELEMENT_ARRAY_BUFFER
#define GL_ELEMENT_ARRAY_BUFFER 0x8893
#endif
#ifndef GL_STATIC_DRAW
#define GL_STATIC_DRAW 0x88E4
#endif
#endif

#define MAX_CONTROL_POINTS 64
#define MAX_KNOTS 128
#define EPSILON 1e-6

// Vector3 structure for 3D points and vectors
typedef struct {
    float x, y, z;
} Vector3;

// Vector4 structure for homogeneous coordinates
typedef struct {
    float x, y, z, w;
} Vector4;

// NURBS curve structure
typedef struct {
    int degree;                           // Degree of the curve
    int num_control_points;              // Number of control points
    Vector4 control_points[MAX_CONTROL_POINTS]; // Control points (homogeneous)
    float knots[MAX_KNOTS];              // Knot vector
    int num_knots;                       // Number of knots
} NURBSCurve;

// NURBS surface structure
typedef struct {
    int degree_u, degree_v;              // Degrees in u and v directions
    int num_control_points_u, num_control_points_v; // Control point counts
    Vector4 control_points[MAX_CONTROL_POINTS][MAX_CONTROL_POINTS]; // Control net
    float knots_u[MAX_KNOTS], knots_v[MAX_KNOTS]; // Knot vectors
    int num_knots_u, num_knots_v;        // Knot counts
} NURBSSurface;

// Surface point with normal for rendering
typedef struct {
    Vector3 position;
    Vector3 normal;
    Vector3 tangent_u, tangent_v;        // Tangent vectors for lighting
} SurfacePoint;

// Tessellated surface for rendering
typedef struct {
    SurfacePoint *points;                // Grid of surface points
    int resolution_u, resolution_v;      // Tessellation resolution
    unsigned int *indices;               // Triangle indices
    int num_triangles;                   // Number of triangles
    unsigned int vao, vbo, ebo;          // OpenGL buffer objects
} TessellatedSurface;

// Light structure
typedef struct {
    Vector3 position;
    Vector3 color;
    float intensity;
    int type; // 0 = point, 1 = directional, 2 = spot
    Vector3 direction; // For directional and spot lights
    float spot_angle; // For spot lights
} Light;

// Material properties
typedef struct {
    Vector3 ambient;
    Vector3 diffuse;
    Vector3 specular;
    float shininess;
} Material;

// Function declarations
float nurbs_basis_function(int i, int degree, float t, float *knots);
Vector3 evaluate_nurbs_curve(NURBSCurve *curve, float t);
Vector3 evaluate_nurbs_curve_derivative(NURBSCurve *curve, float t, int order);
SurfacePoint evaluate_nurbs_surface(NURBSSurface *surface, float u, float v);
TessellatedSurface* tessellate_nurbs_surface(NURBSSurface *surface, int res_u, int res_v);
void calculate_surface_normal(NURBSSurface *surface, float u, float v, Vector3 *normal);
void render_tessellated_surface(TessellatedSurface *surface, Light *lights, int num_lights, Material *material);

// Collision detection
typedef struct {
    Vector3 point;
    Vector3 normal;
    float distance;
    int hit;
} CollisionResult;

CollisionResult ray_nurbs_surface_intersection(Vector3 ray_origin, Vector3 ray_direction, NURBSSurface *surface);
int point_inside_nurbs_volume(Vector3 point, NURBSSurface **surfaces, int num_surfaces);

// Utility functions
Vector3 vector3_add(Vector3 a, Vector3 b);
Vector3 vector3_subtract(Vector3 a, Vector3 b);
Vector3 vector3_multiply(Vector3 v, float scalar);
Vector3 vector3_cross(Vector3 a, Vector3 b);
float vector3_dot(Vector3 a, Vector3 b);
Vector3 vector3_normalize(Vector3 v);
float vector3_length(Vector3 v);

// NURBS creation helpers
NURBSSurface* create_nurbs_plane(float width, float height);
NURBSSurface* create_nurbs_sphere(float radius);
NURBSSurface* create_nurbs_cylinder(float radius, float height);
NURBSSurface* create_nurbs_torus(float major_radius, float minor_radius);

// Memory management
void free_tessellated_surface(TessellatedSurface *surface);
void free_nurbs_surface(NURBSSurface *surface);

#endif // NURBS_H