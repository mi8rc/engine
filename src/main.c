#include <stdio.h>
#include <stdlib.h>
#include <GL/gl.h>
#include <GLFW/glfw3.h>
#include "fps_engine.h"
#include "nurbs.h"
#include "map_loader.h"

// Window dimensions
#define WINDOW_WIDTH 1200
#define WINDOW_HEIGHT 800

// Global variables
GLFWwindow* window = NULL;
FPSEngine* engine = NULL;

// Forward declarations
static void error_callback(int error, const char* description);
static void key_callback(GLFWwindow* window, int key, int scancode, int action, int mods);
static void framebuffer_size_callback(GLFWwindow* window, int width, int height);
static int init_opengl(void);
static void render_frame(void);
static void cleanup(void);

int main(void) {
    printf("NURBS Map Editor - Roblox Studio Style\n");
    printf("=====================================\n");
    
    // Initialize GLFW
    glfwSetErrorCallback(error_callback);
    
    if (!glfwInit()) {
        fprintf(stderr, "Failed to initialize GLFW\n");
        return -1;
    }
    
    // Configure OpenGL context
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
    glfwWindowHint(GLFW_RESIZABLE, GLFW_TRUE);
    
    // Create window
    window = glfwCreateWindow(WINDOW_WIDTH, WINDOW_HEIGHT, 
                             "NURBS Map Editor", NULL, NULL);
    if (!window) {
        fprintf(stderr, "Failed to create GLFW window\n");
        glfwTerminate();
        return -1;
    }
    
    // Set up OpenGL context
    glfwMakeContextCurrent(window);
    glfwSetKeyCallback(window, key_callback);
    glfwSetFramebufferSizeCallback(window, framebuffer_size_callback);
    
    // Initialize FPS Engine
    engine = fps_engine_create();
    if (!engine || fps_engine_init(engine, WINDOW_WIDTH, WINDOW_HEIGHT) != 0) {
        fprintf(stderr, "Failed to initialize FPS engine\n");
        cleanup();
        return -1;
    }
    
    printf("OpenGL Version: %s\n", glGetString(GL_VERSION));
    printf("Renderer: %s\n", glGetString(GL_RENDERER));
    printf("NURBS Map Editor initialized successfully!\n");
    
    // Main render loop
    while (!glfwWindowShouldClose(window)) {
        glfwPollEvents();
        
        // Update engine
        fps_engine_update(engine, 0.016f); // Assume 60 FPS
        
        // Render frame
        fps_engine_render(engine);
        
        glfwSwapBuffers(window);
    }
    
    cleanup();
    return 0;
}

static void error_callback(int error, const char* description) {
    fprintf(stderr, "GLFW Error %d: %s\n", error, description);
}

static void key_callback(GLFWwindow* window, int key, int scancode, int action, int mods) {
    if (key == GLFW_KEY_ESCAPE && action == GLFW_PRESS) {
        glfwSetWindowShouldClose(window, GLFW_TRUE);
    }
}

static void framebuffer_size_callback(GLFWwindow* window, int width, int height) {
    glViewport(0, 0, width, height);
}

static int init_opengl(void) {
    // Enable depth testing
    glEnable(GL_DEPTH_TEST);
    
    // Set clear color (Roblox Studio-like background)
    glClearColor(0.2f, 0.2f, 0.2f, 1.0f);
    
    return 0;
}

static void render_frame(void) {
    // Clear buffers
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    
    // TODO: Render NURBS surfaces and UI
    // For now, just render a simple colored background
    
    // Draw a simple grid to simulate the Roblox Studio viewport
    glBegin(GL_LINES);
    glColor3f(0.3f, 0.3f, 0.3f);
    
    // Draw grid lines
    for (int i = -10; i <= 10; i++) {
        // Horizontal lines
        glVertex3f(-10.0f, 0.0f, (float)i);
        glVertex3f(10.0f, 0.0f, (float)i);
        
        // Vertical lines
        glVertex3f((float)i, 0.0f, -10.0f);
        glVertex3f((float)i, 0.0f, 10.0f);
    }
    
    glEnd();
}

static void cleanup(void) {
    if (engine) {
        fps_engine_destroy(engine);
        engine = NULL;
    }
    if (window) {
        glfwDestroyWindow(window);
        window = NULL;
    }
    glfwTerminate();
}