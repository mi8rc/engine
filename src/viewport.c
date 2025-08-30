#include "viewport.h"
#include "renderer.h"
#include "window.h"

static Viewport *g_viewport = NULL;

// Vertex shader source
static const char *vertex_shader_source = 
"#version 330 core\n"
"layout (location = 0) in vec3 aPos;\n"
"layout (location = 1) in vec3 aNormal;\n"
"layout (location = 2) in vec3 aColor;\n"
"uniform mat4 model;\n"
"uniform mat4 view;\n"
"uniform mat4 projection;\n"
"out vec3 FragPos;\n"
"out vec3 Normal;\n"
"out vec3 Color;\n"
"void main() {\n"
"    FragPos = vec3(model * vec4(aPos, 1.0));\n"
"    Normal = mat3(transpose(inverse(model))) * aNormal;\n"
"    Color = aColor;\n"
"    gl_Position = projection * view * vec4(FragPos, 1.0);\n"
"}\n";

// Fragment shader source
static const char *fragment_shader_source = 
"#version 330 core\n"
"out vec4 FragColor;\n"
"in vec3 FragPos;\n"
"in vec3 Normal;\n"
"in vec3 Color;\n"
"uniform vec3 lightPos;\n"
"uniform vec3 lightColor;\n"
"uniform vec3 viewPos;\n"
"void main() {\n"
"    // Ambient\n"
"    float ambientStrength = 0.3;\n"
"    vec3 ambient = ambientStrength * lightColor;\n"
"    // Diffuse\n"
"    vec3 norm = normalize(Normal);\n"
"    vec3 lightDir = normalize(lightPos - FragPos);\n"
"    float diff = max(dot(norm, lightDir), 0.0);\n"
"    vec3 diffuse = diff * lightColor;\n"
"    // Specular\n"
"    float specularStrength = 0.5;\n"
"    vec3 viewDir = normalize(viewPos - FragPos);\n"
"    vec3 reflectDir = reflect(-lightDir, norm);\n"
"    float spec = pow(max(dot(viewDir, reflectDir), 0.0), 32);\n"
"    vec3 specular = specularStrength * spec * lightColor;\n"
"    vec3 result = (ambient + diffuse + specular) * Color;\n"
"    FragColor = vec4(result, 1.0);\n"
"}\n";

static GLuint compile_shader(GLenum type, const char *source) {
    GLuint shader = glCreateShader(type);
    glShaderSource(shader, 1, &source, NULL);
    glCompileShader(shader);
    
    GLint success;
    glGetShaderiv(shader, GL_COMPILE_STATUS, &success);
    if (!success) {
        char info_log[512];
        glGetShaderInfoLog(shader, 512, NULL, info_log);
        g_error("Shader compilation failed: %s", info_log);
    }
    
    return shader;
}

static void create_grid(Viewport *viewport) {
    float grid_size = 20.0f;
    int grid_lines = 41; // -20 to 20
    float vertices[grid_lines * 4 * 6]; // 4 vertices per line, 6 components per vertex
    
    int index = 0;
    for (int i = 0; i < grid_lines; i++) {
        float pos = (i - grid_lines/2) * 1.0f;
        
        // Vertical lines
        vertices[index++] = pos; vertices[index++] = 0.0f; vertices[index++] = -grid_size/2;
        vertices[index++] = 0.5f; vertices[index++] = 0.5f; vertices[index++] = 0.5f;
        
        vertices[index++] = pos; vertices[index++] = 0.0f; vertices[index++] = grid_size/2;
        vertices[index++] = 0.5f; vertices[index++] = 0.5f; vertices[index++] = 0.5f;
        
        // Horizontal lines
        vertices[index++] = -grid_size/2; vertices[index++] = 0.0f; vertices[index++] = pos;
        vertices[index++] = 0.5f; vertices[index++] = 0.5f; vertices[index++] = 0.5f;
        
        vertices[index++] = grid_size/2; vertices[index++] = 0.0f; vertices[index++] = pos;
        vertices[index++] = 0.5f; vertices[index++] = 0.5f; vertices[index++] = 0.5f;
    }
    
    glGenVertexArrays(1, &viewport->grid_vao);
    glGenBuffers(1, &viewport->grid_vbo);
    
    glBindVertexArray(viewport->grid_vao);
    glBindBuffer(GL_ARRAY_BUFFER, viewport->grid_vbo);
    glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);
    
    // Position attribute
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(0);
    
    // Color attribute
    glVertexAttribPointer(2, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void*)(3 * sizeof(float)));
    glEnableVertexAttribArray(2);
    
    glBindBuffer(GL_ARRAY_BUFFER, 0);
    glBindVertexArray(0);
}

