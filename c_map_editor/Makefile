# NURBS Map Editor - Comprehensive Build System
# A Roblox Studio-like editor for NURBS-based FPS games with full IGES support

# Project info
PROJECT_NAME = nurbs_map_editor
VERSION = 1.0.0
DESCRIPTION = "NURBS Map Editor - Roblox Studio-like editor for NURBS-based FPS games"

# Directories
SRC_DIR = .
NURBS_SRC_DIR = ../src
BUILD_DIR = build
OBJ_DIR = $(BUILD_DIR)/obj
BIN_DIR = $(BUILD_DIR)/bin
DIST_DIR = $(BUILD_DIR)/dist

# Compiler settings
CC = gcc
CXX = g++
CFLAGS = -std=c11 -Wall -Wextra -Wpedantic -O2 -g
CXXFLAGS = -std=c++17 -Wall -Wextra -Wpedantic -O2 -g
LDFLAGS = -Wl,--as-needed

# Debug build settings
DEBUG_CFLAGS = -std=c11 -Wall -Wextra -Wpedantic -Og -g3 -DDEBUG -fsanitize=address
DEBUG_CXXFLAGS = -std=c++17 -Wall -Wextra -Wpedantic -Og -g3 -DDEBUG -fsanitize=address
DEBUG_LDFLAGS = -fsanitize=address

# Package dependencies
GTK_CFLAGS = $(shell pkg-config --cflags gtk+-3.0)
GTK_LIBS = $(shell pkg-config --libs gtk+-3.0)

GL_CFLAGS = $(shell pkg-config --cflags gl glu)
GL_LIBS = $(shell pkg-config --libs gl glu) -lGL -lGLU

GLFW_CFLAGS = $(shell pkg-config --cflags glfw3)
GLFW_LIBS = $(shell pkg-config --libs glfw3)

# Additional libraries
MATH_LIBS = -lm
PTHREAD_LIBS = -lpthread

# Combined flags
ALL_CFLAGS = $(CFLAGS) $(GTK_CFLAGS) $(GL_CFLAGS) $(GLFW_CFLAGS) -I$(NURBS_SRC_DIR)
ALL_LIBS = $(GTK_LIBS) $(GL_LIBS) $(GLFW_LIBS) $(MATH_LIBS) $(PTHREAD_LIBS)

# Debug flags
DEBUG_ALL_CFLAGS = $(DEBUG_CFLAGS) $(GTK_CFLAGS) $(GL_CFLAGS) $(GLFW_CFLAGS) -I$(NURBS_SRC_DIR)
DEBUG_ALL_LIBS = $(GTK_LIBS) $(GL_LIBS) $(GLFW_LIBS) $(MATH_LIBS) $(PTHREAD_LIBS)

# Source files
EDITOR_SOURCES = \
	main.c \
	editor.c \
	iges_loader.c \
	nurbs_primitives.c \
	viewport.c \
	scene_hierarchy.c \
	properties_panel.c \
	file_operations.c \
	undo_redo.c \
	camera_controls.c \
	selection_system.c \
	material_editor.c \
	lighting_system.c

# NURBS library sources
NURBS_SOURCES = \
	$(NURBS_SRC_DIR)/nurbs.c \
	$(NURBS_SRC_DIR)/math_utils.c \
	$(NURBS_SRC_DIR)/fps_engine.c \
	$(NURBS_SRC_DIR)/map_loader.c

# All sources
ALL_SOURCES = $(EDITOR_SOURCES) $(NURBS_SOURCES)

# Object files
EDITOR_OBJECTS = $(EDITOR_SOURCES:%.c=$(OBJ_DIR)/%.o)
NURBS_OBJECTS = $(NURBS_SOURCES:$(NURBS_SRC_DIR)/%.c=$(OBJ_DIR)/nurbs_%.o)
ALL_OBJECTS = $(EDITOR_OBJECTS) $(NURBS_OBJECTS)

# Debug object files
DEBUG_EDITOR_OBJECTS = $(EDITOR_SOURCES:%.c=$(OBJ_DIR)/debug_%.o)
DEBUG_NURBS_OBJECTS = $(NURBS_SOURCES:$(NURBS_SRC_DIR)/%.c=$(OBJ_DIR)/debug_nurbs_%.o)
DEBUG_ALL_OBJECTS = $(DEBUG_EDITOR_OBJECTS) $(DEBUG_NURBS_OBJECTS)

