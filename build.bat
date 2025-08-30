@echo off
echo Building NURBS Map Editor...

echo Attempting CMake build...
if not exist build mkdir build
cd build

cmake .. -G "MinGW Makefiles"
if %ERRORLEVEL% EQU 0 (
    echo CMake configuration successful, building...
    mingw32-make -j%NUMBER_OF_PROCESSORS%
    if %ERRORLEVEL% EQU 0 (
        echo Build complete! Binary location: build/NURBSMapEditor.exe
        goto end
    )
)

echo CMake failed, trying simple Makefile...
cd ..
copy Makefile.simple Makefile
gcc -std=c99 -Wall -O2 -Isrc src/main.c src/nurbs.c src/fps_engine.c src/map_loader.c src/math_utils.c -o NURBSMapEditor.exe -lGL -lGLU -lglfw -lm
if %ERRORLEVEL% EQU 0 (
    echo Build complete! Binary location: NURBSMapEditor.exe
) else (
    echo Build failed! Please check dependencies.
)

:end
pause