void viewport_init_gl(Viewport *viewport) {
    if (viewport->gl_initialized) return;
    
    // Initialize GLEW
    if (glewInit() != GLEW_OK) {
        g_error("Failed to initialize GLEW");
        return;
    }
    
    // Create shaders
    GLuint vertex_shader = compile_shader(GL_VERTEX_SHADER, vertex_shader_source);
    GLuint fragment_shader = compile_shader(GL_FRAGMENT_SHADER, fragment_shader_source);
    
    // Create shader program
    viewport->shader_program = glCreateProgram();
    glAttachShader(viewport->shader_program, vertex_shader);
    glAttachShader(viewport->shader_program, fragment_shader);
    glLinkProgram(viewport->shader_program);
    
    GLint success;
    glGetProgramiv(viewport->shader_program, GL_LINK_STATUS, &success);
    if (!success) {
        char info_log[512];
        glGetProgramInfoLog(viewport->shader_program, 512, NULL, info_log);
        g_error("Shader program linking failed: %s", info_log);
    }
    
    glDeleteShader(vertex_shader);
    glDeleteShader(fragment_shader);
    
    // Create grid
    create_grid(viewport);
    
    // Enable depth testing
    glEnable(GL_DEPTH_TEST);
    
    // Set clear color (dark background like Roblox Studio)
    glClearColor(0.176f, 0.176f, 0.188f, 1.0f); // #2D2D30
    
    viewport->gl_initialized = TRUE;
}

void viewport_cleanup_gl(Viewport *viewport) {
    if (!viewport->gl_initialized) return;
    
    if (viewport->grid_vao) {
        glDeleteVertexArrays(1, &viewport->grid_vao);
        viewport->grid_vao = 0;
    }
    if (viewport->grid_vbo) {
        glDeleteBuffers(1, &viewport->grid_vbo);
        viewport->grid_vbo = 0;
    }
    if (viewport->shader_program) {
        glDeleteProgram(viewport->shader_program);
        viewport->shader_program = 0;
    }
    
    viewport->gl_initialized = FALSE;
}

void viewport_render(Viewport *viewport) {
    if (!viewport->gl_initialized) return;
    
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    
    glUseProgram(viewport->shader_program);
    
    // Set up matrices
    float model[16] = {
        1.0f, 0.0f, 0.0f, 0.0f,
        0.0f, 1.0f, 0.0f, 0.0f,
        0.0f, 0.0f, 1.0f, 0.0f,
        0.0f, 0.0f, 0.0f, 1.0f
    };
    
    // View matrix (camera)
    float view[16];
    create_view_matrix(view, g_editor_state->camera_pos, g_editor_state->camera_rot);
    
    // Projection matrix
    GtkAllocation allocation;
    gtk_widget_get_allocation(viewport->gl_area, &allocation);
    float aspect = (float)allocation.width / (float)allocation.height;
    float projection[16];
    create_projection_matrix(projection, 45.0f, aspect, 0.1f, 1000.0f);
    
    // Set uniforms
    GLint model_loc = glGetUniformLocation(viewport->shader_program, "model");
    GLint view_loc = glGetUniformLocation(viewport->shader_program, "view");
    GLint projection_loc = glGetUniformLocation(viewport->shader_program, "projection");
    GLint light_pos_loc = glGetUniformLocation(viewport->shader_program, "lightPos");
    GLint light_color_loc = glGetUniformLocation(viewport->shader_program, "lightColor");
    GLint view_pos_loc = glGetUniformLocation(viewport->shader_program, "viewPos");
    
    glUniformMatrix4fv(model_loc, 1, GL_FALSE, model);
    glUniformMatrix4fv(view_loc, 1, GL_FALSE, view);
    glUniformMatrix4fv(projection_loc, 1, GL_FALSE, projection);
    glUniform3f(light_pos_loc, 10.0f, 10.0f, 10.0f);
    glUniform3f(light_color_loc, 1.0f, 1.0f, 1.0f);
    glUniform3fv(view_pos_loc, 1, g_editor_state->camera_pos);
    
    // Render grid
    if (viewport->show_grid) {
        glBindVertexArray(viewport->grid_vao);
        glDrawArrays(GL_LINES, 0, 164); // 41 * 4 vertices
        glBindVertexArray(0);
    }
    
    // Render NURBS objects
    render_nurbs_objects();
    
    glUseProgram(0);
}

