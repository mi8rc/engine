#!/bin/bash

# Windows Build Script for NURBS Map Editor
# Run this in MSYS2/MinGW64 environment

echo "=========================================="
echo "NURBS Map Editor - Windows Build Script"
echo "=========================================="

# Check if we're in MSYS2
if [[ "$MSYSTEM" != "MINGW64" && "$MSYSTEM" != "MINGW32" ]]; then
    echo "Warning: Not running in MSYS2 MINGW environment"
    echo "Please run this script in MSYS2 MinGW64 terminal"
fi

# Set Windows build environment
export BUILD_MODE=windows
export CFLAGS="-U_FORTIFY_SOURCE -D_FORTIFY_SOURCE=0"
export LDFLAGS="-Wl,--enable-stdcall-fixup -Wl,--disable-auto-import"

echo "Build Mode: $BUILD_MODE"
echo "CFLAGS: $CFLAGS"
echo "LDFLAGS: $LDFLAGS"
echo ""

# Check dependencies
echo "Checking dependencies..."
pacman -Q mingw-w64-x86_64-gtk3 >/dev/null 2>&1 || echo "Missing: mingw-w64-x86_64-gtk3"
pacman -Q mingw-w64-x86_64-glew >/dev/null 2>&1 || echo "Missing: mingw-w64-x86_64-glew"  
pacman -Q mingw-w64-x86_64-glfw >/dev/null 2>&1 || echo "Missing: mingw-w64-x86_64-glfw"
pacman -Q mingw-w64-x86_64-json-c >/dev/null 2>&1 || echo "Missing: mingw-w64-x86_64-json-c"

echo ""
echo "Starting build..."

# Clean previous build
make BUILD_MODE=windows clean

# Build with Windows mode
echo ""
echo "Building NURBS Map Editor for Windows..."
if make BUILD_MODE=windows; then
    echo ""
    echo "=========================================="
    echo "BUILD SUCCESSFUL!"
    echo "=========================================="
    echo "Executable: build/bin/nurbs_map_editor.exe"
    echo ""
    echo "To run the editor:"
    echo "./build/bin/nurbs_map_editor.exe"
else
    echo ""
    echo "=========================================="
    echo "BUILD FAILED!"
    echo "=========================================="
    echo "Check the error messages above for details."
    echo ""
    echo "Common solutions:"
    echo "1. Install missing dependencies:"
    echo "   pacman -S mingw-w64-x86_64-gtk3 mingw-w64-x86_64-glew mingw-w64-x86_64-glfw mingw-w64-x86_64-json-c"
    echo ""
    echo "2. Update your MSYS2 installation:"
    echo "   pacman -Syu"
    echo ""
    echo "3. Check that you're using the correct MSYS2 terminal (MinGW64)"
    exit 1
fi