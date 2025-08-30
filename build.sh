#!/bin/bash

# NURBS Map Editor Build Script

set -e

echo "Building NURBS Map Editor..."

# Create build directory
mkdir -p build
cd build

# Configure with CMake
echo "Configuring with CMake..."
cmake .. -DCMAKE_BUILD_TYPE=Release

# Build
echo "Building..."
make -j$(nproc)

echo "Build completed successfully!"
echo "Executable: build/NurbsMapEditor"
echo ""
echo "To install system-wide:"
echo "  sudo make install"
echo ""
echo "To run:"
echo "  ./NurbsMapEditor"