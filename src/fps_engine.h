#ifndef FPS_ENGINE_H
#define FPS_ENGINE_H

#include "nurbs.h"
#include <GLFW/glfw3.h>
#ifdef _WIN32
    #include <windows.h>
#endif
#include <GL/gl.h>
#include <GL/glu.h>
#include <stdbool.h>

// OpenGL 3.0+ function declarations for compatibility
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

// Function pointer types
typedef void (APIENTRY *PFNGLGENVERTEXARRAYSPROC)(GLsizei n, GLuint *arrays);
typedef void (APIENTRY *PFNGLBINDVERTEXARRAYPROC)(GLuint array);
typedef void (APIENTRY *PFNGLDELETEVERTEXARRAYSPROC)(GLsizei n, const GLuint *arrays);
typedef void (APIENTRY *PFNGLGENBUFFERSPROC)(GLsizei n, GLuint *buffers);
typedef void (APIENTRY *PFNGLBINDBUFFERPROC)(GLenum target, GLuint buffer);
typedef void (APIENTRY *PFNGLBUFFERDATAPROC)(GLenum target, GLsizeiptr size, const void *data, GLenum usage);
typedef void (APIENTRY *PFNGLDELETEBUFFERSPROC)(GLsizei n, const GLuint *buffers);
typedef void (APIENTRY *PFNGLVERTEXATTRIBPOINTERPROC)(GLuint index, GLint size, GLenum type, GLboolean normalized, GLsizei stride, const void *pointer);
typedef void (APIENTRY *PFNGLENABLEVERTEXATTRIBARRAYPROC)(GLuint index);

// Function pointer declarations
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

#define MAX_OBJECTS 256
#define MAX_LIGHTS 32

// Camera structure for FPS controls
typedef struct {
    Vector3 position;
    Vector3 front;
    Vector3 up;
    Vector3 right;
    Vector3 world_up;
    
    float yaw;
    float pitch;
    
    float movement_speed;
    float mouse_sensitivity;
    float zoom;
    
    bool first_mouse;
    float last_x, last_y;
} Camera;

// Game object containing NURBS surfaces
typedef struct {
    NURBSSurface **surfaces;
    TessellatedSurface **tessellated_surfaces;
    int num_surfaces;
    Vector3 position;
    Vector3 rotation;
    Vector3 scale;
    Material material;
    bool is_collidable;
    char name[64];
} GameObject;

// Game world structure
typedef struct {
    GameObject objects[MAX_OBJECTS];
    int num_objects;
    Light lights[MAX_LIGHTS];
    int num_lights;
    Vector3 ambient_light;
} World;

// Input state
typedef struct {
    bool keys[1024];
    bool keys_processed[1024];
    float mouse_x, mouse_y;
    float delta_time;
    float last_frame;
} InputState;

// Shader program structure
typedef struct {
    unsigned int program_id;
    int position_loc;
    int normal_loc;
    int model_loc;
    int view_loc;
    int projection_loc;
    int light_pos_loc[MAX_LIGHTS];
    int light_color_loc[MAX_LIGHTS];
    int light_intensity_loc[MAX_LIGHTS];
    int num_lights_loc;
    int material_ambient_loc;
    int material_diffuse_loc;
    int material_specular_loc;
    int material_shininess_loc;
    int camera_pos_loc;
} ShaderProgram;

// Game engine structure
typedef struct {
    GLFWwindow *window;
    Camera camera;
    World world;
    InputState input;
    ShaderProgram shader;
    int screen_width, screen_height;
    bool running;
} FPSEngine;

// Function declarations
int load_opengl_extensions(void);
int fps_engine_init(FPSEngine *engine, int width, int height, const char *title);
void fps_engine_cleanup(FPSEngine *engine);
void fps_engine_run(FPSEngine *engine);
void fps_engine_update(FPSEngine *engine);
void fps_engine_render(FPSEngine *engine);
void fps_engine_handle_input(FPSEngine *engine);

// Camera functions
void camera_init(Camera *camera, Vector3 position, Vector3 up, float yaw, float pitch);
void camera_update_vectors(Camera *camera);
void camera_process_keyboard(Camera *camera, int direction, float delta_time);
void camera_process_mouse_movement(Camera *camera, float xoffset, float yoffset, bool constrain_pitch);
void camera_process_mouse_scroll(Camera *camera, float yoffset);
void camera_get_view_matrix(Camera *camera, float *matrix);

// World management
void world_init(World *world);
int world_add_object(World *world, GameObject *object);
int world_add_light(World *world, Light *light);
void world_update(World *world, float delta_time);
void world_render(World *world, ShaderProgram *shader, Camera *camera);

// GameObject functions
GameObject* create_game_object(const char *name, Vector3 position);
void game_object_add_nurbs_surface(GameObject *object, NURBSSurface *surface);
void game_object_set_material(GameObject *object, Material material);
void game_object_update_transform(GameObject *object);
void game_object_render(GameObject *object, ShaderProgram *shader);

// Collision system
bool check_player_collision(FPSEngine *engine, Vector3 new_position);
Vector3 resolve_collision(Vector3 current_pos, Vector3 desired_pos, GameObject *objects, int num_objects);

// Shader management
int load_shader_program(ShaderProgram *program, const char *vertex_source, const char *fragment_source);
void use_shader_program(ShaderProgram *program);
void set_uniform_matrix4(int location, float *matrix);
void set_uniform_vector3(int location, Vector3 vector);
void set_uniform_float(int location, float value);
void set_uniform_int(int location, int value);

// Callback functions
void mouse_callback(GLFWwindow *window, double xpos, double ypos);
void scroll_callback(GLFWwindow *window, double xoffset, double yoffset);
void key_callback(GLFWwindow *window, int key, int scancode, int action, int mods);
void framebuffer_size_callback(GLFWwindow *window, int width, int height);

// Matrix operations
void matrix_identity(float *matrix);
void matrix_perspective(float *matrix, float fov, float aspect, float near, float far);
void matrix_look_at(float *matrix, Vector3 eye, Vector3 center, Vector3 up);
void matrix_translate(float *matrix, Vector3 translation);
void matrix_rotate(float *matrix, float angle, Vector3 axis);
void matrix_scale(float *matrix, Vector3 scale);
void matrix_multiply(float *result, float *a, float *b);

// Movement directions for camera
enum {
    CAMERA_FORWARD,
    CAMERA_BACKWARD,
    CAMERA_LEFT,
    CAMERA_RIGHT,
    CAMERA_UP,
    CAMERA_DOWN
};

#endif // FPS_ENGINE_H