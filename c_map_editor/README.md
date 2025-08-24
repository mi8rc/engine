# NURBS Map Editor

A comprehensive Roblox Studio-like editor for NURBS-based FPS games with full IGES file format support.

## Overview

The NURBS Map Editor is a powerful 3D modeling and level design tool specifically designed for creating maps for NURBS-based FPS games. It provides an intuitive Roblox Studio-like interface while leveraging the mathematical precision of NURBS (Non-Uniform Rational B-Splines) for smooth, scalable geometry.

## Features

### Core Features
- **Roblox Studio-like Interface**: Familiar and intuitive user interface
- **NURBS-based Geometry**: Mathematically precise curved surfaces
- **Full IGES Support**: Import and export industry-standard IGES files
- **Real-time 3D Viewport**: Interactive OpenGL-powered 3D rendering
- **Scene Hierarchy Management**: Organize objects in a tree structure
- **Properties Panel**: Edit object transforms, materials, and lighting
- **Undo/Redo System**: Full operation history with unlimited undo levels

### NURBS Primitives
- **Sphere**: Perfect spherical surfaces
- **Plane**: Flat rectangular surfaces
- **Cylinder**: Cylindrical surfaces with customizable dimensions
- **Torus**: Donut-shaped surfaces with major/minor radius control
- **Custom Surfaces**: Import complex NURBS surfaces from IGES files

### Lighting System
- **Point Lights**: Omnidirectional light sources
- **Directional Lights**: Sun-like parallel lighting
- **Spot Lights**: Cone-shaped directional lighting with falloff

### Material System
- **Ambient Color**: Base color in shadowed areas
- **Diffuse Color**: Primary surface color
- **Specular Color**: Highlight color and shininess
- **Real-time Preview**: See material changes instantly

### File Operations
- **Project Files**: Save and load complete scenes
- **IGES Import/Export**: Full compatibility with CAD software
- **Game Export**: Optimized format for FPS game engines

### Camera Controls
- **Orbit**: Rotate around selected objects
- **Pan**: Move the view horizontally and vertically
- **Zoom**: Smooth zoom in/out with mouse wheel
- **Preset Views**: Top, Front, Side, and Perspective views

## Build Status

