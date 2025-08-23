#include "map_loader.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

// Load map file exported from Python editor
int load_map_file(const char *filename, MapData *map_data) {
    FILE *file = fopen(filename, "r");
    if (!file) {
        fprintf(stderr, "Failed to open map file: %s\n", filename);
        return -1;
    }
    
    // Read file into string
    fseek(file, 0, SEEK_END);
    long file_size = ftell(file);
    fseek(file, 0, SEEK_SET);
    
    char *json_string = malloc(file_size + 1);
    fread(json_string, 1, file_size, file);
    json_string[file_size] = '\0';
    fclose(file);
    
    // Parse JSON
    json_object *root = json_tokener_parse(json_string);
    free(json_string);
    
    if (!root) {
        fprintf(stderr, "Failed to parse JSON in map file\n");
        return -1;
    }
    
    // Initialize map data
    memset(map_data, 0, sizeof(MapData));
    
    // Parse version
    json_object *version_obj;
    if (json_object_object_get_ex(root, "version", &version_obj)) {
        strncpy(map_data->version, json_object_get_string(version_obj), 
                sizeof(map_data->version) - 1);
    }
    
    // Parse objects
    json_object *objects_array;
    if (json_object_object_get_ex(root, "objects", &objects_array)) {
        map_data->num_objects = json_object_array_length(objects_array);
        map_data->objects = malloc(sizeof(MapObject) * map_data->num_objects);
        
        for (int i = 0; i < map_data->num_objects; i++) {
            json_object *obj = json_object_array_get_idx(objects_array, i);
            MapObject *map_obj = &map_data->objects[i];
            
            // Parse object properties
            json_object *name_obj, *type_obj, *pos_obj, *rot_obj, *scale_obj, 
                       *material_obj, *collidable_obj, *params_obj;
            
            if (json_object_object_get_ex(obj, "name", &name_obj)) {
                strncpy(map_obj->name, json_object_get_string(name_obj), 
                        sizeof(map_obj->name) - 1);
            }
            
            if (json_object_object_get_ex(obj, "type", &type_obj)) {
                strncpy(map_obj->type, json_object_get_string(type_obj), 
                        sizeof(map_obj->type) - 1);
            }
            
            if (json_object_object_get_ex(obj, "position", &pos_obj)) {
                map_obj->position = parse_vector3_array(pos_obj);
            }
            
            if (json_object_object_get_ex(obj, "rotation", &rot_obj)) {
                map_obj->rotation = parse_vector3_array(rot_obj);
            }
            
            if (json_object_object_get_ex(obj, "scale", &scale_obj)) {
                map_obj->scale = parse_vector3_array(scale_obj);
            }
            
            if (json_object_object_get_ex(obj, "material", &material_obj)) {
                map_obj->material = parse_material_object(material_obj);
            }
            
            if (json_object_object_get_ex(obj, "collidable", &collidable_obj)) {
                map_obj->is_collidable = json_object_get_boolean(collidable_obj);
            }
            
            // Parse type-specific parameters
            if (json_object_object_get_ex(obj, "parameters", &params_obj)) {
                if (strcmp(map_obj->type, "sphere") == 0) {
                    json_object *radius_obj;
                    if (json_object_object_get_ex(params_obj, "radius", &radius_obj)) {
                        map_obj->params.sphere.radius = json_object_get_double(radius_obj);
                    }
                } else if (strcmp(map_obj->type, "plane") == 0) {
                    json_object *width_obj, *height_obj;
                    if (json_object_object_get_ex(params_obj, "width", &width_obj)) {
                        map_obj->params.plane.width = json_object_get_double(width_obj);
                    }
                    if (json_object_object_get_ex(params_obj, "height", &height_obj)) {
                        map_obj->params.plane.height = json_object_get_double(height_obj);
                    }
                } else if (strcmp(map_obj->type, "cylinder") == 0) {
                    json_object *radius_obj, *height_obj;
                    if (json_object_object_get_ex(params_obj, "radius", &radius_obj)) {
                        map_obj->params.cylinder.radius = json_object_get_double(radius_obj);
                    }
                    if (json_object_object_get_ex(params_obj, "height", &height_obj)) {
                        map_obj->params.cylinder.height = json_object_get_double(height_obj);
                    }
                } else if (strcmp(map_obj->type, "torus") == 0) {
                    json_object *major_obj, *minor_obj;
                    if (json_object_object_get_ex(params_obj, "major_radius", &major_obj)) {
                        map_obj->params.torus.major_radius = json_object_get_double(major_obj);
                    }
                    if (json_object_object_get_ex(params_obj, "minor_radius", &minor_obj)) {
                        map_obj->params.torus.minor_radius = json_object_get_double(minor_obj);
                    }
                }
            }
        }
    }
    
    // Parse lights
    json_object *lights_array;
    if (json_object_object_get_ex(root, "lights", &lights_array)) {
        map_data->num_lights = json_object_array_length(lights_array);
        map_data->lights = malloc(sizeof(MapLight) * map_data->num_lights);
        
        for (int i = 0; i < map_data->num_lights; i++) {
            json_object *light_obj = json_object_array_get_idx(lights_array, i);
            MapLight *map_light = &map_data->lights[i];
            
            json_object *name_obj, *type_obj, *pos_obj, *color_obj, 
                       *intensity_obj, *dir_obj, *angle_obj;
            
            if (json_object_object_get_ex(light_obj, "name", &name_obj)) {
                strncpy(map_light->name, json_object_get_string(name_obj), 
                        sizeof(map_light->name) - 1);
            }
            
            if (json_object_object_get_ex(light_obj, "type", &type_obj)) {
                map_light->type = json_object_get_int(type_obj);
            }
            
            if (json_object_object_get_ex(light_obj, "position", &pos_obj)) {
                map_light->position = parse_vector3_array(pos_obj);
            }
            
            if (json_object_object_get_ex(light_obj, "color", &color_obj)) {
                map_light->color = parse_vector3_array(color_obj);
            }
            
            if (json_object_object_get_ex(light_obj, "intensity", &intensity_obj)) {
                map_light->intensity = json_object_get_double(intensity_obj);
            }
            
            if (json_object_object_get_ex(light_obj, "direction", &dir_obj)) {
                map_light->direction = parse_vector3_array(dir_obj);
            }
            
            if (json_object_object_get_ex(light_obj, "spot_angle", &angle_obj)) {
                map_light->spot_angle = json_object_get_double(angle_obj);
            }
        }
    }
    
    json_object_put(root);
    
    printf("Loaded map: %d objects, %d lights\n", map_data->num_objects, map_data->num_lights);
    return 0;
}

