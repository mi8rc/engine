#include "fps_engine.h"
#include <string.h>
#include <stdio.h>

// Global engine pointer for callbacks
static FPSEngine *g_engine = NULL;

// Default vertex shader for NURBS surfaces
const char *vertex_shader_source = 
"#version 330 core\n"
"layout (location = 0) in vec3 aPos;\n"
"layout (location = 1) in vec3 aNormal;\n"
"\n"
"uniform mat4 model;\n"
"uniform mat4 view;\n"
"uniform mat4 projection;\n"
"\n"
"out vec3 FragPos;\n"
"out vec3 Normal;\n"
"\n"
"void main()\n"
"{\n"
"    FragPos = vec3(model * vec4(aPos, 1.0));\n"
"    Normal = mat3(transpose(inverse(model))) * aNormal;\n"
"    \n"
"    gl_Position = projection * view * vec4(FragPos, 1.0);\n"
"}\n";

// Fragment shader with Phong lighting for multiple lights
const char *fragment_shader_source = 
"#version 330 core\n"
"out vec4 FragColor;\n"
"\n"
"in vec3 FragPos;\n"
"in vec3 Normal;\n"
"\n"
"#define MAX_LIGHTS 32\n"
"\n"
"uniform vec3 lightPos[MAX_LIGHTS];\n"
"uniform vec3 lightColor[MAX_LIGHTS];\n"
"uniform float lightIntensity[MAX_LIGHTS];\n"
"uniform int numLights;\n"
"\n"
"uniform vec3 materialAmbient;\n"
"uniform vec3 materialDiffuse;\n"
"uniform vec3 materialSpecular;\n"
"uniform float materialShininess;\n"
"\n"
"uniform vec3 cameraPos;\n"
"\n"
"void main()\n"
"{\n"
"    vec3 norm = normalize(Normal);\n"
"    vec3 result = materialAmbient * 0.1; // Ambient\n"
"    \n"
"    for (int i = 0; i < numLights && i < MAX_LIGHTS; i++) {\n"
"        // Diffuse\n"
"        vec3 lightDir = normalize(lightPos[i] - FragPos);\n"
"        float diff = max(dot(norm, lightDir), 0.0);\n"
"        vec3 diffuse = diff * lightColor[i] * materialDiffuse * lightIntensity[i];\n"
"        \n"
"        // Specular\n"
"        vec3 viewDir = normalize(cameraPos - FragPos);\n"
"        vec3 reflectDir = reflect(-lightDir, norm);\n"
"        float spec = pow(max(dot(viewDir, reflectDir), 0.0), materialShininess);\n"
"        vec3 specular = spec * lightColor[i] * materialSpecular * lightIntensity[i];\n"
"        \n"
"        // Attenuation\n"
"        float distance = length(lightPos[i] - FragPos);\n"
"        float attenuation = 1.0 / (1.0 + 0.09 * distance + 0.032 * (distance * distance));\n"
"        \n"
"        diffuse *= attenuation;\n"
"        specular *= attenuation;\n"
"        \n"
"        result += diffuse + specular;\n"
"    }\n"
"    \n"
"    FragColor = vec4(result, 1.0);\n"
"}\n";

// Initialize FPS engine
int fps_engine_init(FPSEngine *engine, int width, int height, const char *title) {
    g_engine = engine;
    
    // Initialize GLFW
    if (!glfwInit()) {
        fprintf(stderr, "Failed to initialize GLFW\n");
        return -1;
    }
    
    // Configure GLFW
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
    
    // Create window
    engine->window = glfwCreateWindow(width, height, title, NULL, NULL);
    if (engine->window == NULL) {
        fprintf(stderr, "Failed to create GLFW window\n");
        glfwTerminate();
        return -1;
    }
    
    glfwMakeContextCurrent(engine->window);
    
    // Set callbacks
    glfwSetFramebufferSizeCallback(engine->window, framebuffer_size_callback);
    glfwSetCursorPosCallback(engine->window, mouse_callback);
    glfwSetScrollCallback(engine->window, scroll_callback);
    glfwSetKeyCallback(engine->window, key_callback);
    
    // Capture mouse
    glfwSetInputMode(engine->window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);
    
    // Enable depth testing
    glEnable(GL_DEPTH_TEST);
    
    // Initialize camera
    Vector3 camera_pos = {0.0f, 2.0f, 5.0f};
    Vector3 world_up = {0.0f, 1.0f, 0.0f};
    camera_init(&engine->camera, camera_pos, world_up, -90.0f, 0.0f);
    
    // Initialize world
    world_init(&engine->world);
    
    // Initialize input state
    memset(&engine->input, 0, sizeof(InputState));
    
    // Load shader program
    if (load_shader_program(&engine->shader, vertex_shader_source, fragment_shader_source) != 0) {
        fprintf(stderr, "Failed to load shader program\n");
        return -1;
    }
    
    engine->screen_width = width;
    engine->screen_height = height;
    engine->running = true;
    
    return 0;
}

