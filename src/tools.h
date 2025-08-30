#ifndef TOOLS_H
#define TOOLS_H

#include "main.h"
#include "nurbs.h"

typedef enum {
    TOOL_SELECT = 0,
    TOOL_MOVE,
    TOOL_ROTATE,
    TOOL_SCALE,
    TOOL_CREATE_CURVE,
    TOOL_CREATE_SURFACE,
    TOOL_EDIT_CONTROL_POINTS
} ToolType;

typedef struct {
    ToolType current_tool;
    gboolean is_active;
    
    // Tool state
    float start_pos[3];
    float current_pos[3];
    MapObject *target_object;
    
    // Control point editing
    int selected_control_point;
    gboolean dragging_control_point;
    
} ToolState;

// Function declarations
void tools_init(void);
void tools_cleanup(void);
void tools_set_current(ToolType tool);
ToolType tools_get_current(void);

// Tool operations
void tools_handle_mouse_press(int x, int y, int button);
void tools_handle_mouse_release(int x, int y, int button);
void tools_handle_mouse_move(int x, int y);
void tools_handle_key_press(int key);

// Tool-specific functions
void tool_select_activate(void);
void tool_select_mouse_press(int x, int y, int button);
void tool_select_mouse_move(int x, int y);
void tool_select_mouse_release(int x, int y, int button);

void tool_move_activate(void);
void tool_move_mouse_press(int x, int y, int button);
void tool_move_mouse_move(int x, int y);
void tool_move_mouse_release(int x, int y, int button);

void tool_create_curve_activate(void);
void tool_create_curve_mouse_press(int x, int y, int button);

// Utility functions
MapObject *pick_object_at_screen_pos(int x, int y);
int pick_control_point_at_screen_pos(MapObject *object, int x, int y);
void update_control_point_position(MapObject *object, int cp_index, float *world_pos);

extern ToolState g_tool_state;

#endif // TOOLS_H