# Target executable
TARGET = $(BIN_DIR)/$(PROJECT_NAME)
DEBUG_TARGET = $(BIN_DIR)/$(PROJECT_NAME)_debug

# Default target
.PHONY: all
all: release

# Release build
.PHONY: release
release: $(TARGET)

# Debug build
.PHONY: debug
debug: $(DEBUG_TARGET)

# Create directories
$(BUILD_DIR):
	@mkdir -p $(BUILD_DIR)

$(OBJ_DIR): | $(BUILD_DIR)
	@mkdir -p $(OBJ_DIR)

$(BIN_DIR): | $(BUILD_DIR)
	@mkdir -p $(BIN_DIR)

$(DIST_DIR): | $(BUILD_DIR)
	@mkdir -p $(DIST_DIR)

# Release target
$(TARGET): $(ALL_OBJECTS) | $(BIN_DIR)
	@echo "Linking $(PROJECT_NAME)..."
	$(CC) $(LDFLAGS) -o $@ $^ $(ALL_LIBS)
	@echo "Build complete: $@"

# Debug target
$(DEBUG_TARGET): $(DEBUG_ALL_OBJECTS) | $(BIN_DIR)
	@echo "Linking $(PROJECT_NAME) (debug)..."
	$(CC) $(DEBUG_LDFLAGS) -o $@ $^ $(DEBUG_ALL_LIBS)
	@echo "Debug build complete: $@"

# Release object files
$(OBJ_DIR)/%.o: %.c | $(OBJ_DIR)
	@echo "Compiling $<..."
	$(CC) $(ALL_CFLAGS) -c $< -o $@

$(OBJ_DIR)/nurbs_%.o: $(NURBS_SRC_DIR)/%.c | $(OBJ_DIR)
	@echo "Compiling $<..."
	$(CC) $(ALL_CFLAGS) -c $< -o $@

# Debug object files
$(OBJ_DIR)/debug_%.o: %.c | $(OBJ_DIR)
	@echo "Compiling $< (debug)..."
	$(CC) $(DEBUG_ALL_CFLAGS) -c $< -o $@

$(OBJ_DIR)/debug_nurbs_%.o: $(NURBS_SRC_DIR)/%.c | $(OBJ_DIR)
	@echo "Compiling $< (debug)..."
	$(CC) $(DEBUG_ALL_CFLAGS) -c $< -o $@

# Clean build files
.PHONY: clean
clean:
	@echo "Cleaning build files..."
	rm -rf $(BUILD_DIR)

# Install target
.PHONY: install
install: release
	@echo "Installing $(PROJECT_NAME)..."
	install -d $(DESTDIR)/usr/bin
	install -m 755 $(TARGET) $(DESTDIR)/usr/bin/$(PROJECT_NAME)
	install -d $(DESTDIR)/usr/share/applications
	install -m 644 $(PROJECT_NAME).desktop $(DESTDIR)/usr/share/applications/
	install -d $(DESTDIR)/usr/share/pixmaps
	install -m 644 icons/$(PROJECT_NAME).png $(DESTDIR)/usr/share/pixmaps/
	@echo "Installation complete"

# Uninstall target
.PHONY: uninstall
uninstall:
	@echo "Uninstalling $(PROJECT_NAME)..."
	rm -f $(DESTDIR)/usr/bin/$(PROJECT_NAME)
	rm -f $(DESTDIR)/usr/share/applications/$(PROJECT_NAME).desktop
	rm -f $(DESTDIR)/usr/share/pixmaps/$(PROJECT_NAME).png
	@echo "Uninstallation complete"

