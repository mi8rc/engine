@echo off
REM NURBS Map Editor Windows Build Script using MSYS2

echo Building NURBS Map Editor for Windows...

REM Check if MSYS2 is installed
if not exist "C:\msys64\usr\bin\bash.exe" (
    echo ERROR: MSYS2 not found at C:\msys64\
    echo Please install MSYS2 from https://www.msys2.org/
    echo Then install dependencies with:
    echo   pacman -S mingw-w64-x86_64-gcc mingw-w64-x86_64-cmake mingw-w64-x86_64-ninja
    echo   pacman -S mingw-w64-x86_64-pkg-config mingw-w64-x86_64-gtk3 mingw-w64-x86_64-glew
    pause
    exit /b 1
)

REM Set MSYS2 environment
set PATH=C:\msys64\mingw64\bin;C:\msys64\usr\bin;%PATH%
set PKG_CONFIG_PATH=C:\msys64\mingw64\lib\pkgconfig

REM Create build directory
if not exist build mkdir build
cd build

REM Configure with CMake
echo Configuring with CMake...
cmake .. -G "Ninja" -DCMAKE_BUILD_TYPE=Release

if %ERRORLEVEL% neq 0 (
    echo ERROR: CMake configuration failed
    pause
    exit /b 1
)

REM Build
echo Building...
cmake --build .

if %ERRORLEVEL% neq 0 (
    echo ERROR: Build failed
    pause
    exit /b 1
)

echo.
echo Build completed successfully!
echo Executable: build\NurbsMapEditor.exe
echo.
echo To run the application, make sure MSYS2 MinGW64 is in your PATH:
echo   set PATH=C:\msys64\mingw64\bin;%%PATH%%
echo   .\NurbsMapEditor.exe
echo.
pause