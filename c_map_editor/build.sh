#!/bin/bash

# NURBS Map Editor - Cross-Platform Build Script
# Automatically detects platform and uses appropriate build system

set -e  # Exit on error

PROJECT_NAME="nurbs_map_editor"
VERSION="1.0.0"

# Colors for output
RED='\033[0;31m'
GREEN='\033[0;32m'
YELLOW='\033[1;33m'
BLUE='\033[0;34m'
NC='\033[0m' # No Color

# Print colored output
print_status() {
    echo -e "${BLUE}[INFO]${NC} $1"
}

print_success() {
    echo -e "${GREEN}[SUCCESS]${NC} $1"
}

print_warning() {
    echo -e "${YELLOW}[WARNING]${NC} $1"
}

print_error() {
    echo -e "${RED}[ERROR]${NC} $1"
}

# Detect operating system
detect_os() {
    if [[ "$OSTYPE" == "linux-gnu"* ]]; then
        OS="linux"
    elif [[ "$OSTYPE" == "darwin"* ]]; then
        OS="macos"
    elif [[ "$OSTYPE" == "msys" ]] || [[ "$OSTYPE" == "cygwin" ]]; then
        OS="windows"
    elif [[ "$OSTYPE" == "win32" ]]; then
        OS="windows"
    else
        print_error "Unsupported operating system: $OSTYPE"
        exit 1
    fi
}

# Detect Linux distribution
detect_linux_distro() {
    if [ -f /etc/os-release ]; then
        . /etc/os-release
        DISTRO=$ID
        DISTRO_VERSION=$VERSION_ID
    elif [ -f /etc/redhat-release ]; then
        DISTRO="rhel"
    elif [ -f /etc/debian_version ]; then
        DISTRO="debian"
    else
        DISTRO="unknown"
    fi
}

# Install dependencies based on platform
install_dependencies() {
    print_status "Installing dependencies for $OS..."
    
    case $OS in
        "linux")
            detect_linux_distro
            case $DISTRO in
                "ubuntu"|"debian")
                    print_status "Installing dependencies for Ubuntu/Debian..."
                    make install-deps-ubuntu
                    ;;
                "fedora"|"centos"|"rhel")
                    print_status "Installing dependencies for Fedora/CentOS/RHEL..."
                    make install-deps-fedora
                    ;;
                "arch"|"manjaro")
                    print_status "Installing dependencies for Arch Linux..."
                    make install-deps-arch
                    ;;
                *)
                    print_warning "Unknown Linux distribution: $DISTRO"
                    print_status "Please install dependencies manually and run with --skip-deps"
                    exit 1
                    ;;
            esac
            ;;
        "windows")
            print_status "Installing dependencies for Windows (MSYS2)..."
            make -f Makefile.windows install-deps
            ;;
        "macos")
            print_status "Installing dependencies for macOS..."
            if command -v brew >/dev/null 2>&1; then
                brew install pkg-config gtk+3 glfw glew
            else
                print_error "Homebrew not found. Please install Homebrew first."
                exit 1
            fi
            ;;
        *)
            print_error "Unsupported operating system: $OS"
            exit 1
            ;;
    esac
}

# Check dependencies
check_dependencies() {
    print_status "Checking dependencies..."
    
    case $OS in
        "windows")
            make -f Makefile.windows check-deps
            ;;
        *)
            make check-deps
            ;;
    esac
}

# Build the project
build_project() {
    local build_type=${1:-release}
    print_status "Building $PROJECT_NAME ($build_type) for $OS..."
    
    case $OS in
        "windows")
            make -f Makefile.windows $build_type
            ;;
        *)
            make $build_type
            ;;
    esac
}

# Create package
create_package() {
    print_status "Creating package for $OS..."
    
    case $OS in
        "windows")
            make -f Makefile.windows package
            ;;
        *)
            make package
            if [[ "$OS" == "linux" ]]; then
                # Also create Debian package if possible
                if command -v dpkg-deb >/dev/null 2>&1; then
                    make deb
                fi
            fi
            ;;
    esac
}