// Camera initialization
void camera_init(Camera *camera, Vector3 position, Vector3 up, float yaw, float pitch) {
    camera->position = position;
    camera->world_up = up;
    camera->yaw = yaw;
    camera->pitch = pitch;
    camera->movement_speed = 5.0f;
    camera->mouse_sensitivity = 0.1f;
    camera->zoom = 45.0f;
    camera->first_mouse = true;
    camera->last_x = 400.0f;
    camera->last_y = 300.0f;
    
    camera_update_vectors(camera);
}

void camera_update_vectors(Camera *camera) {
    Vector3 front;
    front.x = cosf(camera->yaw * M_PI / 180.0f) * cosf(camera->pitch * M_PI / 180.0f);
    front.y = sinf(camera->pitch * M_PI / 180.0f);
    front.z = sinf(camera->yaw * M_PI / 180.0f) * cosf(camera->pitch * M_PI / 180.0f);
    camera->front = vector3_normalize(front);
    
    camera->right = vector3_normalize(vector3_cross(camera->front, camera->world_up));
    camera->up = vector3_normalize(vector3_cross(camera->right, camera->front));
}

void camera_process_keyboard(Camera *camera, int direction, float delta_time) {
    float velocity = camera->movement_speed * delta_time;
    
    switch (direction) {
        case CAMERA_FORWARD:
            camera->position = vector3_add(camera->position, vector3_multiply(camera->front, velocity));
            break;
        case CAMERA_BACKWARD:
            camera->position = vector3_subtract(camera->position, vector3_multiply(camera->front, velocity));
            break;
        case CAMERA_LEFT:
            camera->position = vector3_subtract(camera->position, vector3_multiply(camera->right, velocity));
            break;
        case CAMERA_RIGHT:
            camera->position = vector3_add(camera->position, vector3_multiply(camera->right, velocity));
            break;
        case CAMERA_UP:
            camera->position = vector3_add(camera->position, vector3_multiply(camera->up, velocity));
            break;
        case CAMERA_DOWN:
            camera->position = vector3_subtract(camera->position, vector3_multiply(camera->up, velocity));
            break;
    }
}

void camera_process_mouse_movement(Camera *camera, float xoffset, float yoffset, bool constrain_pitch) {
    xoffset *= camera->mouse_sensitivity;
    yoffset *= camera->mouse_sensitivity;
    
    camera->yaw += xoffset;
    camera->pitch += yoffset;
    
    if (constrain_pitch) {
        if (camera->pitch > 89.0f)
            camera->pitch = 89.0f;
        if (camera->pitch < -89.0f)
            camera->pitch = -89.0f;
    }
    
    camera_update_vectors(camera);
}

void camera_get_view_matrix(Camera *camera, float *matrix) {
    Vector3 center = vector3_add(camera->position, camera->front);
    matrix_look_at(matrix, camera->position, center, camera->up);
}

// World management
void world_init(World *world) {
    memset(world, 0, sizeof(World));
    world->ambient_light = (Vector3){0.2f, 0.2f, 0.2f};
}

int world_add_object(World *world, GameObject *object) {
    if (world->num_objects >= MAX_OBJECTS) return -1;
    
    world->objects[world->num_objects] = *object;
    return world->num_objects++;
}

int world_add_light(World *world, Light *light) {
    if (world->num_lights >= MAX_LIGHTS) return -1;
    
    world->lights[world->num_lights] = *light;
    return world->num_lights++;
}

