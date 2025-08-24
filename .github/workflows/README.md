# GitHub Actions CI/CD Pipeline

This document describes the comprehensive CI/CD pipeline for the NURBS Map Editor project.

## Overview

The pipeline automatically builds, tests, and releases the NURBS Map Editor for multiple platforms:
- **Linux**: Ubuntu 20.04 and 22.04 (x86_64)
- **Windows**: MSYS2 with MinGW (x86_64 and i686)

## Workflow Structure

### 1. Build Matrix

#### Linux Builds
- **Ubuntu 20.04**: For broader compatibility
- **Ubuntu 22.04**: Latest LTS with modern dependencies
- **Architecture**: x86_64 only

#### Windows Builds
- **MSYS2 MinGW64**: 64-bit Windows (x86_64)
- **MSYS2 MinGW32**: 32-bit Windows (i686)
- **Compatibility**: Windows 7 and later

### 2. Pipeline Jobs

#### `build-linux`
- Installs GTK+3, OpenGL, GLFW, and other dependencies
- Builds both release and debug versions
- Runs static analysis with cppcheck
- Creates tar.gz and Debian packages
- Uploads artifacts for download

#### `build-windows`
- Sets up MSYS2 environment with MinGW toolchain
- Installs Windows-specific dependencies
- Builds for both x86_64 and i686 architectures
- Creates ZIP packages with bundled DLLs
- Handles Windows-specific compilation flags

#### `code-quality`
- Runs comprehensive static analysis
- Checks code formatting with clang-format
- Performs security scanning with CodeQL
- Ensures code quality standards

#### `release`
- Triggered only on version tags (v*)
- Downloads all build artifacts
- Creates GitHub release with changelog
- Uploads platform-specific packages
- Provides detailed installation instructions

#### `notify`
- Reports build status
- Provides feedback on success/failure

## Triggers

### Automatic Triggers
- **Push to main/develop**: Full build and test
- **Pull requests**: Build and test only
- **Version tags (v*)**: Full build, test, and release

### Manual Triggers
- Can be triggered manually from GitHub Actions tab
- Useful for testing CI/CD changes

## Artifacts

### Linux Artifacts
- `nurbs_map_editor-{version}-linux.tar.gz`: Source distribution
- `nurbs_map_editor_{version}_amd64.deb`: Debian package
- Debug builds available as separate artifacts

### Windows Artifacts
- `nurbs_map_editor-{version}-windows-x64.zip`: 64-bit Windows
- `nurbs_map_editor-{version}-windows-x86.zip`: 32-bit Windows
- Includes all required DLLs for standalone operation

## Dependencies

### Linux Dependencies
```bash
# Ubuntu/Debian
sudo apt-get install build-essential pkg-config libgtk-3-dev \
  libgl1-mesa-dev libglu1-mesa-dev libglfw3-dev libglew-dev
```

### Windows Dependencies (MSYS2)
```bash
# MSYS2 packages
pacman -S mingw-w64-x86_64-toolchain mingw-w64-x86_64-gtk3 \
  mingw-w64-x86_64-mesa mingw-w64-x86_64-glfw mingw-w64-x86_64-glew
```

## Build Configuration

### Compiler Flags
- **Linux**: GCC with `-std=c11 -Wall -Wextra -Wpedantic -O2`
- **Windows**: MinGW-GCC with Windows-specific flags
- **Debug**: AddressSanitizer enabled for memory debugging

### Optimization
- **Release**: `-O2` optimization for performance
- **Debug**: `-Og` with full debug symbols
- **Static Analysis**: Comprehensive checks with cppcheck

## Release Process

### Version Tagging
1. Update version in relevant files
2. Create and push a version tag: `git tag v1.0.0 && git push origin v1.0.0`
3. CI/CD automatically creates GitHub release

### Release Assets
- Source code (automatic GitHub feature)
- Linux tar.gz package
- Debian package for Ubuntu/Debian
- Windows ZIP packages (32-bit and 64-bit)
- Comprehensive release notes

## Quality Assurance

### Static Analysis
- **cppcheck**: C/C++ static analyzer
- **clang-format**: Code formatting verification
- **CodeQL**: Security vulnerability scanning

### Testing
- Unit tests (when implemented)
- Integration tests
- Memory leak detection with Valgrind (Linux)
- AddressSanitizer for memory safety

### Code Coverage
- Prepared for code coverage reporting
- Can be extended with gcov/lcov integration

## Platform-Specific Notes

### Linux
- Uses native package managers for dependencies
- Creates both portable and installable packages
- Supports multiple Ubuntu versions for compatibility

### Windows
- MSYS2 provides POSIX-like environment on Windows
- Automatic DLL bundling for standalone operation
- Supports both 32-bit and 64-bit architectures
- Windows-specific resource compilation

## Troubleshooting

### Common Issues

#### Linux Build Failures
```bash
# Missing dependencies
sudo apt-get update && sudo apt-get install -f

# GTK development headers missing
sudo apt-get install libgtk-3-dev
```

#### Windows Build Failures
```bash
# MSYS2 package database out of date
pacman -Syu

# Missing MinGW toolchain
pacman -S mingw-w64-x86_64-toolchain
```

### Debug Information
- All builds include debug symbols
- Debug versions available as separate artifacts
- Comprehensive logging in CI/CD output

## Extending the Pipeline

### Adding New Platforms
1. Create new job in `.github/workflows/ci-cd.yml`
2. Add platform-specific dependencies
3. Configure build matrix
4. Update release job to include new artifacts

### Adding Tests
1. Implement tests in the project
2. Add test execution to build jobs
3. Configure test result reporting
4. Add coverage reporting if needed

### Security Enhancements
- CodeQL is already integrated
- Can add dependency vulnerability scanning
- SAST (Static Application Security Testing) tools
- Container security scanning for future Docker support

## Monitoring and Maintenance

### Workflow Monitoring
- GitHub Actions provides detailed logs
- Build status badges available
- Email notifications on failure (configurable)

### Maintenance Tasks
- Regular dependency updates
- Security patch management
- Performance optimization
- Documentation updates

## Contact and Support

For issues with the CI/CD pipeline:
- Create GitHub issue with `ci/cd` label
- Check Actions tab for detailed logs
- Refer to this documentation for common solutions

---

This CI/CD pipeline ensures reliable, automated building and distribution of the NURBS Map Editor across multiple platforms while maintaining high code quality standards.