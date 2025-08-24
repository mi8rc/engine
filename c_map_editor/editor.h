#ifndef EDITOR_H
#define EDITOR_H

#include <gtk/gtk.h>
#include <GL/gl.h>
#include <GL/glu.h>
#include <stdbool.h>
#include "../src/nurbs.h"

// Forward declarations
typedef struct MapEditor MapEditor;
typedef struct EditorObject EditorObject;
typedef struct EditorLight EditorLight;
typedef struct EditorMaterial EditorMaterial;

// Object types matching Python version
typedef enum {
    OBJECT_TYPE_SPHERE,
    OBJECT_TYPE_PLANE,
    OBJECT_TYPE_CYLINDER,
    OBJECT_TYPE_TORUS,
    OBJECT_TYPE_CUSTOM,
    OBJECT_TYPE_IGES_MODEL
} ObjectType;

// Light types
typedef enum {
    LIGHT_TYPE_POINT = 0,
    LIGHT_TYPE_DIRECTIONAL = 1,
    LIGHT_TYPE_SPOT = 2
} LightType;

// Tool modes
typedef enum {
    TOOL_SELECT,
    TOOL_MOVE,
    TOOL_ROTATE,
    TOOL_SCALE
} ToolMode;

// View modes
typedef enum {
    VIEW_PERSPECTIVE,
    VIEW_TOP,
    VIEW_FRONT,
    VIEW_SIDE
} ViewMode;

// Editor material structure
typedef struct EditorMaterial {
    Vector3 ambient;
    Vector3 diffuse;
    Vector3 specular;
    float shininess;
} EditorMaterial;

// Editor object structure
typedef struct EditorObject {
    char name[256];
    ObjectType type;
    Vector3 position;
    Vector3 rotation;
    Vector3 scale;
    EditorMaterial material;
    bool is_collidable;
    bool is_visible;
    bool is_selected;
    
    // Type-specific parameters
    union {
        struct { float radius; } sphere;
        struct { float width; float height; } plane;
        struct { float radius; float height; } cylinder;
        struct { float major_radius; float minor_radius; } torus;
    } params;
    
    // NURBS surface data
    NURBSSurface *nurbs_surface;
    TessellatedSurface *tessellated_surface;
    
    // IGES model data (if loaded from IGES file)
    void *iges_data;
    
    struct EditorObject *next;
} EditorObject;

// Editor light structure
typedef struct EditorLight {
    char name[256];
    LightType type;
    Vector3 position;
    Vector3 color;
    float intensity;
    Vector3 direction;  // For directional and spot lights
    float spot_angle;   // For spot lights
    bool is_visible;
    bool is_selected;
    
    struct EditorLight *next;
} EditorLight;

// Camera structure
typedef struct {
    Vector3 position;
    Vector3 target;
    Vector3 up;
    float fov;
    float near_plane;
    float far_plane;
    ViewMode view_mode;
} Camera;

// Undo/Redo system
typedef enum {
    ACTION_CREATE_OBJECT,
    ACTION_DELETE_OBJECT,
    ACTION_MOVE_OBJECT,
    ACTION_ROTATE_OBJECT,
    ACTION_SCALE_OBJECT,
    ACTION_MODIFY_MATERIAL,
    ACTION_CREATE_LIGHT,
    ACTION_DELETE_LIGHT,
    ACTION_MOVE_LIGHT
} ActionType;

typedef struct UndoAction {
    ActionType type;
    void *data;
    struct UndoAction *next;
} UndoAction;