// Helper function to parse Vector3 from JSON array
Vector3 parse_vector3_array(json_object *array) {
    Vector3 result = {0.0f, 0.0f, 0.0f};
    
    if (json_object_is_type(array, json_type_array)) {
        if (json_object_array_length(array) >= 3) {
            result.x = json_object_get_double(json_object_array_get_idx(array, 0));
            result.y = json_object_get_double(json_object_array_get_idx(array, 1));
            result.z = json_object_get_double(json_object_array_get_idx(array, 2));
        }
    }
    
    return result;
}

// Helper function to parse Material from JSON object
Material parse_material_object(json_object *material_obj) {
    Material material = {
        .ambient = {0.2f, 0.2f, 0.2f},
        .diffuse = {0.8f, 0.8f, 0.8f},
        .specular = {1.0f, 1.0f, 1.0f},
        .shininess = 32.0f
    };
    
    json_object *ambient_obj, *diffuse_obj, *specular_obj, *shininess_obj;
    
    if (json_object_object_get_ex(material_obj, "ambient", &ambient_obj)) {
        material.ambient = parse_vector3_array(ambient_obj);
    }
    
    if (json_object_object_get_ex(material_obj, "diffuse", &diffuse_obj)) {
        material.diffuse = parse_vector3_array(diffuse_obj);
    }
    
    if (json_object_object_get_ex(material_obj, "specular", &specular_obj)) {
        material.specular = parse_vector3_array(specular_obj);
    }
    
    if (json_object_object_get_ex(material_obj, "shininess", &shininess_obj)) {
        material.shininess = json_object_get_double(shininess_obj);
    }
    
    return material;
}

// Create game objects from loaded map data
int create_game_objects_from_map(FPSEngine *engine, MapData *map_data) {
    // Create objects
    for (int i = 0; i < map_data->num_objects; i++) {
        MapObject *map_obj = &map_data->objects[i];
        
        GameObject *game_obj = create_game_object(map_obj->name, map_obj->position);
        game_obj->rotation = map_obj->rotation;
        game_obj->scale = map_obj->scale;
        game_obj->material = map_obj->material;
        game_obj->is_collidable = map_obj->is_collidable;
        
        // Create NURBS surface based on type
        NURBSSurface *surface = create_nurbs_from_map_object(map_obj);
        if (surface) {
            game_object_add_nurbs_surface(game_obj, surface);
        }
        
        world_add_object(&engine->world, game_obj);
        free(game_obj); // world_add_object makes a copy
    }
    
    // Create lights
    for (int i = 0; i < map_data->num_lights; i++) {
        MapLight *map_light = &map_data->lights[i];
        
        Light light = {
            .position = map_light->position,
            .color = map_light->color,
            .intensity = map_light->intensity,
            .type = map_light->type,
            .direction = map_light->direction,
            .spot_angle = map_light->spot_angle
        };
        
        world_add_light(&engine->world, &light);
    }
    
    return 0;
}

// Create NURBS surface from map object
NURBSSurface* create_nurbs_from_map_object(MapObject *obj) {
    if (strcmp(obj->type, "sphere") == 0) {
        return create_nurbs_sphere(obj->params.sphere.radius);
    } else if (strcmp(obj->type, "plane") == 0) {
        return create_nurbs_plane(obj->params.plane.width, obj->params.plane.height);
    } else if (strcmp(obj->type, "cylinder") == 0) {
        return create_nurbs_cylinder(obj->params.cylinder.radius, obj->params.cylinder.height);
    } else if (strcmp(obj->type, "torus") == 0) {
        return create_nurbs_torus(obj->params.torus.major_radius, obj->params.torus.minor_radius);
    }
    
    return NULL;
}

// Free map data
void free_map_data(MapData *map_data) {
    if (map_data->objects) {
        free(map_data->objects);
        map_data->objects = NULL;
    }
    
    if (map_data->lights) {
        free(map_data->lights);
        map_data->lights = NULL;
    }
    
    map_data->num_objects = 0;
    map_data->num_lights = 0;
}