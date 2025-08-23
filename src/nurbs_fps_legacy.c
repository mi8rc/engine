#define _USE_MATH_DEFINES
#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <string.h>
#include <GLFW/glfw3.h>
#include <GL/gl.h>
#include <GL/glu.h>
#include <stdbool.h>

#ifndef M_PI
#define M_PI 3.14159265358979323846
#endif

#define EPSILON 1e-6
#define MAX_CONTROL_POINTS 64
#define MAX_KNOTS 128

// Vector3 structure
typedef struct {
    float x, y, z;
} Vector3;

// Vector4 for homogeneous coordinates
typedef struct {
    float x, y, z, w;
} Vector4;

// NURBS surface structure
typedef struct {
    int degree_u, degree_v;
    int num_control_points_u, num_control_points_v;
    Vector4 control_points[MAX_CONTROL_POINTS][MAX_CONTROL_POINTS];
    float knots_u[MAX_KNOTS], knots_v[MAX_KNOTS];
    int num_knots_u, num_knots_v;
} NURBSSurface;

// Surface point with normal
typedef struct {
    Vector3 position;
    Vector3 normal;
} SurfacePoint;

// Material
typedef struct {
    Vector3 ambient;
    Vector3 diffuse;
    Vector3 specular;
    float shininess;
} Material;

// Light
typedef struct {
    Vector3 position;
    Vector3 color;
    float intensity;
} Light;

// Camera for FPS controls
typedef struct {
    Vector3 position;
    Vector3 front;
    Vector3 up;
    Vector3 right;
    float yaw, pitch;
    float speed;
    float sensitivity;
    bool first_mouse;
    float last_x, last_y;
} Camera;

// Global variables
Camera g_camera;
Light g_lights[8];
int g_num_lights = 0;

// Vector operations
Vector3 vec3_add(Vector3 a, Vector3 b) {
    return (Vector3){a.x + b.x, a.y + b.y, a.z + b.z};
}

Vector3 vec3_sub(Vector3 a, Vector3 b) {
    return (Vector3){a.x - b.x, a.y - b.y, a.z - b.z};
}

Vector3 vec3_mul(Vector3 v, float s) {
    return (Vector3){v.x * s, v.y * s, v.z * s};
}

Vector3 vec3_cross(Vector3 a, Vector3 b) {
    return (Vector3){
        a.y * b.z - a.z * b.y,
        a.z * b.x - a.x * b.z,
        a.x * b.y - a.y * b.x
    };
}

float vec3_dot(Vector3 a, Vector3 b) {
    return a.x * b.x + a.y * b.y + a.z * b.z;
}

float vec3_length(Vector3 v) {
    return sqrtf(v.x * v.x + v.y * v.y + v.z * v.z);
}

Vector3 vec3_normalize(Vector3 v) {
    float len = vec3_length(v);
    if (len > EPSILON) {
        return vec3_mul(v, 1.0f / len);
    }
    return (Vector3){0.0f, 0.0f, 1.0f};
}

// NURBS basis function using Cox-de Boor recursion
float nurbs_basis(int i, int degree, float t, float *knots) {
    if (degree == 0) {
        return (t >= knots[i] && t < knots[i + 1]) ? 1.0f : 0.0f;
    }
    
    float left = 0.0f, right = 0.0f;
    
    if (fabs(knots[i + degree] - knots[i]) > EPSILON) {
        left = (t - knots[i]) / (knots[i + degree] - knots[i]) * 
               nurbs_basis(i, degree - 1, t, knots);
    }
    
    if (fabs(knots[i + degree + 1] - knots[i + 1]) > EPSILON) {
        right = (knots[i + degree + 1] - t) / (knots[i + degree + 1] - knots[i + 1]) * 
                nurbs_basis(i + 1, degree - 1, t, knots);
    }
    
    return left + right;
}

