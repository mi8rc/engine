# NURBS Map Editor

A simple NURBS-based map editor with Roblox Studio-like interface, written in C.

## Features

- 3D viewport with camera controls
- NURBS surface and curve editing
- Roblox Studio-inspired UI layout
- Cross-platform support (Windows/Linux)

## Building

### Prerequisites (MSYS2)

```bash
# Install required packages
pacman -S mingw-w64-x86_64-gcc \
          mingw-w64-x86_64-cmake \
          mingw-w64-x86_64-make \
          mingw-w64-x86_64-glfw \
          mingw-w64-x86_64-pkg-config
```

### Build Instructions

```bash
# Clone the repository
git clone <your-repo-url>
cd NURBSMapEditor

# Build using the script
./build.sh

# Or manually:
mkdir build && cd build
cmake .. -G "MinGW Makefiles"
make -j$(nproc)
```

### Running

```bash
./build/NURBSMapEditor.exe
```

## Controls

- **ESC**: Exit application
- **Mouse**: Navigate 3D viewport (TODO)
- **WASD**: Camera movement (TODO)

## Development Status

This is a minimal implementation. The following features are planned:
- [ ] NURBS mathematics implementation
- [ ] UI panels (hierarchy, properties, toolbar)
- [ ] Object manipulation tools
- [ ] File I/O for map formats
- [ ] Material and texture support

## License

MIT License