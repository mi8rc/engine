#ifndef FPS_ENGINE_H
#define FPS_ENGINE_H

#include "nurbs.h"
#include <GLFW/glfw3.h>
// Remove legacy OpenGL headers and use GLFW's OpenGL headers instead
// #include <GL/gl.h>
// #include <GL/glu.h>
#include <stdbool.h>

// OpenGL function pointer type definitions for modern OpenGL functions
// These are needed for Windows compilation with GCC
typedef void (APIENTRYP PFNGLGENVERTEXARRAYSPROC) (GLsizei n, GLuint *arrays);
typedef void (APIENTRYP PFNGLBINDVERTEXARRAYPROC) (GLuint array);
typedef void (APIENTRYP PFNGLDELETEVERTEXARRAYSPROC) (GLsizei n, const GLuint *arrays);
typedef void (APIENTRYP PFNGLGENBUFFERSPROC) (GLsizei n, GLuint *buffers);
typedef void (APIENTRYP PFNGLBINDBUFFERPROC) (GLenum target, GLuint buffer);
typedef void (APIENTRYP PFNGLBUFFERDATAPROC) (GLenum target, GLsizeiptr size, const void *data, GLenum usage);
typedef void (APIENTRYP PFNGLDELETEBUFFERSPROC) (GLsizei n, const GLuint *buffers);
typedef void (APIENTRYP PFNGLVERTEXATTRIBPOINTERPROC) (GLuint index, GLint size, GLenum type, GLboolean normalized, GLsizei stride, const void *pointer);
typedef void (APIENTRYP PFNGLENABLEVERTEXATTRIBARRAYPROC) (GLuint index);
typedef GLuint (APIENTRYP PFNGLCREATESHADERPROC) (GLenum type);
typedef void (APIENTRYP PFNGLSHADERSOURCEPROC) (GLuint shader, GLsizei count, const GLchar *const*string, const GLint *length);
typedef void (APIENTRYP PFNGLCOMPILESHADERPROC) (GLuint shader);
typedef void (APIENTRYP PFNGLGETSHADERIVPROC) (GLuint shader, GLenum pname, GLint *params);
typedef void (APIENTRYP PFNGLGETSHADERINFOLOGPROC) (GLuint shader, GLsizei maxLength, GLsizei *length, GLchar *infoLog);
typedef GLuint (APIENTRYP PFNGLCREATEPROGRAMPROC) (void);
typedef void (APIENTRYP PFNGLATTACHSHADERPROC) (GLuint program, GLuint shader);
typedef void (APIENTRYP PFNGLLINKPROGRAMPROC) (GLuint program);
typedef void (APIENTRYP PFNGLGETPROGRAMIVPROC) (GLuint program, GLenum pname, GLint *params);
typedef void (APIENTRYP PFNGLGETPROGRAMINFOLOGPROC) (GLuint program, GLsizei maxLength, GLsizei *length, GLchar *infoLog);
typedef void (APIENTRYP PFNGLDELETESHADERPROC) (GLuint shader);
typedef void (APIENTRYP PFNGLUSEPROGRAMPROC) (GLuint program);
typedef GLint (APIENTRYP PFNGLGETUNIFORMLOCATIONPROC) (GLuint program, const GLchar *name);
typedef void (APIENTRYP PFNGLUNIFORMMATRIX4FVPROC) (GLint location, GLsizei count, GLboolean transpose, const GLfloat *value);
typedef void (APIENTRYP PFNGLUNIFORM3FPROC) (GLint location, GLfloat v0, GLfloat v1, GLfloat v2);
typedef void (APIENTRYP PFNGLUNIFORM1FPROC) (GLint location, GLfloat v0);
typedef void (APIENTRYP PFNGLUNIFORM1IPROC) (GLint location, GLint v0);

// Additional OpenGL constants that might be missing
#ifndef GL_TRIANGLES
#define GL_TRIANGLES 0x0004
#endif

#ifndef GL_UNSIGNED_INT
#define GL_UNSIGNED_INT 0x1405
#endif

#ifndef GL_ARRAY_BUFFER
#define GL_ARRAY_BUFFER 0x8892
#endif

#ifndef GL_ELEMENT_ARRAY_BUFFER
#define GL_ELEMENT_ARRAY_BUFFER 0x8893
#endif

#ifndef GL_STATIC_DRAW
#define GL_STATIC_DRAW 0x88E4
#endif

#ifndef GL_DEPTH_TEST
#define GL_DEPTH_TEST 0x0B71
#endif

#ifndef GL_COLOR_BUFFER_BIT
#define GL_COLOR_BUFFER_BIT 0x00004000
#endif

#ifndef GL_DEPTH_BUFFER_BIT
#define GL_DEPTH_BUFFER_BIT 0x00000100
#endif

#ifndef GL_FLOAT
#define GL_FLOAT 0x1406
#endif

#ifndef GL_FALSE
#define GL_FALSE 0
#endif

#ifndef GL_VERTEX_SHADER
#define GL_VERTEX_SHADER 0x8B31
#endif

#ifndef GL_FRAGMENT_SHADER
#define GL_FRAGMENT_SHADER 0x8B30
#endif

#ifndef GL_COMPILE_STATUS
#define GL_COMPILE_STATUS 0x8B81
#endif

#ifndef GL_LINK_STATUS
#define GL_LINK_STATUS 0x8B82
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