# Package creation
.PHONY: package
package: release | $(DIST_DIR)
	@echo "Creating package..."
	mkdir -p $(DIST_DIR)/$(PROJECT_NAME)-$(VERSION)/bin
	mkdir -p $(DIST_DIR)/$(PROJECT_NAME)-$(VERSION)/share/applications
	mkdir -p $(DIST_DIR)/$(PROJECT_NAME)-$(VERSION)/share/pixmaps
	mkdir -p $(DIST_DIR)/$(PROJECT_NAME)-$(VERSION)/doc
	
	cp $(TARGET) $(DIST_DIR)/$(PROJECT_NAME)-$(VERSION)/bin/
	cp $(PROJECT_NAME).desktop $(DIST_DIR)/$(PROJECT_NAME)-$(VERSION)/share/applications/
	cp icons/$(PROJECT_NAME).png $(DIST_DIR)/$(PROJECT_NAME)-$(VERSION)/share/pixmaps/
	cp README.md $(DIST_DIR)/$(PROJECT_NAME)-$(VERSION)/doc/
	cp LICENSE $(DIST_DIR)/$(PROJECT_NAME)-$(VERSION)/doc/
	cp CHANGELOG.md $(DIST_DIR)/$(PROJECT_NAME)-$(VERSION)/doc/
	
	cd $(DIST_DIR) && tar -czf $(PROJECT_NAME)-$(VERSION).tar.gz $(PROJECT_NAME)-$(VERSION)/
	@echo "Package created: $(DIST_DIR)/$(PROJECT_NAME)-$(VERSION).tar.gz"

# Debian package
.PHONY: deb
deb: release | $(DIST_DIR)
	@echo "Creating Debian package..."
	mkdir -p $(DIST_DIR)/debian/DEBIAN
	mkdir -p $(DIST_DIR)/debian/usr/bin
	mkdir -p $(DIST_DIR)/debian/usr/share/applications
	mkdir -p $(DIST_DIR)/debian/usr/share/pixmaps
	mkdir -p $(DIST_DIR)/debian/usr/share/doc/$(PROJECT_NAME)
	
	cp $(TARGET) $(DIST_DIR)/debian/usr/bin/
	cp $(PROJECT_NAME).desktop $(DIST_DIR)/debian/usr/share/applications/
	cp icons/$(PROJECT_NAME).png $(DIST_DIR)/debian/usr/share/pixmaps/
	cp README.md $(DIST_DIR)/debian/usr/share/doc/$(PROJECT_NAME)/
	cp LICENSE $(DIST_DIR)/debian/usr/share/doc/$(PROJECT_NAME)/
	cp CHANGELOG.md $(DIST_DIR)/debian/usr/share/doc/$(PROJECT_NAME)/
	
	echo "Package: $(PROJECT_NAME)" > $(DIST_DIR)/debian/DEBIAN/control
	echo "Version: $(VERSION)" >> $(DIST_DIR)/debian/DEBIAN/control
	echo "Section: graphics" >> $(DIST_DIR)/debian/DEBIAN/control
	echo "Priority: optional" >> $(DIST_DIR)/debian/DEBIAN/control
	echo "Architecture: amd64" >> $(DIST_DIR)/debian/DEBIAN/control
	echo "Depends: libgtk-3-0, libgl1-mesa-glx, libglu1-mesa, libglfw3" >> $(DIST_DIR)/debian/DEBIAN/control
	echo "Maintainer: NURBS Editor Team <contact@nurbseditor.com>" >> $(DIST_DIR)/debian/DEBIAN/control
	echo "Description: $(DESCRIPTION)" >> $(DIST_DIR)/debian/DEBIAN/control
	echo " A comprehensive NURBS-based map editor with Roblox Studio-like interface" >> $(DIST_DIR)/debian/DEBIAN/control
	echo " and full IGES file format support for creating FPS game maps." >> $(DIST_DIR)/debian/DEBIAN/control
	
	dpkg-deb --build $(DIST_DIR)/debian $(DIST_DIR)/$(PROJECT_NAME)_$(VERSION)_amd64.deb
	@echo "Debian package created: $(DIST_DIR)/$(PROJECT_NAME)_$(VERSION)_amd64.deb"

# Run the application
.PHONY: run
run: release
	@echo "Running $(PROJECT_NAME)..."
	$(TARGET)

# Run debug version
.PHONY: run-debug
run-debug: debug
	@echo "Running $(PROJECT_NAME) (debug)..."
	$(DEBUG_TARGET)

# Test target
.PHONY: test
test: debug
	@echo "Running tests..."
	# Add test commands here
	@echo "Tests completed"

