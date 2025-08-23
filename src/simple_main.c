#define _USE_MATH_DEFINES
#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <GLFW/glfw3.h>
#include <GL/gl.h>
#include <GL/glu.h>

#ifndef M_PI
#define M_PI 3.14159265358979323846
#endif

// Simple NURBS evaluation for testing
float nurbs_basis_function(int i, int degree, float t, float *knots) {
    if (degree == 0) {
        return (t >= knots[i] && t < knots[i + 1]) ? 1.0f : 0.0f;
    }
    
    float left = 0.0f, right = 0.0f;
    float epsilon = 1e-6f;
    
    if (fabs(knots[i + degree] - knots[i]) > epsilon) {
        left = (t - knots[i]) / (knots[i + degree] - knots[i]) * 
               nurbs_basis_function(i, degree - 1, t, knots);
    }
    
    if (fabs(knots[i + degree + 1] - knots[i + 1]) > epsilon) {
        right = (knots[i + degree + 1] - t) / (knots[i + degree + 1] - knots[i + 1]) * 
                nurbs_basis_function(i + 1, degree - 1, t, knots);
    }
    
    return left + right;
}

void render_nurbs_sphere() {
    // Simple sphere using NURBS-like evaluation
    int lat_segments = 20;
    int lon_segments = 20;
    float radius = 1.0f;
    
    glBegin(GL_TRIANGLES);
    for (int lat = 0; lat < lat_segments; lat++) {
        for (int lon = 0; lon < lon_segments; lon++) {
            float lat1 = M_PI * (-0.5f + (float)lat / lat_segments);
            float lat2 = M_PI * (-0.5f + (float)(lat + 1) / lat_segments);
            float lon1 = 2.0f * M_PI * (float)lon / lon_segments;
            float lon2 = 2.0f * M_PI * (float)(lon + 1) / lon_segments;
            
            // Generate vertices
            float x1 = radius * cosf(lat1) * cosf(lon1);
            float y1 = radius * sinf(lat1);
            float z1 = radius * cosf(lat1) * sinf(lon1);
            
            float x2 = radius * cosf(lat2) * cosf(lon1);
            float y2 = radius * sinf(lat2);
            float z2 = radius * cosf(lat2) * sinf(lon1);
            
            float x3 = radius * cosf(lat2) * cosf(lon2);
            float y3 = radius * sinf(lat2);
            float z3 = radius * cosf(lat2) * sinf(lon2);
            
            float x4 = radius * cosf(lat1) * cosf(lon2);
            float y4 = radius * sinf(lat1);
            float z4 = radius * cosf(lat1) * sinf(lon2);
            
            // First triangle
            glNormal3f(x1, y1, z1); glVertex3f(x1, y1, z1);
            glNormal3f(x2, y2, z2); glVertex3f(x2, y2, z2);
            glNormal3f(x3, y3, z3); glVertex3f(x3, y3, z3);
            
            // Second triangle
            glNormal3f(x1, y1, z1); glVertex3f(x1, y1, z1);
            glNormal3f(x3, y3, z3); glVertex3f(x3, y3, z3);
            glNormal3f(x4, y4, z4); glVertex3f(x4, y4, z4);
        }
    }
    glEnd();
}

void render_nurbs_plane() {
    // Simple plane using NURBS-like evaluation
    float size = 5.0f;
    int segments = 20;
    
    glBegin(GL_TRIANGLES);
    for (int i = 0; i < segments; i++) {
        for (int j = 0; j < segments; j++) {
            float x1 = -size + (2.0f * size * i) / segments;
            float x2 = -size + (2.0f * size * (i + 1)) / segments;
            float z1 = -size + (2.0f * size * j) / segments;
            float z2 = -size + (2.0f * size * (j + 1)) / segments;
            
            // First triangle
            glNormal3f(0.0f, 1.0f, 0.0f);
            glVertex3f(x1, 0.0f, z1);
            glVertex3f(x2, 0.0f, z1);
            glVertex3f(x2, 0.0f, z2);
            
            // Second triangle
            glNormal3f(0.0f, 1.0f, 0.0f);
            glVertex3f(x1, 0.0f, z1);
            glVertex3f(x2, 0.0f, z2);
            glVertex3f(x1, 0.0f, z2);
        }
    }
    glEnd();
}

// Camera variables
float camera_x = 0.0f, camera_y = 2.0f, camera_z = 5.0f;
float camera_yaw = -90.0f, camera_pitch = 0.0f;
float last_x = 400.0f, last_y = 300.0f;
bool first_mouse = true;

void mouse_callback(GLFWwindow* window, double xpos, double ypos) {
    (void)window;
    
    if (first_mouse) {
        last_x = xpos;
        last_y = ypos;
        first_mouse = false;
    }
    
    float xoffset = xpos - last_x;
    float yoffset = last_y - ypos;
    last_x = xpos;
    last_y = ypos;
    
    float sensitivity = 0.1f;
    xoffset *= sensitivity;
    yoffset *= sensitivity;
    
    camera_yaw += xoffset;
    camera_pitch += yoffset;
    
    if (camera_pitch > 89.0f) camera_pitch = 89.0f;
    if (camera_pitch < -89.0f) camera_pitch = -89.0f;
}

