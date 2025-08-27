# Windows Build Instructions - NURBS Map Editor

## 🚀 Enhanced Windows Build System

The Makefile has been completely redesigned to handle Windows builds more robustly. This document explains how to use the new system to fix your Windows/MSYS2 build issues.

## 🔧 Quick Fix for Your Issue

If you're getting the linker error `ld returned 5 exit status`, use this command:

```bash
make BUILD_MODE=windows clean && make BUILD_MODE=windows debug
```

Or for release build:
```bash
make BUILD_MODE=windows clean && make BUILD_MODE=windows release
```

## 📋 Prerequisites

### Required MSYS2 Packages
Install these packages in your MSYS2 environment:

```bash
# Core development tools
pacman -S mingw-w64-x86_64-gcc
pacman -S mingw-w64-x86_64-make
pacman -S mingw-w64-x86_64-pkg-config

# Graphics and UI libraries
pacman -S mingw-w64-x86_64-gtk3
pacman -S mingw-w64-x86_64-glew
pacman -S mingw-w64-x86_64-glfw
pacman -S mingw-w64-x86_64-freeglut

# Additional libraries
pacman -S mingw-w64-x86_64-json-c
```

Or install all at once:
```bash
pacman -S mingw-w64-x86_64-{gcc,make,pkg-config,gtk3,glew,glfw,freeglut,json-c}
```

## 🎯 New Build System Features

### 1. **Platform Detection**
The Makefile now automatically detects your platform and configures accordingly:

- **Auto-detection**: Detects MINGW64_NT, Windows_NT, etc.
- **Forced modes**: Override detection for cross-compilation
- **Better error handling**: Clear messages about what's being built

### 2. **Enhanced Windows Linking**
The new system addresses common Windows linking issues:

- **Duplicate library elimination**: Automatically removes conflicting libraries
- **Proper library ordering**: Windows-specific library order
- **Better linker flags**: Improved error reporting and compatibility

### 3. **Build Mode Override**
You can force specific build configurations:

```bash
# Force Windows build (even on Linux)
make BUILD_MODE=windows release

# Force Linux build  
make BUILD_MODE=linux release

# Auto-detect (default)
make release
```

## 🛠️ Usage Instructions

### Basic Build Commands

```bash
# Clean and build release version for Windows
make BUILD_MODE=windows clean && make BUILD_MODE=windows release

# Build debug version for Windows
make BUILD_MODE=windows debug

# Show what platform/settings are detected
make BUILD_MODE=windows debug-platform

# Test Windows linking incrementally (for debugging)
make BUILD_MODE=windows test-link-windows
```

### Platform Detection Debug

To see exactly what the Makefile detects:

```bash
make debug-platform
```

Sample output:
```
=== Platform Detection ===
UNAME_S: MINGW64_NT-10.0-19044
OS: Windows_NT
BUILD_MODE: auto
IS_WINDOWS: 1
DETECTED_PLATFORM: Windows/MSYS2 (MINGW64_NT-10.0-19044)
EXE_SUFFIX: .exe
LINK_STRATEGY: windows_safe

=== Libraries ===
GTK_LIBS: -lgtk-3 -lgdk-3 -lz -lpangocairo-1.0 -lpango-1.0 [...]
GTK_CLEAN_LIBS: -lgtk-3 -lgdk-3 -lz -lpangocairo-1.0 [...]  # Duplicates removed
ALL_LIBS: [filtered library list]
```

### Testing Incremental Linking

If you're still having linker issues, use the incremental test:

```bash
make BUILD_MODE=windows test-link-windows
```

This will test linking step by step:
1. Basic GTK linking
2. Adding OpenGL  
3. Adding GLFW
4. Full linking

It will show you exactly where the linking fails.

## 🔍 Troubleshooting Common Issues

### Issue 1: "ld returned 5 exit status"
**Cause**: Library conflicts or missing dependencies
**Solution**: 
```bash
make BUILD_MODE=windows debug-platform  # Check detection
make BUILD_MODE=windows test-link-windows  # Find where it fails
```

### Issue 2: Multiple definition errors
**Cause**: Functions defined in multiple files
**Solution**: The new system filters out duplicate libraries automatically

### Issue 3: Missing pkg-config libraries
**Cause**: Libraries not installed in MSYS2
**Solution**: 
```bash
make check-deps  # Check what's missing
# Install missing packages with pacman
```

### Issue 4: Wrong subsystem (console vs windows)
**Cause**: Using `-mwindows` with `main()` instead of `WinMain()`
**Solution**: The new system uses console subsystem by default

## 📁 Project Structure

After building, you'll have:

```
build/
├── bin/
│   ├── nurbs_map_editor.exe      # Release version
│   └── nurbs_map_editor_debug.exe # Debug version
├── obj/                           # Object files
└── dist/                          # Package directory
```

## 🚀 Advanced Usage

### Cross-Compilation

You can build for Windows even from a Linux system:

```bash
# Install MinGW cross-compiler on Linux
sudo apt install mingw-w64

# Force Windows build mode
make BUILD_MODE=windows CC=x86_64-w64-mingw32-gcc release
```

### Custom Architecture

```bash
# Build for 32-bit Windows
make BUILD_MODE=windows TARGET_ARCH=i686 release

# Build for 64-bit Windows (default)
make BUILD_MODE=windows TARGET_ARCH=x86_64 release
```

### Package Creation

```bash
# Create distributable package
make BUILD_MODE=windows package
```

## 📝 What Changed from Original

### Key Improvements:

1. **Better Platform Detection**: Now properly detects MINGW environments
2. **Library Conflict Resolution**: Automatically filters duplicate Windows libraries from GTK
3. **Enhanced Linker Strategy**: Uses Windows-specific linker flags
4. **Forced Build Modes**: Can override platform detection
5. **Incremental Testing**: Step-by-step linking tests for debugging
6. **Comprehensive Diagnostics**: Better error reporting and platform info

### The Root Problems Fixed:

- **Duplicate Libraries**: GTK's pkg-config was including Windows system libraries that conflicted with WIN_LIBS
- **Library Order**: Windows linking is more sensitive to library order than Linux
- **Subsystem Mismatch**: Was using Windows subsystem with console-style main()
- **Platform Detection**: Didn't properly handle all MSYS2 environment variations

## 🎉 Success Verification

After building successfully, you should see:

```
Linking nurbs_map_editor for Windows using enhanced strategy...
Platform: Windows (forced)
Libraries: [clean library list without duplicates]
Windows build complete: build/bin/nurbs_map_editor.exe
```

The built executable should run without missing DLL errors.

## 📞 Need Help?

If you're still having issues:

1. Run `make BUILD_MODE=windows debug-platform` and share the output
2. Run `make BUILD_MODE=windows test-link-windows` to see where linking fails
3. Check that all required MSYS2 packages are installed
4. Verify you're running from within MSYS2 environment, not Command Prompt

The new build system provides much better diagnostics to help identify exactly what's going wrong.