# Check dependencies
.PHONY: check-deps
check-deps:
	@echo "Checking dependencies..."
	@echo -n "GTK+ 3.0: "
	@pkg-config --exists gtk+-3.0 && echo "OK" || echo "MISSING"
	@echo -n "OpenGL: "
	@pkg-config --exists gl && echo "OK" || echo "MISSING"
	@echo -n "GLU: "
	@pkg-config --exists glu && echo "OK" || echo "MISSING"
	@echo -n "GLFW3: "
	@pkg-config --exists glfw3 && echo "OK" || echo "MISSING"
	@echo "Dependency check complete"

# Install dependencies (Ubuntu/Debian)
.PHONY: install-deps-ubuntu
install-deps-ubuntu:
	@echo "Installing dependencies for Ubuntu/Debian..."
	sudo apt-get update
	sudo apt-get install -y \
		build-essential \
		pkg-config \
		libgtk-3-dev \
		libgl1-mesa-dev \
		libglu1-mesa-dev \
		libglfw3-dev \
		libglew-dev
	@echo "Dependencies installed"

# Install dependencies (Fedora/CentOS/RHEL)
.PHONY: install-deps-fedora
install-deps-fedora:
	@echo "Installing dependencies for Fedora/CentOS/RHEL..."
	sudo dnf install -y \
		gcc \
		gcc-c++ \
		make \
		pkgconfig \
		gtk3-devel \
		mesa-libGL-devel \
		mesa-libGLU-devel \
		glfw-devel \
		glew-devel
	@echo "Dependencies installed"

# Install dependencies (Arch Linux)
.PHONY: install-deps-arch
install-deps-arch:
	@echo "Installing dependencies for Arch Linux..."
	sudo pacman -S --needed \
		base-devel \
		pkgconf \
		gtk3 \
		mesa \
		glu \
		glfw \
		glew
	@echo "Dependencies installed"

# Format code
.PHONY: format
format:
	@echo "Formatting code..."
	find . -name "*.c" -o -name "*.h" | xargs clang-format -i
	@echo "Code formatting complete"

# Static analysis
.PHONY: analyze
analyze:
	@echo "Running static analysis..."
	cppcheck --enable=all --std=c11 $(EDITOR_SOURCES)
	@echo "Static analysis complete"

# Memory check
.PHONY: memcheck
memcheck: debug
	@echo "Running memory check..."
	valgrind --tool=memcheck --leak-check=full --show-leak-kinds=all $(DEBUG_TARGET)

# Generate documentation
.PHONY: docs
docs:
	@echo "Generating documentation..."
	doxygen Doxyfile
	@echo "Documentation generated in docs/"

# Help target
.PHONY: help
help:
	@echo "NURBS Map Editor Build System"
	@echo "============================"
	@echo ""
	@echo "Available targets:"
	@echo "  all                 - Build release version (default)"
	@echo "  release             - Build optimized release version"
	@echo "  debug               - Build debug version with symbols"
	@echo "  clean               - Remove all build files"
	@echo "  install             - Install to system (requires sudo)"
	@echo "  uninstall           - Remove from system (requires sudo)"
	@echo "  package             - Create tar.gz package"
	@echo "  deb                 - Create Debian package"
	@echo "  run                 - Run release version"
	@echo "  run-debug           - Run debug version"
	@echo "  test                - Run tests"
	@echo "  check-deps          - Check for required dependencies"
	@echo "  install-deps-ubuntu - Install dependencies on Ubuntu/Debian"
	@echo "  install-deps-fedora - Install dependencies on Fedora/CentOS/RHEL"
	@echo "  install-deps-arch   - Install dependencies on Arch Linux"
	@echo "  format              - Format source code"
	@echo "  analyze             - Run static analysis"
	@echo "  memcheck            - Run memory leak check"
	@echo "  docs                - Generate documentation"
	@echo "  help                - Show this help"
	@echo ""
	@echo "Project: $(PROJECT_NAME) v$(VERSION)"
	@echo "Description: $(DESCRIPTION)"

# Dependencies
-include $(ALL_OBJECTS:.o=.d)
-include $(DEBUG_ALL_OBJECTS:.o=.d)