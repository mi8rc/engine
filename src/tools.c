#include "tools.h"
#include "renderer.h"
#include "window.h"

ToolState g_tool_state;

void tools_init(void) {
    g_tool_state.current_tool = TOOL_SELECT;
    g_tool_state.is_active = FALSE;
    g_tool_state.target_object = NULL;
    g_tool_state.selected_control_point = -1;
    g_tool_state.dragging_control_point = FALSE;
}

void tools_cleanup(void) {
    // Nothing to cleanup for now
}

void tools_set_current(ToolType tool) {
    g_tool_state.current_tool = tool;
    g_tool_state.is_active = FALSE;
    g_tool_state.target_object = NULL;
    
    // Activate tool-specific behavior
    switch (tool) {
        case TOOL_SELECT:
            tool_select_activate();
            break;
        case TOOL_MOVE:
            tool_move_activate();
            break;
        case TOOL_CREATE_CURVE:
            tool_create_curve_activate();
            break;
        default:
            break;
    }
}

ToolType tools_get_current(void) {
    return g_tool_state.current_tool;
}

void tools_handle_mouse_press(int x, int y, int button) {
    switch (g_tool_state.current_tool) {
        case TOOL_SELECT:
            tool_select_mouse_press(x, y, button);
            break;
        case TOOL_MOVE:
            tool_move_mouse_press(x, y, button);
            break;
        case TOOL_CREATE_CURVE:
            tool_create_curve_mouse_press(x, y, button);
            break;
        default:
            break;
    }
}

void tools_handle_mouse_release(int x, int y, int button) {
    switch (g_tool_state.current_tool) {
        case TOOL_SELECT:
            tool_select_mouse_release(x, y, button);
            break;
        case TOOL_MOVE:
            tool_move_mouse_release(x, y, button);
            break;
        default:
            break;
    }
}

void tools_handle_mouse_move(int x, int y) {
    switch (g_tool_state.current_tool) {
        case TOOL_SELECT:
            tool_select_mouse_move(x, y);
            break;
        case TOOL_MOVE:
            tool_move_mouse_move(x, y);
            break;
        default:
            break;
    }
}

void tools_handle_key_press(int key) {
    // Handle keyboard shortcuts
    switch (key) {
        case 'S':
        case 's':
            tools_set_current(TOOL_SELECT);
            break;
        case 'M':
        case 'm':
            tools_set_current(TOOL_MOVE);
            break;
        case 'C':
        case 'c':
            tools_set_current(TOOL_CREATE_CURVE);
            break;
        default:
            break;
    }
}

// Select tool implementation
void tool_select_activate(void) {
    // Set cursor to default
    printf("Select tool activated\n");
}

void tool_select_mouse_press(int x, int y, int button) {
    if (button == 1) { // Left mouse button
        MapObject *object = pick_object_at_screen_pos(x, y);
        
        if (object) {
            // Check if clicking on a control point
            int cp_index = pick_control_point_at_screen_pos(object, x, y);
            
            if (cp_index >= 0) {
                g_tool_state.selected_control_point = cp_index;
                g_tool_state.dragging_control_point = TRUE;
                g_tool_state.target_object = object;
            } else {
                // Select the object
                if (g_editor_state) {
                    // Clear previous selection
                    GList *current = g_editor_state->objects;
                    while (current) {
                        MapObject *obj = (MapObject*)current->data;
                        obj->selected = FALSE;
                        current = current->next;
                    }
                    
                    object->selected = TRUE;
                    g_list_free(g_editor_state->selected_objects);
                    g_editor_state->selected_objects = g_list_append(NULL, object);
                }
            }
        } else {
            // Clear selection
            if (g_editor_state) {
                GList *current = g_editor_state->objects;
                while (current) {
                    MapObject *obj = (MapObject*)current->data;
                    obj->selected = FALSE;
                    current = current->next;
                }
                g_list_free(g_editor_state->selected_objects);
                g_editor_state->selected_objects = NULL;
            }
        }
        
        g_tool_state.is_active = TRUE;
        screen_to_world(x, y, g_tool_state.start_pos);
    }
}

void tool_select_mouse_move(int x, int y) {
    if (g_tool_state.is_active && g_tool_state.dragging_control_point) {
        float world_pos[3];
        screen_to_world(x, y, world_pos);
        
        update_control_point_position(g_tool_state.target_object, 
                                    g_tool_state.selected_control_point, 
                                    world_pos);
    }
}

void tool_select_mouse_release(int x, int y, int button) {
    if (button == 1) {
        g_tool_state.is_active = FALSE;
        g_tool_state.dragging_control_point = FALSE;
        g_tool_state.selected_control_point = -1;
    }
}

// Move tool implementation
void tool_move_activate(void) {
    printf("Move tool activated\n");
}

void tool_move_mouse_press(int x, int y, int button) {
    if (button == 1 && g_editor_state && g_editor_state->selected_objects) {
        g_tool_state.is_active = TRUE;
        g_tool_state.target_object = (MapObject*)g_editor_state->selected_objects->data;
        screen_to_world(x, y, g_tool_state.start_pos);
        
        // Store current position
        g_tool_state.current_pos[0] = g_tool_state.target_object->position[0];
        g_tool_state.current_pos[1] = g_tool_state.target_object->position[1];
        g_tool_state.current_pos[2] = g_tool_state.target_object->position[2];
    }
}

