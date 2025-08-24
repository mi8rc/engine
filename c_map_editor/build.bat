@echo off
REM NURBS Map Editor - Windows Build Script
REM For use in MSYS2 environment or Windows Command Prompt with MSYS2 in PATH

setlocal EnableDelayedExpansion

set PROJECT_NAME=nurbs_map_editor
set VERSION=1.0.0

REM Colors (if supported)
set GREEN=[32m
set RED=[31m
set YELLOW=[33m
set BLUE=[34m
set NC=[0m

REM Default values
set BUILD_TYPE=release
set TARGET_ARCH=x86_64
set SKIP_DEPS=false
set VERBOSE=false

REM Parse command line arguments
:parse_args
if "%~1"=="" goto :end_parse
if "%~1"=="--skip-deps" (
    set SKIP_DEPS=true
    shift
    goto :parse_args
)
if "%~1"=="--verbose" (
    set VERBOSE=true
    shift
    goto :parse_args
)
if "%~1"=="--arch" (
    set TARGET_ARCH=%~2
    shift
    shift
    goto :parse_args
)
if "%~1"=="build" (
    set COMMAND=build
    shift
    if "%~1"=="release" (
        set BUILD_TYPE=release
        shift
    ) else if "%~1"=="debug" (
        set BUILD_TYPE=debug
        shift
    )
    goto :parse_args
)
if "%~1"=="deps" (
    set COMMAND=deps
    shift
    goto :parse_args
)
if "%~1"=="check" (
    set COMMAND=check
    shift
    goto :parse_args
)
if "%~1"=="package" (
    set COMMAND=package
    shift
    goto :parse_args
)
if "%~1"=="clean" (
    set COMMAND=clean
    shift
    goto :parse_args
)
if "%~1"=="all" (
    set COMMAND=all
    shift
    goto :parse_args
)
if "%~1"=="help" (
    set COMMAND=help
    shift
    goto :parse_args
)
echo Unknown argument: %~1
goto :show_help
:end_parse

REM Set default command if none specified
if not defined COMMAND set COMMAND=build

echo %BLUE%[INFO]%NC% NURBS Map Editor Windows Build Script
echo %BLUE%[INFO]%NC% Project: %PROJECT_NAME% v%VERSION%
echo %BLUE%[INFO]%NC% Target Architecture: %TARGET_ARCH%
echo %BLUE%[INFO]%NC% Build Type: %BUILD_TYPE%
echo.

REM Check if we're in MSYS2 environment
where /q pacman
if %ERRORLEVEL% NEQ 0 (
    echo %RED%[ERROR]%NC% MSYS2 not found in PATH
    echo Please run this script from MSYS2 terminal or add MSYS2 to your PATH
    echo Download MSYS2 from: https://www.msys2.org/
    exit /b 1
)

REM Execute command
if "%COMMAND%"=="help" goto :show_help
if "%COMMAND%"=="deps" goto :install_deps
if "%COMMAND%"=="check" goto :check_deps
if "%COMMAND%"=="build" goto :build_project
if "%COMMAND%"=="package" goto :create_package
if "%COMMAND%"=="clean" goto :clean_build
if "%COMMAND%"=="all" goto :build_all

echo %RED%[ERROR]%NC% Unknown command: %COMMAND%
goto :show_help

:install_deps
echo %BLUE%[INFO]%NC% Installing dependencies for Windows (MSYS2)...
make -f Makefile.windows TARGET_ARCH=%TARGET_ARCH% install-deps
if %ERRORLEVEL% NEQ 0 (
    echo %RED%[ERROR]%NC% Failed to install dependencies
    exit /b 1
)
echo %GREEN%[SUCCESS]%NC% Dependencies installed successfully
goto :end

:check_deps
echo %BLUE%[INFO]%NC% Checking dependencies...
make -f Makefile.windows TARGET_ARCH=%TARGET_ARCH% check-deps
if %ERRORLEVEL% NEQ 0 (
    echo %YELLOW%[WARNING]%NC% Some dependencies are missing
    if "%SKIP_DEPS%"=="false" (
        echo %BLUE%[INFO]%NC% Installing missing dependencies...
        goto :install_deps
    ) else (
        echo %RED%[ERROR]%NC% Dependencies missing and --skip-deps specified
        exit /b 1
    )
)
echo %GREEN%[SUCCESS]%NC% All dependencies are available
goto :end

:build_project
if "%SKIP_DEPS%"=="false" (
    call :check_deps
    if !ERRORLEVEL! NEQ 0 exit /b !ERRORLEVEL!
)

echo %BLUE%[INFO]%NC% Building %PROJECT_NAME% (%BUILD_TYPE%) for Windows...
make -f Makefile.windows TARGET_ARCH=%TARGET_ARCH% %BUILD_TYPE%
if %ERRORLEVEL% NEQ 0 (
    echo %RED%[ERROR]%NC% Build failed
    exit /b 1
)
echo %GREEN%[SUCCESS]%NC% Build completed successfully
goto :end

:create_package
echo %BLUE%[INFO]%NC% Creating Windows package...
make -f Makefile.windows TARGET_ARCH=%TARGET_ARCH% package
if %ERRORLEVEL% NEQ 0 (
    echo %RED%[ERROR]%NC% Package creation failed
    exit /b 1
)
echo %GREEN%[SUCCESS]%NC% Package created successfully
goto :end

:clean_build
echo %BLUE%[INFO]%NC% Cleaning build files...
make -f Makefile.windows TARGET_ARCH=%TARGET_ARCH% clean
if %ERRORLEVEL% NEQ 0 (
    echo %RED%[ERROR]%NC% Clean failed
    exit /b 1
)
echo %GREEN%[SUCCESS]%NC% Clean completed successfully
goto :end

:build_all
echo %BLUE%[INFO]%NC% Running full build pipeline...
if "%SKIP_DEPS%"=="false" (
    call :install_deps
    if !ERRORLEVEL! NEQ 0 exit /b !ERRORLEVEL!
)
call :check_deps
if !ERRORLEVEL! NEQ 0 exit /b !ERRORLEVEL!
call :build_project
if !ERRORLEVEL! NEQ 0 exit /b !ERRORLEVEL!
call :create_package
if !ERRORLEVEL! NEQ 0 exit /b !ERRORLEVEL!
echo %GREEN%[SUCCESS]%NC% Full build pipeline completed successfully
goto :end

:show_help
echo NURBS Map Editor - Windows Build Script
echo =====================================
echo.
echo Usage: %0 [OPTIONS] [COMMAND]
echo.
echo Commands:
echo   build [release^|debug]   - Build the project (default: release)
echo   deps                    - Install dependencies
echo   check                   - Check dependencies
echo   package                 - Create distribution package
echo   clean                   - Clean build files
echo   all                     - Install deps, build, and package
echo   help                    - Show this help
echo.
echo Options:
echo   --skip-deps            - Skip dependency installation
echo   --verbose              - Enable verbose output
echo   --arch ARCH            - Target architecture (x86_64, i686)
echo.
echo Examples:
echo   %0 build               - Build release version
echo   %0 build debug         - Build debug version
echo   %0 all                 - Full build pipeline
echo   %0 --arch i686 build   - Build for 32-bit Windows
echo.
echo Requirements:
echo   - MSYS2 installed and in PATH
echo   - Run from MSYS2 terminal for best results
echo.
echo Download MSYS2 from: https://www.msys2.org/
goto :end

:end
endlocal