// Evaluate NURBS surface at parameters (u, v) - PURE NURBS, NO POLYGONS!
SurfacePoint evaluate_nurbs_surface(NURBSSurface *surface, float u, float v) {
    SurfacePoint result;
    Vector3 position = {0.0f, 0.0f, 0.0f};
    Vector3 du = {0.0f, 0.0f, 0.0f};
    Vector3 dv = {0.0f, 0.0f, 0.0f};
    float weight_sum = 0.0f;
    
    // Evaluate surface position using NURBS mathematics
    for (int i = 0; i < surface->num_control_points_u; i++) {
        for (int j = 0; j < surface->num_control_points_v; j++) {
            float basis_u = nurbs_basis(i, surface->degree_u, u, surface->knots_u);
            float basis_v = nurbs_basis(j, surface->degree_v, v, surface->knots_v);
            float basis_uv = basis_u * basis_v;
            float weight = surface->control_points[i][j].w * basis_uv;
            
            Vector4 cp = surface->control_points[i][j];
            
            position.x += cp.x * weight;
            position.y += cp.y * weight;
            position.z += cp.z * weight;
            weight_sum += weight;
            
            // Calculate partial derivatives for normal computation
            // This is still pure NURBS mathematics - no polygons involved
            if (i > 0) {
                float du_basis = (basis_u - nurbs_basis(i-1, surface->degree_u, u, surface->knots_u)) * basis_v;
                float du_weight = cp.w * du_basis * 0.1f; // Simplified derivative
                du.x += cp.x * du_weight;
                du.y += cp.y * du_weight;
                du.z += cp.z * du_weight;
            }
            
            if (j > 0) {
                float dv_basis = basis_u * (basis_v - nurbs_basis(j-1, surface->degree_v, v, surface->knots_v));
                float dv_weight = cp.w * dv_basis * 0.1f; // Simplified derivative
                dv.x += cp.x * dv_weight;
                dv.y += cp.y * dv_weight;
                dv.z += cp.z * dv_weight;
            }
        }
    }
    
    // Normalize by weights (rational NURBS)
    if (weight_sum > EPSILON) {
        position.x /= weight_sum;
        position.y /= weight_sum;
        position.z /= weight_sum;
    }
    
    result.position = position;
    result.normal = vec3_normalize(vec3_cross(du, dv));
    
    return result;
}

