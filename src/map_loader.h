#ifndef MAP_LOADER_H
#define MAP_LOADER_H

#include "fps_engine.h"
#include <json-c/json.h>

// Map file format structures
typedef struct {
    char name[64];
    char type[32];
    Vector3 position;
    Vector3 rotation;
    Vector3 scale;
    Material material;
    bool is_collidable;
    
    // Type-specific parameters
    union {
        struct { float radius; } sphere;
        struct { float width, height; } plane;
        struct { float radius, height; } cylinder;
        struct { float major_radius, minor_radius; } torus;
    } params;
} MapObject;

typedef struct {
    char name[64];
    int type; // 0=point, 1=directional, 2=spot
    Vector3 position;
    Vector3 color;
    float intensity;
    Vector3 direction;
    float spot_angle;
} MapLight;

typedef struct {
    char version[16];
    MapObject *objects;
    int num_objects;
    MapLight *lights;
    int num_lights;
} MapData;

// Function declarations
int load_map_file(const char *filename, MapData *map_data);
void free_map_data(MapData *map_data);
int create_game_objects_from_map(FPSEngine *engine, MapData *map_data);
NURBSSurface* create_nurbs_from_map_object(MapObject *obj);

// JSON parsing helpers
Vector3 parse_vector3_array(json_object *array);
Material parse_material_object(json_object *material_obj);

#endif // MAP_LOADER_H