gboolean on_gl_area_render(GtkGLArea *area, GdkGLContext *context, gpointer user_data) {
    Viewport *viewport = (Viewport*)user_data;
    viewport_render(viewport);
    return TRUE;
}

void on_gl_area_realize(GtkGLArea *area, gpointer user_data) {
    Viewport *viewport = (Viewport*)user_data;
    gtk_gl_area_make_current(area);
    viewport_init_gl(viewport);
}

void on_gl_area_unrealize(GtkGLArea *area, gpointer user_data) {
    Viewport *viewport = (Viewport*)user_data;
    gtk_gl_area_make_current(area);
    viewport_cleanup_gl(viewport);
}

gboolean on_gl_area_button_press(GtkWidget *widget, GdkEventButton *event, gpointer user_data) {
    Viewport *viewport = (Viewport*)user_data;
    
    if (event->button == 2) { // Middle mouse button
        viewport->mouse_dragging = TRUE;
        viewport->last_mouse_x = event->x;
        viewport->last_mouse_y = event->y;
        return TRUE;
    }
    
    return FALSE;
}

gboolean on_gl_area_button_release(GtkWidget *widget, GdkEventButton *event, gpointer user_data) {
    Viewport *viewport = (Viewport*)user_data;
    
    if (event->button == 2) {
        viewport->mouse_dragging = FALSE;
        return TRUE;
    }
    
    return FALSE;
}

gboolean on_gl_area_motion_notify(GtkWidget *widget, GdkEventMotion *event, gpointer user_data) {
    Viewport *viewport = (Viewport*)user_data;
    
    if (viewport->mouse_dragging) {
        double dx = event->x - viewport->last_mouse_x;
        double dy = event->y - viewport->last_mouse_y;
        
        // Rotate camera
        g_editor_state->camera_rot[1] += dx * 0.01f;
        g_editor_state->camera_rot[0] += dy * 0.01f;
        
        viewport->last_mouse_x = event->x;
        viewport->last_mouse_y = event->y;
        
        gtk_widget_queue_draw(widget);
        return TRUE;
    }
    
    return FALSE;
}

gboolean on_gl_area_scroll(GtkWidget *widget, GdkEventScroll *event, gpointer user_data) {
    if (event->direction == GDK_SCROLL_UP) {
        g_editor_state->camera_zoom *= 0.9f;
    } else if (event->direction == GDK_SCROLL_DOWN) {
        g_editor_state->camera_zoom *= 1.1f;
    }
    
    gtk_widget_queue_draw(widget);
    return TRUE;
}

GtkWidget *create_viewport(void) {
    g_viewport = g_malloc0(sizeof(Viewport));
    
    // Create container
    g_viewport->container = gtk_box_new(GTK_ORIENTATION_VERTICAL, 0);
    
    // Create GL area
    g_viewport->gl_area = gtk_gl_area_new();
    gtk_gl_area_set_required_version(GTK_GL_AREA(g_viewport->gl_area), 3, 3);
    gtk_box_pack_start(GTK_BOX(g_viewport->container), g_viewport->gl_area, TRUE, TRUE, 0);
    
    // Initialize state
    g_viewport->gl_initialized = FALSE;
    g_viewport->mouse_dragging = FALSE;
    g_viewport->show_grid = TRUE;
    
    // Connect signals
    g_signal_connect(g_viewport->gl_area, "render", G_CALLBACK(on_gl_area_render), g_viewport);
    g_signal_connect(g_viewport->gl_area, "realize", G_CALLBACK(on_gl_area_realize), g_viewport);
    g_signal_connect(g_viewport->gl_area, "unrealize", G_CALLBACK(on_gl_area_unrealize), g_viewport);
    
    // Mouse events
    gtk_widget_add_events(g_viewport->gl_area, GDK_BUTTON_PRESS_MASK | GDK_BUTTON_RELEASE_MASK | 
                         GDK_POINTER_MOTION_MASK | GDK_SCROLL_MASK);
    g_signal_connect(g_viewport->gl_area, "button-press-event", G_CALLBACK(on_gl_area_button_press), g_viewport);
    g_signal_connect(g_viewport->gl_area, "button-release-event", G_CALLBACK(on_gl_area_button_release), g_viewport);
    g_signal_connect(g_viewport->gl_area, "motion-notify-event", G_CALLBACK(on_gl_area_motion_notify), g_viewport);
    g_signal_connect(g_viewport->gl_area, "scroll-event", G_CALLBACK(on_gl_area_scroll), g_viewport);
    
    return g_viewport->container;
}