void world_render(World *world, ShaderProgram *shader, Camera *camera) {
    use_shader_program(shader);
    
    // Set camera uniforms
    set_uniform_vector3(shader->camera_pos_loc, camera->position);
    
    // Set light uniforms
    set_uniform_int(shader->num_lights_loc, world->num_lights);
    for (int i = 0; i < world->num_lights; i++) {
        set_uniform_vector3(shader->light_pos_loc[i], world->lights[i].position);
        set_uniform_vector3(shader->light_color_loc[i], world->lights[i].color);
        set_uniform_float(shader->light_intensity_loc[i], world->lights[i].intensity);
    }
    
    // Render all objects
    for (int i = 0; i < world->num_objects; i++) {
        game_object_render(&world->objects[i], shader);
    }
}

// GameObject functions
GameObject* create_game_object(const char *name, Vector3 position) {
    GameObject *object = malloc(sizeof(GameObject));
    memset(object, 0, sizeof(GameObject));
    
    strncpy(object->name, name, sizeof(object->name) - 1);
    object->position = position;
    object->rotation = (Vector3){0.0f, 0.0f, 0.0f};
    object->scale = (Vector3){1.0f, 1.0f, 1.0f};
    object->is_collidable = true;
    
    // Default material
    object->material.ambient = (Vector3){0.2f, 0.2f, 0.2f};
    object->material.diffuse = (Vector3){0.8f, 0.8f, 0.8f};
    object->material.specular = (Vector3){1.0f, 1.0f, 1.0f};
    object->material.shininess = 32.0f;
    
    return object;
}

void game_object_add_nurbs_surface(GameObject *object, NURBSSurface *surface) {
    if (object->num_surfaces >= MAX_CONTROL_POINTS) return;
    
    if (!object->surfaces) {
        object->surfaces = malloc(sizeof(NURBSSurface*) * MAX_CONTROL_POINTS);
        object->tessellated_surfaces = malloc(sizeof(TessellatedSurface*) * MAX_CONTROL_POINTS);
    }
    
    object->surfaces[object->num_surfaces] = surface;
    object->tessellated_surfaces[object->num_surfaces] = tessellate_nurbs_surface(surface, 32, 32);
    object->num_surfaces++;
}

void game_object_render(GameObject *object, ShaderProgram *shader) {
    // Calculate model matrix
    float model[16];
    matrix_identity(model);
    matrix_translate(model, object->position);
    // Add rotation and scale transforms here
    
    set_uniform_matrix4(shader->model_loc, model);
    
    // Set material uniforms
    set_uniform_vector3(shader->material_ambient_loc, object->material.ambient);
    set_uniform_vector3(shader->material_diffuse_loc, object->material.diffuse);
    set_uniform_vector3(shader->material_specular_loc, object->material.specular);
    set_uniform_float(shader->material_shininess_loc, object->material.shininess);
    
    // Render all tessellated surfaces
    for (int i = 0; i < object->num_surfaces; i++) {
        TessellatedSurface *surface = object->tessellated_surfaces[i];
        glBindVertexArray(surface->vao);
        glDrawElements(GL_TRIANGLES, surface->num_triangles * 3, GL_UNSIGNED_INT, 0);
    }
}

// Main game loop
void fps_engine_run(FPSEngine *engine) {
    while (!glfwWindowShouldClose(engine->window) && engine->running) {
        // Calculate delta time
        float current_frame = glfwGetTime();
        engine->input.delta_time = current_frame - engine->input.last_frame;
        engine->input.last_frame = current_frame;
        
        // Poll events
        glfwPollEvents();
        
        // Handle input
        fps_engine_handle_input(engine);
        
        // Update
        fps_engine_update(engine);
        
        // Render
        fps_engine_render(engine);
        
        // Swap buffers
        glfwSwapBuffers(engine->window);
    }
}

