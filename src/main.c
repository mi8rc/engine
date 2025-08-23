#include "fps_engine.h"
#include "map_loader.h"
#include <stdio.h>
#include <stdlib.h>

void create_demo_scene(FPSEngine *engine) {
    // Create some demo NURBS objects
    
    // Create a NURBS plane floor
    GameObject *floor = create_game_object("Floor", (Vector3){0.0f, -2.0f, 0.0f});
    NURBSSurface *floor_surface = create_nurbs_plane(20.0f, 20.0f);
    game_object_add_nurbs_surface(floor, floor_surface);
    
    // Set floor material
    Material floor_material = {
        .ambient = {0.1f, 0.3f, 0.1f},
        .diffuse = {0.3f, 0.8f, 0.3f},
        .specular = {0.1f, 0.1f, 0.1f},
        .shininess = 4.0f
    };
    game_object_set_material(floor, floor_material);
    world_add_object(&engine->world, floor);
    
    // Create some NURBS spheres
    for (int i = 0; i < 5; i++) {
        char name[32];
        sprintf(name, "Sphere_%d", i);
        
        Vector3 position = {
            (float)(i - 2) * 3.0f,
            0.0f,
            (float)(i % 2) * 3.0f - 1.5f
        };
        
        GameObject *sphere = create_game_object(name, position);
        NURBSSurface *sphere_surface = create_nurbs_sphere(1.0f);
        game_object_add_nurbs_surface(sphere, sphere_surface);
        
        // Random material colors
        Material sphere_material = {
            .ambient = {0.1f, 0.1f, 0.1f},
            .diffuse = {
                0.3f + (float)i * 0.15f,
                0.5f - (float)i * 0.1f,
                0.8f - (float)i * 0.1f
            },
            .specular = {1.0f, 1.0f, 1.0f},
            .shininess = 32.0f
        };
        game_object_set_material(sphere, sphere_material);
        world_add_object(&engine->world, sphere);
    }
    
    // Create some walls using NURBS planes
    GameObject *wall1 = create_game_object("Wall1", (Vector3){-10.0f, 3.0f, 0.0f});
    NURBSSurface *wall1_surface = create_nurbs_plane(2.0f, 6.0f);
    game_object_add_nurbs_surface(wall1, wall1_surface);
    
    Material wall_material = {
        .ambient = {0.2f, 0.2f, 0.3f},
        .diffuse = {0.6f, 0.6f, 0.8f},
        .specular = {0.3f, 0.3f, 0.3f},
        .shininess = 16.0f
    };
    game_object_set_material(wall1, wall_material);
    world_add_object(&engine->world, wall1);
    
    GameObject *wall2 = create_game_object("Wall2", (Vector3){10.0f, 3.0f, 0.0f});
    NURBSSurface *wall2_surface = create_nurbs_plane(2.0f, 6.0f);
    game_object_add_nurbs_surface(wall2, wall2_surface);
    game_object_set_material(wall2, wall_material);
    world_add_object(&engine->world, wall2);
    
    // Add some lights
    Light main_light = {
        .position = {0.0f, 8.0f, 0.0f},
        .color = {1.0f, 1.0f, 1.0f},
        .intensity = 1.5f,
        .type = 0 // Point light
    };
    world_add_light(&engine->world, &main_light);
    
    Light accent_light1 = {
        .position = {-5.0f, 3.0f, 5.0f},
        .color = {1.0f, 0.7f, 0.3f},
        .intensity = 0.8f,
        .type = 0
    };
    world_add_light(&engine->world, &accent_light1);
    
    Light accent_light2 = {
        .position = {5.0f, 3.0f, -5.0f},
        .color = {0.3f, 0.7f, 1.0f},
        .intensity = 0.8f,
        .type = 0
    };
    world_add_light(&engine->world, &accent_light2);
}

void game_object_set_material(GameObject *object, Material material) {
    object->material = material;
}

void world_update(World *world, float delta_time) {
    // Update animations or object states here
    // For now, just rotate some spheres
    for (int i = 0; i < world->num_objects; i++) {
        GameObject *obj = &world->objects[i];
        if (strstr(obj->name, "Sphere") != NULL) {
            obj->rotation.y += delta_time * 30.0f; // Rotate 30 degrees per second
            if (obj->rotation.y > 360.0f) obj->rotation.y -= 360.0f;
        }
    }
}

int main(int argc, char *argv[]) {
    printf("NURBS FPS Game Engine\n");
    printf("Controls:\n");
    printf("  WASD - Move\n");
    printf("  Mouse - Look around\n");
    printf("  Space - Move up\n");
    printf("  Shift - Move down\n");
    printf("  Escape - Exit\n\n");
    
    FPSEngine engine;
    
    // Initialize engine
    if (fps_engine_init(&engine, 1200, 800, "NURBS FPS Game") != 0) {
        fprintf(stderr, "Failed to initialize FPS engine\n");
        return -1;
    }
    
    // Check if a map file was provided
    if (argc > 1) {
        printf("Loading map file: %s\n", argv[1]);
        
        MapData map_data;
        if (load_map_file(argv[1], &map_data) == 0) {
            create_game_objects_from_map(&engine, &map_data);
            free_map_data(&map_data);
            printf("Map loaded successfully!\n");
        } else {
            printf("Failed to load map file. Using demo scene instead.\n");
            create_demo_scene(&engine);
        }
    } else {
        printf("No map file specified. Using demo scene.\n");
        printf("Usage: %s [map_file.map]\n", argv[0]);
        create_demo_scene(&engine);
    }
    
    printf("Engine initialized successfully. Starting game loop...\n");
    
    // Run game loop
    fps_engine_run(&engine);
    
    // Cleanup
    fps_engine_cleanup(&engine);
    
    printf("Game engine shut down successfully.\n");
    return 0;
}