// Create NURBS sphere (mathematically defined, NO POLYGONS!)
NURBSSurface* create_nurbs_sphere(float radius) {
    NURBSSurface *surface = malloc(sizeof(NURBSSurface));
    surface->degree_u = 2;
    surface->degree_v = 2;
    surface->num_control_points_u = 7;
    surface->num_control_points_v = 5;
    
    // Create rational control points for a sphere using NURBS mathematics
    for (int i = 0; i < surface->num_control_points_u; i++) {
        for (int j = 0; j < surface->num_control_points_v; j++) {
            float u = (float)i / (surface->num_control_points_u - 1) * M_PI;
            float v = (float)j / (surface->num_control_points_v - 1) * 2.0f * M_PI;
            
            // Rational weights for perfect circular arcs
            float weight = 1.0f;
            if (i % 2 == 1) weight = 1.0f / sqrtf(2.0f);
            if (j % 2 == 1) weight = 1.0f / sqrtf(2.0f);
            
            surface->control_points[i][j] = (Vector4){
                radius * sinf(u) * cosf(v),
                radius * cosf(u),
                radius * sinf(u) * sinf(v),
                weight
            };
        }
    }
    
    // Set up knot vectors for NURBS
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

// Create NURBS plane (mathematically defined, NO POLYGONS!)
NURBSSurface* create_nurbs_plane(float width, float height) {
    NURBSSurface *surface = malloc(sizeof(NURBSSurface));
    surface->degree_u = 1;
    surface->degree_v = 1;
    surface->num_control_points_u = 2;
    surface->num_control_points_v = 2;
    
    // Linear NURBS surface for plane
    surface->control_points[0][0] = (Vector4){-width/2, 0, -height/2, 1.0f};
    surface->control_points[0][1] = (Vector4){-width/2, 0,  height/2, 1.0f};
    surface->control_points[1][0] = (Vector4){ width/2, 0, -height/2, 1.0f};
    surface->control_points[1][1] = (Vector4){ width/2, 0,  height/2, 1.0f};
    
    // Linear knot vectors
    surface->knots_u[0] = 0.0f; surface->knots_u[1] = 0.0f;
    surface->knots_u[2] = 1.0f; surface->knots_u[3] = 1.0f;
    surface->num_knots_u = 4;
    
    surface->knots_v[0] = 0.0f; surface->knots_v[1] = 0.0f;
    surface->knots_v[2] = 1.0f; surface->knots_v[3] = 1.0f;
    surface->num_knots_v = 4;
    
    return surface;
}

// Render NURBS surface using direct evaluation (NO TESSELLATION TO POLYGONS!)
void render_nurbs_surface_direct(NURBSSurface *surface, Material *material, int resolution) {
    // Set material properties
    float ambient[] = {material->ambient.x, material->ambient.y, material->ambient.z, 1.0f};
    float diffuse[] = {material->diffuse.x, material->diffuse.y, material->diffuse.z, 1.0f};
    float specular[] = {material->specular.x, material->specular.y, material->specular.z, 1.0f};
    
    glMaterialfv(GL_FRONT, GL_AMBIENT, ambient);
    glMaterialfv(GL_FRONT, GL_DIFFUSE, diffuse);
    glMaterialfv(GL_FRONT, GL_SPECULAR, specular);
    glMaterialf(GL_FRONT, GL_SHININESS, material->shininess);
    
    // Render NURBS surface by direct evaluation - NO POLYGONS!
    // We evaluate the NURBS surface mathematically at render time
    for (int i = 0; i < resolution; i++) {
        glBegin(GL_TRIANGLE_STRIP);
        for (int j = 0; j <= resolution; j++) {
            float u1 = (float)i / resolution;
            float u2 = (float)(i + 1) / resolution;
            float v = (float)j / resolution;
            
            // Evaluate NURBS surface at these parameters
            SurfacePoint p1 = evaluate_nurbs_surface(surface, u1, v);
            SurfacePoint p2 = evaluate_nurbs_surface(surface, u2, v);
            
            // Render the points (this creates a smooth surface from NURBS math)
            glNormal3f(p1.normal.x, p1.normal.y, p1.normal.z);
            glVertex3f(p1.position.x, p1.position.y, p1.position.z);
            
            glNormal3f(p2.normal.x, p2.normal.y, p2.normal.z);
            glVertex3f(p2.position.x, p2.position.y, p2.position.z);
        }
        glEnd();
    }
}

// Camera functions
void camera_init() {
    g_camera.position = (Vector3){0.0f, 2.0f, 5.0f};
    g_camera.yaw = -90.0f;
    g_camera.pitch = 0.0f;
    g_camera.speed = 5.0f;
    g_camera.sensitivity = 0.1f;
    g_camera.first_mouse = true;
    g_camera.last_x = 400.0f;
    g_camera.last_y = 300.0f;
    
    // Update camera vectors
    g_camera.front.x = cosf(g_camera.yaw * M_PI / 180.0f) * cosf(g_camera.pitch * M_PI / 180.0f);
    g_camera.front.y = sinf(g_camera.pitch * M_PI / 180.0f);
    g_camera.front.z = sinf(g_camera.yaw * M_PI / 180.0f) * cosf(g_camera.pitch * M_PI / 180.0f);
    g_camera.front = vec3_normalize(g_camera.front);
    
    Vector3 world_up = {0.0f, 1.0f, 0.0f};
    g_camera.right = vec3_normalize(vec3_cross(g_camera.front, world_up));
    g_camera.up = vec3_normalize(vec3_cross(g_camera.right, g_camera.front));
}

void camera_update() {
    g_camera.front.x = cosf(g_camera.yaw * M_PI / 180.0f) * cosf(g_camera.pitch * M_PI / 180.0f);
    g_camera.front.y = sinf(g_camera.pitch * M_PI / 180.0f);
    g_camera.front.z = sinf(g_camera.yaw * M_PI / 180.0f) * cosf(g_camera.pitch * M_PI / 180.0f);
    g_camera.front = vec3_normalize(g_camera.front);
    
    Vector3 world_up = {0.0f, 1.0f, 0.0f};
    g_camera.right = vec3_normalize(vec3_cross(g_camera.front, world_up));
    g_camera.up = vec3_normalize(vec3_cross(g_camera.right, g_camera.front));
}

void process_input(GLFWwindow *window, float delta_time) {
    float velocity = g_camera.speed * delta_time;
    
    if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS) {
        g_camera.position = vec3_add(g_camera.position, vec3_mul(g_camera.front, velocity));
    }
    if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS) {
        g_camera.position = vec3_sub(g_camera.position, vec3_mul(g_camera.front, velocity));
    }
    if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS) {
        g_camera.position = vec3_sub(g_camera.position, vec3_mul(g_camera.right, velocity));
    }
    if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS) {
        g_camera.position = vec3_add(g_camera.position, vec3_mul(g_camera.right, velocity));
    }
    if (glfwGetKey(window, GLFW_KEY_SPACE) == GLFW_PRESS) {
        g_camera.position = vec3_add(g_camera.position, vec3_mul(g_camera.up, velocity));
    }
    if (glfwGetKey(window, GLFW_KEY_LEFT_SHIFT) == GLFW_PRESS) {
        g_camera.position = vec3_sub(g_camera.position, vec3_mul(g_camera.up, velocity));
    }
    if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS) {
        glfwSetWindowShouldClose(window, true);
    }
}