# Run tests
run_tests() {
    print_status "Running tests..."
    
    case $OS in
        "windows")
            # Windows tests (if implemented)
            print_warning "Tests not yet implemented for Windows"
            ;;
        *)
            make test
            ;;
    esac
}

# Run static analysis
run_analysis() {
    print_status "Running static analysis..."
    
    case $OS in
        "windows")
            make -f Makefile.windows analyze
            ;;
        *)
            make analyze
            ;;
    esac
}

# Clean build files
clean_build() {
    print_status "Cleaning build files..."
    
    case $OS in
        "windows")
            make -f Makefile.windows clean
            ;;
        *)
            make clean
            ;;
    esac
}

# Show help
show_help() {
    echo "NURBS Map Editor - Cross-Platform Build Script"
    echo "=============================================="
    echo ""
    echo "Usage: $0 [OPTIONS] [COMMAND]"
    echo ""
    echo "Commands:"
    echo "  build [release|debug]   - Build the project (default: release)"
    echo "  deps                    - Install dependencies"
    echo "  check                   - Check dependencies"
    echo "  test                    - Run tests"
    echo "  analyze                 - Run static analysis"
    echo "  package                 - Create distribution package"
    echo "  clean                   - Clean build files"
    echo "  all                     - Install deps, build, test, and package"
    echo "  help                    - Show this help"
    echo ""
    echo "Options:"
    echo "  --skip-deps            - Skip dependency installation"
    echo "  --verbose              - Enable verbose output"
    echo "  --arch ARCH            - Target architecture (Windows only: x86_64, i686)"
    echo ""
    echo "Examples:"
    echo "  $0 build               - Build release version"
    echo "  $0 build debug         - Build debug version"
    echo "  $0 all                 - Full build pipeline"
    echo "  $0 --arch i686 build   - Build for 32-bit Windows"
}

# Main function
main() {
    local skip_deps=false
    local verbose=false
    local arch=""
    local command="build"
    local build_type="release"
    
    # Parse command line arguments
    while [[ $# -gt 0 ]]; do
        case $1 in
            --skip-deps)
                skip_deps=true
                shift
                ;;
            --verbose)
                verbose=true
                set -x
                shift
                ;;
            --arch)
                arch="$2"
                export TARGET_ARCH="$arch"
                shift 2
                ;;
            build|deps|check|test|analyze|package|clean|all|help)
                command="$1"
                shift
                if [[ "$command" == "build" && $# -gt 0 && ("$1" == "release" || "$1" == "debug") ]]; then
                    build_type="$1"
                    shift
                fi
                ;;
            *)
                print_error "Unknown option: $1"
                show_help
                exit 1
                ;;
        esac
    done
    
    # Detect operating system
    detect_os
    print_status "Detected operating system: $OS"
    
    if [[ -n "$arch" ]]; then
        print_status "Target architecture: $arch"
    fi
    
    # Execute command
    case $command in
        "help")
            show_help
            ;;
        "deps")
            install_dependencies
            ;;
        "check")
            check_dependencies
            ;;
        "build")
            if [[ "$skip_deps" == false ]]; then
                check_dependencies || {
                    print_warning "Dependencies check failed. Installing dependencies..."
                    install_dependencies
                }
            fi
            build_project "$build_type"
            print_success "Build completed successfully!"
            ;;
        "test")
            run_tests
            ;;
        "analyze")
            run_analysis
            ;;
        "package")
            create_package
            print_success "Package created successfully!"
            ;;
        "clean")
            clean_build
            print_success "Clean completed!"
            ;;
        "all")
            if [[ "$skip_deps" == false ]]; then
                install_dependencies
            fi
            check_dependencies
            build_project "$build_type"
            run_tests
            run_analysis
            create_package
            print_success "Full build pipeline completed successfully!"
            ;;
        *)
            print_error "Unknown command: $command"
            show_help
            exit 1
            ;;
    esac
}

# Run main function with all arguments
main "$@"