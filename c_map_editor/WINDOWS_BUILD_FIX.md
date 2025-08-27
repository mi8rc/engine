# Windows Build Fix for NURBS Map Editor

## Problem Summary

The NURBS Map Editor was failing to build on Windows due to cross-compilation issues between Linux glibc and Windows MinGW. The specific errors included:

1. **glibc Fortify Functions**: `__printf_chk`, `__fprintf_chk`, `__snprintf_chk`, `__fread_chk`
2. **glibc ctype Function**: `__ctype_b_loc`  
3. **GLEW Function Pointers**: `__glewGenVertexArrays`, `__glewCreateShader`, etc.
4. **Standard Library Conflicts**: `stderr` symbol issues

## Solution Implemented

### 1. Makefile Updates
- Added Windows-specific compiler flags to disable glibc fortify functions
- Enabled MinGW ANSI stdio compatibility
- Configured static GLEW linking
- Fixed library linking order and flags

### 2. Cross-Platform Compatibility Header
Created `win_compat.h` with:
- Windows header inclusion management
- GLEW static linking configuration
- Safe ctype function implementations
- Math constant definitions

### 3. Source Code Updates
Updated all source files to include the compatibility header:
- `main.c`
- `editor.c`
- `iges_loader.c`
- `nurbs_primitives.c`

### 4. Header Fixes
- Updated `src/nurbs.h` for proper Windows OpenGL inclusion
- Updated `src/fps_engine.h` for Windows compatibility

## How to Build

### Method 1: Use the Build Script (Recommended)
```bash
# In MSYS2 MinGW64 terminal:
cd c_map_editor
./build_windows.sh
```

### Method 2: Manual Build
```bash
# In MSYS2 MinGW64 terminal:
cd c_map_editor

# Install dependencies if needed:
pacman -S mingw-w64-x86_64-gtk3 mingw-w64-x86_64-glew mingw-w64-x86_64-glfw mingw-w64-x86_64-json-c

# Clean and build:
make BUILD_MODE=windows clean
make BUILD_MODE=windows
```

### Method 3: With Environment Variables
```bash
# Set environment variables:
export BUILD_MODE=windows
export CFLAGS="-U_FORTIFY_SOURCE -D_FORTIFY_SOURCE=0 -D__USE_MINGW_ANSI_STDIO=1 -DGLEW_STATIC"
export LDFLAGS="-Wl,--enable-stdcall-fixup -Wl,--disable-auto-import"

# Build:
make clean
make
```

## Dependencies Required

Install these packages in MSYS2:
```bash
pacman -S mingw-w64-x86_64-toolchain \
          mingw-w64-x86_64-gtk3 \
          mingw-w64-x86_64-glew \
          mingw-w64-x86_64-glfw \
          mingw-w64-x86_64-json-c \
          mingw-w64-x86_64-pkg-config
```

## Key Technical Changes

### Compiler Flags Added
- `-U_FORTIFY_SOURCE -D_FORTIFY_SOURCE=0`: Disables glibc fortify functions
- `-D__USE_MINGW_ANSI_STDIO=1`: Uses MinGW stdio instead of glibc
- `-DGLEW_STATIC`: Links GLEW statically to avoid function pointer issues
- `-static-libgcc`: Statically links GCC runtime

### Linker Flags Added
- `-Wl,--enable-stdcall-fixup`: Fixes Windows calling convention issues
- `-Wl,--disable-auto-import`: Prevents automatic symbol importing conflicts

### Header Inclusion Order
1. Windows compatibility header (`win_compat.h`)
2. Windows system headers with proper defines
3. GLEW headers (with static linking)
4. Standard C library headers
5. Application headers

## Verification

After successful build, you should see:
```
BUILD SUCCESSFUL!
Executable: build/bin/nurbs_map_editor.exe
```

Run the editor:
```bash
./build/bin/nurbs_map_editor.exe
```

## Troubleshooting

### If you still get glibc errors:
- Ensure you're using MSYS2 MinGW64 terminal (not MSYS2 or regular Windows Command Prompt)
- Verify all dependencies are installed
- Try a clean build: `make BUILD_MODE=windows clean && make BUILD_MODE=windows`

### If you get GLEW errors:
- Ensure `mingw-w64-x86_64-glew` is installed
- Verify GLEW_STATIC is being defined in compilation

### If you get GTK errors:
- Ensure `mingw-w64-x86_64-gtk3` is installed
- Check that pkg-config can find GTK: `pkg-config --exists gtk+-3.0`

## Platform Detection Debug

To debug platform detection:
```bash
make BUILD_MODE=windows debug-platform
```

This will show all detected settings and library paths.