void process_input(GLFWwindow *window, float deltaTime) {
    float speed = 5.0f * deltaTime;
    
    float front_x = cosf(camera_yaw * M_PI / 180.0f) * cosf(camera_pitch * M_PI / 180.0f);
    float front_y = sinf(camera_pitch * M_PI / 180.0f);
    float front_z = sinf(camera_yaw * M_PI / 180.0f) * cosf(camera_pitch * M_PI / 180.0f);
    
    if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS) {
        camera_x += front_x * speed;
        camera_y += front_y * speed;
        camera_z += front_z * speed;
    }
    if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS) {
        camera_x -= front_x * speed;
        camera_y -= front_y * speed;
        camera_z -= front_z * speed;
    }
    if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS) {
        // Calculate right vector
        float right_x = cosf((camera_yaw - 90.0f) * M_PI / 180.0f);
        float right_z = sinf((camera_yaw - 90.0f) * M_PI / 180.0f);
        camera_x -= right_x * speed;
        camera_z -= right_z * speed;
    }
    if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS) {
        float right_x = cosf((camera_yaw - 90.0f) * M_PI / 180.0f);
        float right_z = sinf((camera_yaw - 90.0f) * M_PI / 180.0f);
        camera_x += right_x * speed;
        camera_z += right_z * speed;
    }
    if (glfwGetKey(window, GLFW_KEY_SPACE) == GLFW_PRESS) {
        camera_y += speed;
    }
    if (glfwGetKey(window, GLFW_KEY_LEFT_SHIFT) == GLFW_PRESS) {
        camera_y -= speed;
    }
    if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS) {
        glfwSetWindowShouldClose(window, true);
    }
}

void framebuffer_size_callback(GLFWwindow* window, int width, int height) {
    (void)window;
    glViewport(0, 0, width, height);
}

int main() {
    printf("NURBS FPS Game Engine (Legacy OpenGL)\n");
    printf("Controls:\n");
    printf("  WASD - Move\n");
    printf("  Mouse - Look around\n");
    printf("  Space - Move up\n");
    printf("  Shift - Move down\n");
    printf("  Escape - Exit\n\n");
    
    // Initialize GLFW
    if (!glfwInit()) {
        fprintf(stderr, "Failed to initialize GLFW\n");
        return -1;
    }
    
    // Configure GLFW for legacy OpenGL
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 2);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 1);
    
    // Create window
    GLFWwindow* window = glfwCreateWindow(1200, 800, "NURBS FPS Game", NULL, NULL);
    if (window == NULL) {
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
    glEnable(GL_LIGHTING);
    glEnable(GL_LIGHT0);
    glEnable(GL_COLOR_MATERIAL);
    
    printf("OpenGL Version: %s\n", glGetString(GL_VERSION));
    printf("Renderer: %s\n", glGetString(GL_RENDERER));
    
    // Set up lighting
    float light_pos[] = {2.0f, 4.0f, 2.0f, 1.0f};
    float light_color[] = {1.0f, 1.0f, 1.0f, 1.0f};
    float ambient[] = {0.2f, 0.2f, 0.2f, 1.0f};
    
    glLightfv(GL_LIGHT0, GL_POSITION, light_pos);
    glLightfv(GL_LIGHT0, GL_DIFFUSE, light_color);
    glLightfv(GL_LIGHT0, GL_SPECULAR, light_color);
    glLightModelfv(GL_LIGHT_MODEL_AMBIENT, ambient);
    
    float last_time = glfwGetTime();
    
    printf("Engine initialized successfully. Starting game loop...\n");
    
    // Main loop
    while (!glfwWindowShouldClose(window)) {
        float current_time = glfwGetTime();
        float delta_time = current_time - last_time;
        last_time = current_time;
        
        // Input
        process_input(window, delta_time);
        
        // Render
        glClearColor(0.1f, 0.1f, 0.1f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
        
        // Set up matrices
        glMatrixMode(GL_PROJECTION);
        glLoadIdentity();
        gluPerspective(45.0f, 1200.0f / 800.0f, 0.1f, 100.0f);
        
        glMatrixMode(GL_MODELVIEW);
        glLoadIdentity();
        
        // Camera
        float front_x = cosf(camera_yaw * M_PI / 180.0f) * cosf(camera_pitch * M_PI / 180.0f);
        float front_y = sinf(camera_pitch * M_PI / 180.0f);
        float front_z = sinf(camera_yaw * M_PI / 180.0f) * cosf(camera_pitch * M_PI / 180.0f);
        
        gluLookAt(camera_x, camera_y, camera_z,
                  camera_x + front_x, camera_y + front_y, camera_z + front_z,
                  0.0f, 1.0f, 0.0f);
        
        // Render floor
        glPushMatrix();
        glTranslatef(0.0f, -2.0f, 0.0f);
        glColor3f(0.3f, 0.8f, 0.3f);
        render_nurbs_plane();
        glPopMatrix();
        
        // Render spheres
        for (int i = 0; i < 5; i++) {
            glPushMatrix();
            glTranslatef((i - 2) * 3.0f, 0.0f, (i % 2) * 3.0f - 1.5f);
            
            // Different colors for each sphere
            float r = 0.3f + i * 0.15f;
            float g = 0.5f - i * 0.1f;
            float b = 0.8f - i * 0.1f;
            glColor3f(r, g, b);
            
            render_nurbs_sphere();
            glPopMatrix();
        }
        
        glfwSwapBuffers(window);
        glfwPollEvents();
    }
    
    glfwTerminate();
    printf("Game engine shut down successfully.\n");
    return 0;
}