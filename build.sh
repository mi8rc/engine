#!/bin/bash

# Simple build script for MSYS2/Linux
# Run this in MSYS2 MinGW64 terminal or Linux terminal

echo "Building NURBS Map Editor..."

# Detect platform
if [[ "$OSTYPE" == "msys" ]] || [[ "$OSTYPE" == "cygwin" ]]; then
    GENERATOR="MinGW Makefiles"
    EXECUTABLE="NURBSMapEditor.exe"
else
    GENERATOR="Unix Makefiles"
    EXECUTABLE="NURBSMapEditor"
fi

echo "Platform: $OSTYPE, Generator: $GENERATOR"

# Try CMake build first
echo "Attempting CMake build..."
mkdir -p build
cd build

if cmake .. -G "$GENERATOR"; then
    echo "CMake configuration successful, building..."
    if make -j$(nproc); then
        echo "Build complete! Binary location: build/$EXECUTABLE"
        exit 0
    else
        echo "Make failed, trying fallback..."
    fi
else
    echo "CMake failed, trying fallback..."
fi

# Fallback to simple Makefile
cd ..
echo "Using simple Makefile..."
cp Makefile.simple Makefile

if make -j$(nproc); then
    echo "Build complete! Binary location: ./$EXECUTABLE"
else
    echo "Build failed! Please check dependencies."
    exit 1
fi