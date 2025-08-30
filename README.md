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

### User Interface
- **Hierarchy Panel**: Scene object management with tree view
- **Property Panel**: Real-time object property editing
- **3D Viewport**: Interactive 3D scene with grid and navigation
- **Toolbar**: Quick access to common tools
- **Dark Theme**: Professional dark interface matching modern IDEs

## Building

### Windows (MSYS2) - Recommended

1. **Install MSYS2** from https://www.msys2.org/

2. **Install dependencies**:
   ```bash
   # Open MSYS2 MinGW 64-bit terminal
   pacman -S mingw-w64-x86_64-gcc
   pacman -S mingw-w64-x86_64-cmake
   pacman -S mingw-w64-x86_64-ninja
   pacman -S mingw-w64-x86_64-pkg-config
   pacman -S mingw-w64-x86_64-gtk3
   pacman -S mingw-w64-x86_64-glew
   ```

3. **Build**:
   ```bash
   # In MSYS2 MinGW 64-bit terminal
   cmake -B build -G "Ninja" -DCMAKE_BUILD_TYPE=Release
   cmake --build build
   ```

   Or use the provided batch file:
   ```cmd
   build-windows.bat
   ```

### Linux (Ubuntu/Debian)

```bash
# Install dependencies
sudo apt-get update
sudo apt-get install build-essential cmake pkg-config
sudo apt-get install libgtk-3-dev libgl1-mesa-dev libglew-dev

# Build
./build.sh
```

### Why MSYS2?

- ✅ **No vcpkg bullshit** - Direct package management
- ✅ **Native GTK3 support** - Proper Windows GTK3 builds
- ✅ **Reliable dependencies** - Battle-tested package system
- ✅ **Easy distribution** - Self-contained binaries
- ✅ **Fast builds** - No endless dependency resolution

## Usage

### Getting Started

1. **Launch the Editor**: Run `NurbsMapEditor.exe` (Windows) or `./NurbsMapEditor` (Linux)
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

### Camera Controls

- **Middle Mouse**: Rotate camera
- **Scroll Wheel**: Zoom in/out
- **Shift+Middle Mouse**: Pan camera

## Distribution

### Windows
The built executable requires MSYS2 MinGW64 DLLs. For distribution:
1. Copy required DLLs from `C:\msys64\mingw64\bin\`
2. Include GTK3 themes and icons
3. Package with installer or portable zip

### Linux
Standard shared library dependencies via package manager.

## Architecture

### Core Components
- **NURBS Engine** (`src/nurbs.c`): Mathematical NURBS evaluation and rendering
- **File I/O** (`src/file_io.c`): Multi-format file import/export including IGES
- **Viewport** (`src/viewport.c`): OpenGL-based 3D rendering
- **UI Framework** (`src/ui_builder.c`): GTK3 interface with custom styling
- **Tools System** (`src/tools.c`): Interactive editing tools

## License

This project is licensed under the MIT License.

## Why Not vcpkg?

vcpkg is a nightmare for GTK3 on Windows:
- Endless dependency loops
- Broken baseline commits
- Manifest mode confusion
- Poor GTK3 Windows support

MSYS2 just works. Use it.