void fps_engine_handle_input(FPSEngine *engine) {
    // Handle keyboard input
    if (engine->input.keys[GLFW_KEY_W]) {
        Vector3 new_pos = engine->camera.position;
        camera_process_keyboard(&engine->camera, CAMERA_FORWARD, engine->input.delta_time);
        if (check_player_collision(engine, engine->camera.position)) {
            engine->camera.position = new_pos; // Revert movement
        }
    }
    if (engine->input.keys[GLFW_KEY_S]) {
        Vector3 new_pos = engine->camera.position;
        camera_process_keyboard(&engine->camera, CAMERA_BACKWARD, engine->input.delta_time);
        if (check_player_collision(engine, engine->camera.position)) {
            engine->camera.position = new_pos;
        }
    }
    if (engine->input.keys[GLFW_KEY_A]) {
        Vector3 new_pos = engine->camera.position;
        camera_process_keyboard(&engine->camera, CAMERA_LEFT, engine->input.delta_time);
        if (check_player_collision(engine, engine->camera.position)) {
            engine->camera.position = new_pos;
        }
    }
    if (engine->input.keys[GLFW_KEY_D]) {
        Vector3 new_pos = engine->camera.position;
        camera_process_keyboard(&engine->camera, CAMERA_RIGHT, engine->input.delta_time);
        if (check_player_collision(engine, engine->camera.position)) {
            engine->camera.position = new_pos;
        }
    }
    if (engine->input.keys[GLFW_KEY_SPACE]) {
        camera_process_keyboard(&engine->camera, CAMERA_UP, engine->input.delta_time);
    }
    if (engine->input.keys[GLFW_KEY_LEFT_SHIFT]) {
        camera_process_keyboard(&engine->camera, CAMERA_DOWN, engine->input.delta_time);
    }
    if (engine->input.keys[GLFW_KEY_ESCAPE]) {
        engine->running = false;
    }
}

void fps_engine_update(FPSEngine *engine) {
    world_update(&engine->world, engine->input.delta_time);
}

void fps_engine_render(FPSEngine *engine) {
    // Clear screen
    glClearColor(0.1f, 0.1f, 0.1f, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    
    // Set up matrices
    float view[16], projection[16];
    camera_get_view_matrix(&engine->camera, view);
    matrix_perspective(projection, engine->camera.zoom, 
                      (float)engine->screen_width / engine->screen_height, 0.1f, 100.0f);
    
    use_shader_program(&engine->shader);
    set_uniform_matrix4(engine->shader.view_loc, view);
    set_uniform_matrix4(engine->shader.projection_loc, projection);
    
    // Render world
    world_render(&engine->world, &engine->shader, &engine->camera);
}

// Collision detection
bool check_player_collision(FPSEngine *engine, Vector3 new_position) {
    float player_radius = 0.5f;
    
    for (int i = 0; i < engine->world.num_objects; i++) {
        GameObject *obj = &engine->world.objects[i];
        if (!obj->is_collidable) continue;
        
        // Simple sphere-surface collision check
        for (int j = 0; j < obj->num_surfaces; j++) {
            // Check if player sphere intersects with any surface
            // This is a simplified check - in practice you'd use more sophisticated collision
            float distance_to_object = vector3_length(vector3_subtract(new_position, obj->position));
            if (distance_to_object < player_radius + 1.0f) { // Rough collision approximation
                return true;
            }
        }
    }
    
    return false;
}

// Cleanup
void fps_engine_cleanup(FPSEngine *engine) {
    // Cleanup game objects
    for (int i = 0; i < engine->world.num_objects; i++) {
        GameObject *obj = &engine->world.objects[i];
        for (int j = 0; j < obj->num_surfaces; j++) {
            free_tessellated_surface(obj->tessellated_surfaces[j]);
            free_nurbs_surface(obj->surfaces[j]);
        }
        if (obj->surfaces) free(obj->surfaces);
        if (obj->tessellated_surfaces) free(obj->tessellated_surfaces);
    }
    
    glfwTerminate();
}

// GLFW callbacks
void mouse_callback(GLFWwindow *window, double xpos, double ypos) {
    if (g_engine->camera.first_mouse) {
        g_engine->camera.last_x = xpos;
        g_engine->camera.last_y = ypos;
        g_engine->camera.first_mouse = false;
    }
    
    float xoffset = xpos - g_engine->camera.last_x;
    float yoffset = g_engine->camera.last_y - ypos; // Reversed since y-coordinates go from bottom to top
    
    g_engine->camera.last_x = xpos;
    g_engine->camera.last_y = ypos;
    
    camera_process_mouse_movement(&g_engine->camera, xoffset, yoffset, true);
}

void key_callback(GLFWwindow *window, int key, int scancode, int action, int mods) {
    if (key >= 0 && key < 1024) {
        if (action == GLFW_PRESS)
            g_engine->input.keys[key] = true;
        else if (action == GLFW_RELEASE)
            g_engine->input.keys[key] = false;
    }
}

void framebuffer_size_callback(GLFWwindow *window, int width, int height) {
    glViewport(0, 0, width, height);
    g_engine->screen_width = width;
    g_engine->screen_height = height;
}