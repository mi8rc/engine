# NURBS FPS Game Engine

A complete First-Person Shooter game engine that uses **ONLY NURBS surfaces** for all geometry rendering, with lighting and collision detection support. Includes a Python map editor similar to Roblox Studio.

## 🚨 IMPORTANT: NO POLYGONS!

This engine is built entirely around NURBS (Non-Uniform Rational B-Splines) surfaces. **NO POLYGONAL MESHES ARE USED**. All geometry is mathematically defined and tessellated from NURBS surfaces at runtime.

## Features

### C Game Engine
- **Pure NURBS Rendering**: All geometry uses NURBS surfaces (spheres, planes, cylinders, tori)
- **Advanced Lighting**: Multi-light Phong shading model with point, directional, and spot lights
- **NURBS Collision Detection**: Ray-surface intersection using tessellated NURBS surfaces
- **FPS Controls**: Full first-person camera with WASD movement and mouse look
- **OpenGL Integration**: Modern OpenGL 3.3+ with shader-based rendering
- **Map Loading**: Load scenes created in the Python editor

### Python Map Editor
- **Roblox Studio-like Interface**: Familiar 3D editing environment
- **NURBS Object Placement**: Add spheres, planes, cylinders, and tori
- **Light Management**: Place and configure point, directional, and spot lights
- **Material Editor**: Adjust ambient, diffuse, specular properties and shininess
- **Scene Hierarchy**: Tree view of all objects and lights
- **Property Inspector**: Real-time editing of transform and material properties
- **Export to Game**: Save scenes in format compatible with C engine

## Directory Structure

```
workspace/
├── src/                    # C game engine source
│   ├── nurbs.h/.c         # Core NURBS mathematics and rendering
│   ├── fps_engine.h/.c    # Main game engine and FPS controls
│   ├── math_utils.c       # Matrix operations and shader management
│   ├── map_loader.h/.c    # Map file loading from Python editor
│   └── main.c             # Main game entry point
├── map_editor/            # Python map editor
│   ├── nurbs_editor.py    # Main editor application
│   └── requirements.txt   # Python dependencies
├── Makefile              # Build system
└── README.md             # This file
```

## Installation

### System Dependencies

**Ubuntu/Debian:**
```bash
make install-deps
```

**Fedora/RHEL:**
```bash
make install-deps-fedora
```

**Arch Linux:**
```bash
make install-deps-arch
```

### Python Editor Dependencies
```bash
cd map_editor
pip install -r requirements.txt
```

## Building

### Headless Build (Perfect for CI/CD)
```bash
make -f Makefile.headless headless
```

### Test NURBS Mathematics (No GPU Required)
```bash
make -f Makefile.headless test
```

### Full CI/CD Test Suite
```bash
make -f Makefile.headless ci-test
```

### Performance Benchmark
```bash
make -f Makefile.headless benchmark
```

### Clean Build Files
```bash
make -f Makefile.headless clean
```

## Usage

### Running the Headless Engine (CI/CD Mode)

**Test NURBS mathematics:**
```bash
./bin/nurbs_headless_test
```

**With performance benchmark:**
```bash
make -f Makefile.headless benchmark
```

### For Systems with GPU

For systems with proper GPU support, you can build the full OpenGL version by installing GPU drivers and using the original approach. However, the headless version is perfect for:

- ✅ **GitHub Actions CI/CD**
- ✅ **Docker containers**
- ✅ **Headless servers**
- ✅ **Testing environments**
- ✅ **Mathematical validation**

### Using the Map Editor

**Start the editor:**
```bash
cd map_editor
python3 nurbs_editor.py
```

**Creating a Scene:**
1. Use the **Insert** menu to add NURBS objects and lights
2. Select objects in the **Scene Hierarchy**
3. Modify properties in the **Properties** panel
4. Use tools in the toolbar for selection, movement, rotation, and scaling
5. Export via **File → Export for Game...**

## NURBS Mathematics

The engine implements full NURBS surface evaluation:

- **B-spline basis functions** using Cox-de Boor recursion
- **Rational surface evaluation** with homogeneous coordinates
- **Partial derivative calculation** for normal vectors
- **Tessellation to triangles** for OpenGL rendering
- **Collision detection** via ray-surface intersection

### Supported NURBS Primitives

1. **Sphere** - Rational Bézier sphere patches
2. **Plane** - Linear NURBS surface 
3. **Cylinder** - Circular rational curves extruded linearly
4. **Torus** - Rational surface of revolution

## Lighting Model

The engine uses Phong shading with:
- **Ambient lighting** - Global illumination
- **Diffuse lighting** - Lambert's cosine law
- **Specular highlights** - Phong reflection model
- **Multiple light sources** - Up to 32 lights
- **Attenuation** - Distance-based light falloff

## Map File Format

Maps are exported as JSON with this structure:

```json
{
  "version": "1.0",
  "objects": [
    {
      "name": "Sphere_1",
      "type": "sphere",
      "position": [0.0, 0.0, 0.0],
      "rotation": [0.0, 0.0, 0.0],
      "scale": [1.0, 1.0, 1.0],
      "material": {
        "ambient": [0.2, 0.2, 0.2],
        "diffuse": [0.8, 0.8, 0.8],
        "specular": [1.0, 1.0, 1.0],
        "shininess": 32.0
      },
      "collidable": true,
      "parameters": {"radius": 1.0}
    }
  ],
  "lights": [
    {
      "name": "Point_Light_1",
      "type": 0,
      "position": [0.0, 5.0, 0.0],
      "color": [1.0, 1.0, 1.0],
      "intensity": 1.0,
      "direction": [0.0, -1.0, 0.0],
      "spot_angle": 45.0
    }
  ]
}
```

## Performance Notes

- **Tessellation Resolution**: Adjust in `tessellate_nurbs_surface()` calls
- **Collision Precision**: Modify tessellation resolution for collision surfaces
- **Light Count**: Performance scales with number of lights (max 32)
- **Surface Complexity**: Higher-degree NURBS surfaces are more expensive

## Architecture Overview

```
Python Editor → Export .map → C Engine → NURBS Math → OpenGL → Display
                                    ↓
                             Collision Detection
                                    ↓
                              Physics Response
```

## Contributing

When adding new NURBS surface types:

1. Add creation function to `nurbs.c`
2. Update `ObjectType` enum in Python editor
3. Add parsing in `map_loader.c`
4. Test collision detection

## License

This project demonstrates advanced NURBS mathematics in real-time applications. Use for educational and research purposes.

## References

- "The NURBS Book" by Piegl and Tiller
- OpenGL Programming Guide
- Computer Graphics: Principles and Practice

---

**Remember: This engine uses ONLY NURBS surfaces. No polygons! 🎯**