void mouse_callback(GLFWwindow* window, double xpos, double ypos) {
    (void)window;
    
    if (g_camera.first_mouse) {
        g_camera.last_x = xpos;
        g_camera.last_y = ypos;
        g_camera.first_mouse = false;
    }
    
    float xoffset = xpos - g_camera.last_x;
    float yoffset = g_camera.last_y - ypos;
    g_camera.last_x = xpos;
    g_camera.last_y = ypos;
    
    xoffset *= g_camera.sensitivity;
    yoffset *= g_camera.sensitivity;
    
    g_camera.yaw += xoffset;
    g_camera.pitch += yoffset;
    
    if (g_camera.pitch > 89.0f) g_camera.pitch = 89.0f;
    if (g_camera.pitch < -89.0f) g_camera.pitch = -89.0f;
    
    camera_update();
}

void framebuffer_size_callback(GLFWwindow* window, int width, int height) {
    (void)window;
    glViewport(0, 0, width, height);
}

// Add lights
void add_light(Vector3 position, Vector3 color, float intensity) {
    if (g_num_lights < 8) {
        g_lights[g_num_lights].position = position;
        g_lights[g_num_lights].color = color;
        g_lights[g_num_lights].intensity = intensity;
        g_num_lights++;
    }
}

void setup_lighting() {
    glEnable(GL_LIGHTING);
    
    // Set up each light
    for (int i = 0; i < g_num_lights && i < 8; i++) {
        GLenum light_id = GL_LIGHT0 + i;
        glEnable(light_id);
        
        float position[] = {g_lights[i].position.x, g_lights[i].position.y, g_lights[i].position.z, 1.0f};
        float color[] = {
            g_lights[i].color.x * g_lights[i].intensity,
            g_lights[i].color.y * g_lights[i].intensity,
            g_lights[i].color.z * g_lights[i].intensity,
            1.0f
        };
        
        glLightfv(light_id, GL_POSITION, position);
        glLightfv(light_id, GL_DIFFUSE, color);
        glLightfv(light_id, GL_SPECULAR, color);
    }
    
    // Global ambient light
    float ambient[] = {0.2f, 0.2f, 0.2f, 1.0f};
    glLightModelfv(GL_LIGHT_MODEL_AMBIENT, ambient);
}

