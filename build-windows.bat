@echo off
REM NURBS Map Editor Windows Build Script using MSYS2

echo Building NURBS Map Editor for Windows...

REM Check if MSYS2 is installed
if not exist "C:\msys64\usr\bin\bash.exe" (
    echo ERROR: MSYS2 not found at C:\msys64\
    echo Please install MSYS2 from https://www.msys2.org/
    echo Then install dependencies with:
    echo   pacman -S mingw-w64-x86_64-gcc mingw-w64-x86_64-pkg-config
    echo   pacman -S mingw-w64-x86_64-gtk3 mingw-w64-x86_64-glew
    pause
    exit /b 1
)

REM Launch MSYS2 MinGW64 shell and build
C:\msys64\usr\bin\bash.exe -lc "cd '%CD%' && make release"

if %ERRORLEVEL% neq 0 (
    echo ERROR: Build failed
    pause
    exit /b 1
)

echo.
echo Build completed successfully!
echo Executable: build\bin\NurbsMapEditor.exe
echo.
echo To run the application:
echo   make run
echo.
echo To create distribution package:
echo   make package
echo.
pause