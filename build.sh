#!/bin/bash

# NURBS Map Editor Linux Build Script

set -e

echo "Building NURBS Map Editor for Linux..."

# Check dependencies
echo "Checking dependencies..."

if ! command -v pkg-config &> /dev/null; then
    echo "ERROR: pkg-config not found"
    echo "Install with: sudo apt-get install pkg-config"
    exit 1
fi

if ! pkg-config --exists gtk+-3.0; then
    echo "ERROR: GTK3 development libraries not found"
    echo "Install with: sudo apt-get install libgtk-3-dev"
    exit 1
fi

if ! pkg-config --exists glew; then
    echo "ERROR: GLEW development libraries not found"
    echo "Install with: sudo apt-get install libglew-dev"
    exit 1
fi

if ! pkg-config --exists gl; then
    echo "ERROR: OpenGL development libraries not found"
    echo "Install with: sudo apt-get install libgl1-mesa-dev"
    exit 1
fi

echo "Dependencies OK!"

# Build
echo "Building release version..."
make release

echo ""
echo "Build completed successfully!"
echo "Executable: build/bin/NurbsMapEditor"
echo ""
echo "To run the application:"
echo "  make run"
echo ""
echo "To install system-wide:"
echo "  sudo make install"
echo ""
echo "To create distribution package:"
echo "  make package"