[![CI/CD](https://github.com/your-repo/nurbs-map-editor/actions/workflows/ci-cd.yml/badge.svg)](https://github.com/your-repo/nurbs-map-editor/actions/workflows/ci-cd.yml)
[![License: MIT](https://img.shields.io/badge/License-MIT-yellow.svg)](https://opensource.org/licenses/MIT)
[![Platform](https://img.shields.io/badge/platform-Linux%20%7C%20Windows-blue)](https://github.com/your-repo/nurbs-map-editor/releases)

## Installation

### Automated Builds

Pre-built binaries are available for download from the [Releases](https://github.com/your-repo/nurbs-map-editor/releases) page:

- **Linux**: Debian packages (.deb) and source packages (.tar.gz)
- **Windows**: ZIP packages for 64-bit and 32-bit systems with all dependencies included

### Prerequisites

The NURBS Map Editor requires the following dependencies:

#### Ubuntu/Debian
```bash
sudo apt-get update
sudo apt-get install build-essential pkg-config libgtk-3-dev libgl1-mesa-dev libglu1-mesa-dev libglfw3-dev libglew-dev
```

#### Fedora/CentOS/RHEL
```bash
sudo dnf install gcc gcc-c++ make pkgconfig gtk3-devel mesa-libGL-devel mesa-libGLU-devel glfw-devel glew-devel
```

#### Arch Linux
```bash
sudo pacman -S base-devel pkgconf gtk3 mesa glu glfw glew
```

### Building from Source

#### Quick Build (Cross-Platform)

Use the automated build script:
```bash
git clone https://github.com/your-repo/nurbs-map-editor.git
cd nurbs-map-editor/c_map_editor
./build.sh all
```

#### Manual Build

1. Clone the repository:
```bash
git clone https://github.com/your-repo/nurbs-map-editor.git
cd nurbs-map-editor/c_map_editor
```

2. Check dependencies:
```bash
make check-deps
```

3. Build the project:
```bash
make release
```

4. Run the editor:
```bash
make run
```

#### Windows Build (MSYS2)

For Windows users with MSYS2:
```bash
# Use Windows-specific Makefile
make -f Makefile.windows release

# Or use the batch script
build.bat all
```

### Installation

To install system-wide:
```bash
sudo make install
```

To uninstall:
```bash
sudo make uninstall
```

## Usage

### Getting Started

1. **Launch the Editor**:
   ```bash
   nurbs_map_editor
   ```

2. **Create Your First Object**:
   - Go to Insert menu → NURBS Sphere
   - The sphere will appear in the 3D viewport and scene hierarchy

3. **Navigate the 3D Viewport**:
   - **Left Mouse**: Rotate camera around the scene
   - **Middle Mouse**: Pan the camera
   - **Mouse Wheel**: Zoom in/out
   - **Right Mouse**: Context menu (future feature)

4. **Edit Object Properties**:
   - Select an object in the scene hierarchy
   - Use the Properties panel to modify:
     - Position, rotation, and scale
     - Material properties (ambient, diffuse, specular)
     - Object-specific parameters

### Working with IGES Files

#### Importing IGES Models
1. Go to File → Import IGES...
2. Select your IGES file
3. The model will be loaded and converted to NURBS surfaces
4. Objects appear in the scene hierarchy for individual manipulation

#### Exporting IGES Models
1. Select objects to export (or leave none selected to export all)
2. Go to File → Export IGES...
3. Choose filename and location
4. The IGES file will contain all selected NURBS surfaces

### Tools and Manipulation

#### Selection Tool (Default)
- Click objects to select them
- Selected objects are highlighted
- Properties panel updates to show selected object

#### Move Tool
- Drag objects to move them in 3D space
- Use axis-aligned gizmos for precise movement
- Hold Shift for fine adjustments

#### Rotate Tool
- Rotate objects around their center
- Use rotation gizmos for axis-aligned rotation
- Hold Shift for 15-degree increments

#### Scale Tool
- Resize objects uniformly or per-axis
- Maintain proportions or scale independently
- Hold Shift for proportional scaling

### Camera Views

- **Perspective**: Natural 3D view with depth perception
- **Top View**: Looking down from above (X-Z plane)
- **Front View**: Looking from the front (X-Y plane)
- **Side View**: Looking from the side (Y-Z plane)

### Lighting

#### Adding Lights
1. Go to Insert menu
2. Choose light type:
   - Point Light: Omnidirectional
   - Directional Light: Parallel rays (sun-like)
   - Spot Light: Cone-shaped beam

#### Configuring Lights
- **Position**: 3D location of the light
- **Color**: RGB color of the light
- **Intensity**: Brightness multiplier
- **Direction**: For directional and spot lights
- **Spot Angle**: Cone angle for spot lights

## Technical Details

### NURBS Implementation

The editor uses industry-standard NURBS mathematics:
- **Basis Functions**: Cox-de Boor recursion formula
- **Surface Evaluation**: Bivariate NURBS surface evaluation
- **Derivatives**: Automatic tangent and normal calculation
- **Tessellation**: Adaptive mesh generation for rendering

### IGES Compatibility

Supported IGES entities:
- **Entity 126**: Rational B-spline curves
- **Entity 128**: Rational B-spline surfaces
- **Entity 190**: Plane surfaces
- **Entity 192**: Cylindrical surfaces
- **Entity 196**: Spherical surfaces
- **Entity 198**: Toroidal surfaces

### Performance Optimization

- **Level-of-Detail**: Automatic tessellation based on view distance
- **Frustum Culling**: Only render visible objects
- **Occlusion Culling**: Skip objects hidden behind others
- **Efficient Memory Management**: Minimal allocation/deallocation

## Development

### Building for Development

1. Build debug version:
```bash
make debug
```

2. Run with debugging:
```bash
make run-debug
```

3. Memory leak checking:
```bash
make memcheck
```

### Code Style

The project follows strict C11 standards with:
- **Naming**: snake_case for functions and variables
- **Indentation**: 4 spaces, no tabs
- **Line Length**: Maximum 100 characters
- **Documentation**: Doxygen-style comments

### Contributing

1. Fork the repository
2. Create a feature branch
3. Make your changes
4. Run tests: `make test`
5. Format code: `make format`
6. Run static analysis: `make analyze`
7. Submit a pull request

## Architecture

### Module Structure

```
c_map_editor/
├── main.c              # Application entry point
├── editor.c/.h         # Main editor framework
├── iges_loader.c/.h    # IGES file format support
├── nurbs_primitives.c/.h # NURBS shape creation
├── viewport.c/.h       # 3D rendering viewport
├── scene_hierarchy.c/.h # Object tree management
├── properties_panel.c/.h # UI for object properties
├── file_operations.c/.h # Save/load functionality
├── undo_redo.c/.h      # Command pattern implementation
├── camera_controls.c/.h # 3D navigation
├── selection_system.c/.h # Object picking and highlighting
├── material_editor.c/.h # Material property editing
└── lighting_system.c/.h # Light management
```

### Dependencies

- **GTK+ 3.0**: User interface framework
- **OpenGL**: 3D graphics rendering
- **GLU**: OpenGL utility functions
- **GLFW**: Window and input management
- **GLEW**: OpenGL extension loading

## License

This project is licensed under the MIT License - see the LICENSE file for details.

## Support

For bug reports, feature requests, or questions:
- **Issues**: [GitHub Issues](https://github.com/your-repo/nurbs-map-editor/issues)
- **Documentation**: [Wiki](https://github.com/your-repo/nurbs-map-editor/wiki)
- **Email**: contact@nurbseditor.com

## Roadmap

### Version 1.1 (Planned)
- [ ] Texture mapping support
- [ ] Advanced NURBS operations (trim, blend, fillet)
- [ ] Plugin system for custom tools
- [ ] Collaborative editing support

### Version 1.2 (Future)
- [ ] Animation timeline
- [ ] Physics simulation preview
- [ ] VR/AR support
- [ ] Cloud storage integration

## Acknowledgments

- **NURBS Theory**: Based on "The NURBS Book" by Piegl and Tiller
- **IGES Specification**: IGES 5.3 standard implementation
- **Roblox Studio**: UI/UX inspiration for intuitive 3D editing
- **Contributors**: All developers who have contributed to this project

---

**NURBS Map Editor** - Bringing professional CAD precision to game development.