void tool_move_mouse_move(int x, int y) {
    if (g_tool_state.is_active && g_tool_state.target_object) {
        float world_pos[3];
        screen_to_world(x, y, world_pos);
        
        // Calculate delta
        float delta[3];
        delta[0] = world_pos[0] - g_tool_state.start_pos[0];
        delta[1] = world_pos[1] - g_tool_state.start_pos[1];
        delta[2] = world_pos[2] - g_tool_state.start_pos[2];
        
        // Update object position
        g_tool_state.target_object->position[0] = g_tool_state.current_pos[0] + delta[0];
        g_tool_state.target_object->position[1] = g_tool_state.current_pos[1] + delta[1];
        g_tool_state.target_object->position[2] = g_tool_state.current_pos[2] + delta[2];
    }
}

void tool_move_mouse_release(int x, int y, int button) {
    if (button == 1) {
        g_tool_state.is_active = FALSE;
        g_tool_state.target_object = NULL;
    }
}

// Create curve tool implementation
void tool_create_curve_activate(void) {
    printf("Create curve tool activated\n");
}

void tool_create_curve_mouse_press(int x, int y, int button) {
    if (button == 1) {
        // Create a new curve at the clicked position
        float world_pos[3];
        screen_to_world(x, y, world_pos);
        
        MapObject *object = map_object_new(OBJECT_CURVE, "New Curve");
        NurbsCurve *curve = nurbs_curve_new(3, 4);
        
        // Set control points around the clicked position
        nurbs_curve_set_control_point(curve, 0, world_pos[0] - 1.0f, world_pos[1], world_pos[2], 1.0f);
        nurbs_curve_set_control_point(curve, 1, world_pos[0] - 0.3f, world_pos[1] + 0.5f, world_pos[2], 1.0f);
        nurbs_curve_set_control_point(curve, 2, world_pos[0] + 0.3f, world_pos[1] + 0.5f, world_pos[2], 1.0f);
        nurbs_curve_set_control_point(curve, 3, world_pos[0] + 1.0f, world_pos[1], world_pos[2], 1.0f);
        
        object->data.curve = curve;
        
        if (g_editor_state) {
            g_editor_state->objects = g_list_append(g_editor_state->objects, object);
        }
        
        // Switch back to select tool
        tools_set_current(TOOL_SELECT);
    }
}

// Utility functions
MapObject *pick_object_at_screen_pos(int x, int y) {
    // Simplified object picking - in a real implementation, this would
    // use OpenGL picking or ray casting
    
    if (!g_editor_state || !g_editor_state->objects) return NULL;
    
    float world_pos[3];
    screen_to_world(x, y, world_pos);
    
    // Find the closest object to the click position
    MapObject *closest_object = NULL;
    float closest_distance = FLT_MAX;
    
    GList *current = g_editor_state->objects;
    while (current) {
        MapObject *object = (MapObject*)current->data;
        
        if (!object->visible) {
            current = current->next;
            continue;
        }
        
        // Simple distance check to object position
        float dx = object->position[0] - world_pos[0];
        float dy = object->position[1] - world_pos[1];
        float dz = object->position[2] - world_pos[2];
        float distance = sqrtf(dx*dx + dy*dy + dz*dz);
        
        if (distance < closest_distance && distance < 2.0f) { // 2.0 unit selection radius
            closest_distance = distance;
            closest_object = object;
        }
        
        current = current->next;
    }
    
    return closest_object;
}

int pick_control_point_at_screen_pos(MapObject *object, int x, int y) {
    if (!object || object->type != OBJECT_CURVE || !object->data.curve) {
        return -1;
    }
    
    float world_pos[3];
    screen_to_world(x, y, world_pos);
    
    NurbsCurve *curve = object->data.curve;
    
    // Check each control point
    for (int i = 0; i < curve->num_control_points; i++) {
        float cp_world[3];
        cp_world[0] = object->position[0] + curve->control_points[i].x;
        cp_world[1] = object->position[1] + curve->control_points[i].y;
        cp_world[2] = object->position[2] + curve->control_points[i].z;
        
        float dx = cp_world[0] - world_pos[0];
        float dy = cp_world[1] - world_pos[1];
        float dz = cp_world[2] - world_pos[2];
        float distance = sqrtf(dx*dx + dy*dy + dz*dz);
        
        if (distance < 0.5f) { // 0.5 unit selection radius for control points
            return i;
        }
    }
    
    return -1;
}

void update_control_point_position(MapObject *object, int cp_index, float *world_pos) {
    if (!object || object->type != OBJECT_CURVE || !object->data.curve) {
        return;
    }
    
    NurbsCurve *curve = object->data.curve;
    
    if (cp_index < 0 || cp_index >= curve->num_control_points) {
        return;
    }
    
    // Convert world position to object-relative position
    float local_pos[3];
    local_pos[0] = world_pos[0] - object->position[0];
    local_pos[1] = world_pos[1] - object->position[1];
    local_pos[2] = world_pos[2] - object->position[2];
    
    nurbs_curve_set_control_point(curve, cp_index, 
                                 local_pos[0], local_pos[1], local_pos[2],
                                 curve->control_points[cp_index].w);
}