// Main editor structure
typedef struct MapEditor {
    // GTK widgets
    GtkApplication *app;
    GtkWidget *main_window;
    GtkWidget *main_paned;
    GtkWidget *left_panel;
    GtkWidget *center_panel;
    GtkWidget *right_panel;
    
    // Menu and toolbar
    GtkWidget *menubar;
    GtkWidget *toolbar;
    GtkWidget *tool_buttons[4]; // select, move, rotate, scale
    
    // 3D viewport
    GtkWidget *viewport_area;
    GdkGLContext *gl_context;
    bool viewport_initialized;
    int viewport_width;
    int viewport_height;
    
    // Scene hierarchy
    GtkWidget *scene_tree;
    GtkTreeStore *tree_store;
    
    // Properties panel
    GtkWidget *properties_notebook;
    GtkWidget *transform_page;
    GtkWidget *material_page;
    GtkWidget *lighting_page;
    
    // Transform controls
    GtkWidget *pos_entries[3];  // X, Y, Z position
    GtkWidget *rot_entries[3];  // X, Y, Z rotation
    GtkWidget *scale_entries[3]; // X, Y, Z scale
    
    // Material controls
    GtkWidget *ambient_color_btn;
    GtkWidget *diffuse_color_btn;
    GtkWidget *specular_color_btn;
    GtkWidget *shininess_entry;
    
    // Light controls
    GtkWidget *light_type_combo;
    GtkWidget *intensity_entry;
    GtkWidget *light_color_btn;
    
    // Scene data
    EditorObject *objects;
    EditorLight *lights;
    EditorObject *selected_object;
    EditorLight *selected_light;
    
    // Editor state
    ToolMode current_tool;
    Camera camera;
    bool show_grid;
    bool show_wireframe;
    bool show_lighting;
    
    // File operations
    char current_file[1024];
    bool scene_modified;
    
    // Undo/Redo
    UndoAction *undo_stack;
    UndoAction *redo_stack;
    int undo_depth;
    
    // Mouse interaction
    bool mouse_dragging;
    int last_mouse_x;
    int last_mouse_y;
    
} MapEditor;

// Core editor functions
MapEditor* editor_create(GtkApplication *app);
void editor_destroy(MapEditor *editor);
void editor_show(MapEditor *editor);

// Scene management
void editor_new_scene(MapEditor *editor);
bool editor_load_scene(MapEditor *editor, const char *filename);
bool editor_save_scene(MapEditor *editor, const char *filename);
bool editor_export_for_game(MapEditor *editor, const char *filename);

// Object management
EditorObject* editor_create_object(MapEditor *editor, ObjectType type);
void editor_delete_object(MapEditor *editor, EditorObject *obj);
void editor_duplicate_object(MapEditor *editor, EditorObject *obj);
void editor_select_object(MapEditor *editor, EditorObject *obj);

// Light management
EditorLight* editor_create_light(MapEditor *editor, LightType type);
void editor_delete_light(MapEditor *editor, EditorLight *light);
void editor_select_light(MapEditor *editor, EditorLight *light);

// Tool operations
void editor_set_tool(MapEditor *editor, ToolMode tool);
void editor_transform_selected(MapEditor *editor, Vector3 translation, Vector3 rotation, Vector3 scale);

// Camera operations
void editor_set_camera_view(MapEditor *editor, ViewMode view);
void editor_reset_camera(MapEditor *editor);
void editor_focus_selected(MapEditor *editor);

// Viewport operations
void editor_render_viewport(MapEditor *editor);
void editor_handle_mouse_click(MapEditor *editor, int x, int y, int button);
void editor_handle_mouse_drag(MapEditor *editor, int x, int y);
void editor_handle_mouse_scroll(MapEditor *editor, int x, int y, float delta);

// UI updates
void editor_update_scene_tree(MapEditor *editor);
void editor_update_properties(MapEditor *editor);
void editor_update_transform_ui(MapEditor *editor);
void editor_update_material_ui(MapEditor *editor);
void editor_update_light_ui(MapEditor *editor);

// Undo/Redo
void editor_push_undo_action(MapEditor *editor, ActionType type, void *data);
void editor_undo(MapEditor *editor);
void editor_redo(MapEditor *editor);

// IGES support
bool editor_import_iges(MapEditor *editor, const char *filename);
bool editor_export_iges(MapEditor *editor, const char *filename);

// Missing function declarations
void editor_setup_actions(MapEditor *editor);
void editor_create_layout(MapEditor *editor);
void editor_create_toolbar(MapEditor *editor);
void editor_create_scene_hierarchy(MapEditor *editor);
void editor_create_properties_panel(MapEditor *editor);
void editor_create_viewport(MapEditor *editor);
void editor_update_tool_buttons(MapEditor *editor);

// Utility functions
const char* object_type_to_string(ObjectType type);
const char* light_type_to_string(LightType type);
Vector3 screen_to_world(MapEditor *editor, int screen_x, int screen_y);
EditorObject* editor_pick_object(MapEditor *editor, int screen_x, int screen_y);

#endif // EDITOR_H