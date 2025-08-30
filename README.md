# NURBS Map Editor

A professional 3D NURBS-based map editor built with C and GTK3, featuring a Roblox Studio-inspired interface and comprehensive IGES file support.

## Features

### Core Functionality
- **NURBS Curves and Surfaces**: Full support for Non-Uniform Rational B-Splines
- **3D Viewport**: OpenGL-based rendering with camera controls
- **Interactive Editing**: Control point manipulation, curve/surface creation
- **Professional UI**: Dark theme inspired by Roblox Studio

### File Format Support
- **IGES (.igs/.iges)**: Full import/export support for IGES files
- **Native Format (.nme)**: Optimized format for project files
- **OBJ Export**: Export tessellated geometry to Wavefront OBJ
- **STEP Support**: Basic STEP file compatibility (planned)

### User Interface
- **Hierarchy Panel**: Scene object management with tree view
- **Property Panel**: Real-time object property editing
- **3D Viewport**: Interactive 3D scene with grid and navigation
- **Toolbar**: Quick access to common tools
- **Dark Theme**: Professional dark interface matching modern IDEs

### Tools
- **Select Tool**: Object and control point selection
- **Move Tool**: Object transformation
- **Create Tools**: Curve and surface creation
- **Edit Tools**: Control point manipulation

## Building

### Prerequisites

#### Windows (MSVC)
- Visual Studio 2022 or later
- vcpkg package manager
- CMake 3.16+

#### Linux
- GCC or Clang
- GTK3 development libraries
- OpenGL and GLEW
- CMake 3.16+

### Windows Build (MSVC)

```bash
# Install dependencies with vcpkg
vcpkg install gtk3:x64-windows glew:x64-windows opengl:x64-windows

# Configure and build
cmake -B build -S . -G "Visual Studio 17 2022" -A x64 -DCMAKE_TOOLCHAIN_FILE=path/to/vcpkg.cmake
cmake --build build --config Release
```

### Linux Build

```bash
# Install dependencies (Ubuntu/Debian)
sudo apt-get install build-essential cmake pkg-config
sudo apt-get install libgtk-3-dev libgl1-mesa-dev libglew-dev

# Configure and build
cmake -B build -S . -DCMAKE_BUILD_TYPE=Release
cmake --build build

# Install
sudo cmake --install build
```

## Usage

### Getting Started

1. **Launch the Editor**: Run the executable to open the main interface
2. **Create Objects**: Use the hierarchy panel toolbar to add NURBS curves or surfaces
3. **Edit Properties**: Select objects to edit their properties in the property panel
4. **Manipulate Geometry**: Use tools to move objects and edit control points
5. **Save/Load**: Use File menu to save projects or import IGES files

### Keyboard Shortcuts

- `S` - Select tool
- `M` - Move tool  
- `C` - Create curve tool
- `Ctrl+N` - New file
- `Ctrl+O` - Open file
- `Ctrl+S` - Save file

### IGES File Support

The editor provides comprehensive IGES file support:

- **Import**: Load NURBS curves and surfaces from IGES files
- **Export**: Save scenes to IGES format for CAD interoperability
- **Entity Support**: 
  - NURBS Curves (Type 126)
  - NURBS Surfaces (Type 128)
  - Transformation matrices
  - Geometric entities

### Camera Controls

- **Middle Mouse**: Rotate camera
- **Scroll Wheel**: Zoom in/out
- **Shift+Middle Mouse**: Pan camera

## Architecture

### Core Components

- **NURBS Engine** (`src/nurbs.c`): Mathematical NURBS evaluation and rendering
- **File I/O** (`src/file_io.c`): Multi-format file import/export including IGES
- **Viewport** (`src/viewport.c`): OpenGL-based 3D rendering
- **UI Framework** (`src/ui_builder.c`): GTK3 interface with custom styling
- **Tools System** (`src/tools.c`): Interactive editing tools

### NURBS Implementation

The editor implements industry-standard NURBS algorithms:

- **Basis Functions**: Cox-de Boor recursion formula
- **Curve Evaluation**: Rational curve evaluation with weights
- **Surface Evaluation**: Tensor product surface evaluation
- **Tessellation**: Adaptive mesh generation for rendering

### IGES Implementation

Standards-compliant IGES implementation:

- **Parser**: Full IGES file structure parsing
- **Entity Support**: Major geometric entities
- **Export**: Proper IGES formatting and structure
- **Validation**: Error checking and recovery

## Contributing

1. Fork the repository
2. Create a feature branch
3. Make your changes
4. Add tests if applicable
5. Submit a pull request

### Code Style

- Follow GNU C coding style
- Use descriptive variable names
- Comment complex algorithms
- Maintain consistent indentation

## License

This project is licensed under the MIT License - see the LICENSE file for details.

## Roadmap

### Version 1.1
- [ ] STEP file support
- [ ] Advanced surface operations
- [ ] Plugin system
- [ ] Scripting interface

### Version 1.2
- [ ] Mesh generation
- [ ] Boolean operations
- [ ] Advanced materials
- [ ] Animation support

## Support

For support, please:
1. Check the documentation
2. Search existing issues
3. Create a new issue with detailed information

## Acknowledgments

- GTK3 development team
- OpenGL community
- IGES/STEP standards organizations
- Roblox Studio for UI inspiration