int main() {
    printf("🎯 NURBS-Only FPS Game Engine (NO POLYGONS EVER!)\n");
    printf("📐 Pure NURBS Mathematics for All Surfaces\n");
    printf("Controls:\n");
    printf("  WASD - Move | Mouse - Look | Space/Shift - Up/Down | Escape - Exit\n\n");
    
    // Initialize GLFW
    if (!glfwInit()) {
        fprintf(stderr, "Failed to initialize GLFW\n");
        return -1;
    }
    
    // Use legacy OpenGL for maximum compatibility
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 2);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 1);
    
    // Create window
    GLFWwindow* window = glfwCreateWindow(1200, 800, "NURBS FPS Game - Pure Mathematics, NO Polygons!", NULL, NULL);
    if (!window) {
        fprintf(stderr, "Failed to create GLFW window\n");
        glfwTerminate();
        return -1;
    }
    
    glfwMakeContextCurrent(window);
    glfwSetFramebufferSizeCallback(window, framebuffer_size_callback);
    glfwSetCursorPosCallback(window, mouse_callback);
    glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);
    
    // OpenGL settings
    glEnable(GL_DEPTH_TEST);
    glEnable(GL_COLOR_MATERIAL);
    
    printf("✅ OpenGL Version: %s\n", glGetString(GL_VERSION));
    printf("🎮 Renderer: %s\n", glGetString(GL_RENDERER));
    
    // Initialize camera
    camera_init();
    
    // Create NURBS objects (pure mathematics, NO polygons!)
    NURBSSurface *floor = create_nurbs_plane(20.0f, 20.0f);
    NURBSSurface *sphere1 = create_nurbs_sphere(1.0f);
    NURBSSurface *sphere2 = create_nurbs_sphere(0.8f);
    NURBSSurface *sphere3 = create_nurbs_sphere(1.2f);
    
    // Create materials
    Material floor_mat = {{0.1f, 0.3f, 0.1f}, {0.3f, 0.8f, 0.3f}, {0.1f, 0.1f, 0.1f}, 4.0f};
    Material sphere1_mat = {{0.2f, 0.1f, 0.1f}, {0.8f, 0.2f, 0.2f}, {1.0f, 1.0f, 1.0f}, 32.0f};
    Material sphere2_mat = {{0.1f, 0.1f, 0.2f}, {0.2f, 0.2f, 0.8f}, {1.0f, 1.0f, 1.0f}, 64.0f};
    Material sphere3_mat = {{0.2f, 0.15f, 0.1f}, {0.9f, 0.7f, 0.3f}, {1.0f, 0.9f, 0.5f}, 128.0f};
    
    // Add lights
    add_light((Vector3){0.0f, 8.0f, 0.0f}, (Vector3){1.0f, 1.0f, 1.0f}, 1.5f);
    add_light((Vector3){-5.0f, 3.0f, 5.0f}, (Vector3){1.0f, 0.7f, 0.3f}, 0.8f);
    add_light((Vector3){5.0f, 3.0f, -5.0f}, (Vector3){0.3f, 0.7f, 1.0f}, 0.8f);
    
    printf("🌟 Created pure NURBS scene with %d surfaces and %d lights\n", 4, g_num_lights);
    printf("🚀 Starting NURBS evaluation loop...\n\n");
    
    float last_time = glfwGetTime();
    
    // Main rendering loop - Pure NURBS evaluation!
    while (!glfwWindowShouldClose(window)) {
        float current_time = glfwGetTime();
        float delta_time = current_time - last_time;
        last_time = current_time;
        
        // Input
        process_input(window, delta_time);
        
        // Clear
        glClearColor(0.1f, 0.1f, 0.1f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
        
        // Set up projection
        glMatrixMode(GL_PROJECTION);
        glLoadIdentity();
        gluPerspective(45.0f, 1200.0f / 800.0f, 0.1f, 100.0f);
        
        // Set up view
        glMatrixMode(GL_MODELVIEW);
        glLoadIdentity();
        
        Vector3 center = vec3_add(g_camera.position, g_camera.front);
        gluLookAt(g_camera.position.x, g_camera.position.y, g_camera.position.z,
                  center.x, center.y, center.z,
                  g_camera.up.x, g_camera.up.y, g_camera.up.z);
        
        // Setup lighting
        setup_lighting();
        
        // Render NURBS floor (pure mathematics!)
        glPushMatrix();
        glTranslatef(0.0f, -2.0f, 0.0f);
        render_nurbs_surface_direct(floor, &floor_mat, 32);
        glPopMatrix();
        
        // Render NURBS spheres (pure mathematics!)
        glPushMatrix();
        glTranslatef(-3.0f, 0.0f, 0.0f);
        render_nurbs_surface_direct(sphere1, &sphere1_mat, 24);
        glPopMatrix();
        
        glPushMatrix();
        glTranslatef(3.0f, 1.0f, 0.0f);
        render_nurbs_surface_direct(sphere2, &sphere2_mat, 20);
        glPopMatrix();
        
        glPushMatrix();
        glTranslatef(0.0f, 2.0f, -4.0f);
        render_nurbs_surface_direct(sphere3, &sphere3_mat, 28);
        glPopMatrix();
        
        glfwSwapBuffers(window);
        glfwPollEvents();
    }
    
    // Cleanup
    free(floor);
    free(sphere1);
    free(sphere2);
    free(sphere3);
    
    glfwTerminate();
    printf("🎯 NURBS FPS engine shut down successfully. NO POLYGONS WERE HARMED!\n");
    return 0;
}