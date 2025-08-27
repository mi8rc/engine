# NURBS Map Editor - Windows MSYS2 Build System
# Specialized Makefile for Windows compilation with MSYS2/MinGW

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

# Detect platform
UNAME_S := $(shell uname -s)

# Platform-specific settings
ifeq ($(UNAME_S),Linux)
    # Linux settings
    EXE_SUFFIX = 
    PLATFORM_CFLAGS = -DLINUX
    PLATFORM_LDFLAGS = 
    WIN_LIBS = 
    WINDRES = 
else ifeq ($(OS),Windows_NT)
    # Windows/MSYS2 settings
    EXE_SUFFIX = .exe
    TARGET_ARCH ?= x86_64
    MSYS2_PREFIX = /mingw64
    ifeq ($(TARGET_ARCH),i686)
        MSYS2_PREFIX = /mingw32
    endif
    PLATFORM_CFLAGS = -DWIN32 -D_WIN32_WINNT=0x0601
    PLATFORM_LDFLAGS = -Wl,--subsystem,console
    WIN_LIBS = -luser32 -lkernel32 -lgdi32 -lwinspool -lcomdlg32 -ladvapi32 -lshell32 -lole32 -loleaut32 -luuid -lodbc32 -lodbccp32
    WINDRES = windres
else
    # Default Unix-like settings
    EXE_SUFFIX = 
    PLATFORM_CFLAGS = -DUNIX
    PLATFORM_LDFLAGS = 
    WIN_LIBS = 
    WINDRES = 
endif

# Compiler settings
CC = gcc
CXX = g++
CFLAGS = -std=c11 -Wall -Wextra -Wpedantic -O2 -g $(PLATFORM_CFLAGS)
CXXFLAGS = -std=c++17 -Wall -Wextra -Wpedantic -O2 -g $(PLATFORM_CFLAGS)
LDFLAGS = $(PLATFORM_LDFLAGS)

# Debug build settings
DEBUG_CFLAGS = -std=c11 -Wall -Wextra -Wpedantic -Og -g3 -DDEBUG $(PLATFORM_CFLAGS)
DEBUG_CXXFLAGS = -std=c++17 -Wall -Wextra -Wpedantic -Og -g3 -DDEBUG $(PLATFORM_CFLAGS)
ifeq ($(UNAME_S),Linux)
    DEBUG_LDFLAGS = $(PLATFORM_LDFLAGS)
else
    # Windows debug should use console subsystem for better debugging
    DEBUG_LDFLAGS = -Wl,--subsystem,console
endif

# Package dependencies
GTK_CFLAGS = $(shell pkg-config --cflags gtk+-3.0 2>/dev/null)
GTK_LIBS = $(shell pkg-config --libs gtk+-3.0 2>/dev/null)

ifeq ($(UNAME_S),Linux)
    # Linux OpenGL libraries
    GL_CFLAGS = $(shell pkg-config --cflags gl glu glew 2>/dev/null)
    GL_LIBS = $(shell pkg-config --libs gl glu glew 2>/dev/null || echo "-lGL -lGLU -lGLEW")
    
    GLFW_CFLAGS = $(shell pkg-config --cflags glfw3 2>/dev/null)
    GLFW_LIBS = $(shell pkg-config --libs glfw3 2>/dev/null || echo "-lglfw")
    
    JSON_CFLAGS = $(shell pkg-config --cflags json-c 2>/dev/null)
    JSON_LIBS = $(shell pkg-config --libs json-c 2>/dev/null || echo "-ljson-c")
else
    # Windows/MSYS2 OpenGL libraries
    GL_CFLAGS = $(shell pkg-config --cflags gl glu glew 2>/dev/null || echo "-I$(MSYS2_PREFIX)/include")
    GL_LIBS = $(shell pkg-config --libs gl glu glew 2>/dev/null || echo "-lopengl32 -lglu32 -lglew32")
    
    GLFW_CFLAGS = $(shell pkg-config --cflags glfw3 2>/dev/null || echo "-I$(MSYS2_PREFIX)/include")
    GLFW_LIBS = $(shell pkg-config --libs glfw3 2>/dev/null || echo "-lglfw3")
    
    JSON_CFLAGS = $(shell pkg-config --cflags json-c 2>/dev/null || echo "-I$(MSYS2_PREFIX)/include")
    JSON_LIBS = $(shell pkg-config --libs json-c 2>/dev/null || echo "-ljson-c")
endif

# Additional libraries
MATH_LIBS = -lm
ifeq ($(UNAME_S),Linux)
    PTHREAD_LIBS = -lpthread
else
    # Windows doesn't need explicit pthread linking with mingw
    PTHREAD_LIBS = 
endif

# Combined flags
ALL_CFLAGS = $(CFLAGS) $(GTK_CFLAGS) $(GL_CFLAGS) $(GLFW_CFLAGS) $(JSON_CFLAGS) -I$(NURBS_SRC_DIR)
# Order libraries properly for Windows linking
ifeq ($(UNAME_S),Linux)
    ALL_LIBS = $(GTK_LIBS) $(GL_LIBS) $(GLFW_LIBS) $(JSON_LIBS) $(WIN_LIBS) $(MATH_LIBS) $(PTHREAD_LIBS)
else
    # Windows library order matters more - put system libs last
    ALL_LIBS = $(GTK_LIBS) $(GL_LIBS) $(GLFW_LIBS) $(JSON_LIBS) $(MATH_LIBS) $(PTHREAD_LIBS) $(WIN_LIBS)
endif

# Debug flags
DEBUG_ALL_CFLAGS = $(DEBUG_CFLAGS) $(GTK_CFLAGS) $(GL_CFLAGS) $(GLFW_CFLAGS) $(JSON_CFLAGS) -I$(NURBS_SRC_DIR)
ifeq ($(UNAME_S),Linux)
    DEBUG_ALL_LIBS = $(GTK_LIBS) $(GL_LIBS) $(GLFW_LIBS) $(JSON_LIBS) $(WIN_LIBS) $(MATH_LIBS) $(PTHREAD_LIBS)
else
    DEBUG_ALL_LIBS = $(GTK_LIBS) $(GL_LIBS) $(GLFW_LIBS) $(JSON_LIBS) $(MATH_LIBS) $(PTHREAD_LIBS) $(WIN_LIBS)
endif

# Source files
EDITOR_SOURCES = \
	main.c \
	editor.c \
	iges_loader.c \
	nurbs_primitives.c

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

# Resource files
RESOURCE_RC = $(PROJECT_NAME).rc
RESOURCE_OBJ = $(OBJ_DIR)/$(PROJECT_NAME).res.o

# Target executables
TARGET = $(BIN_DIR)/$(PROJECT_NAME)$(EXE_SUFFIX)
DEBUG_TARGET = $(BIN_DIR)/$(PROJECT_NAME)_debug$(EXE_SUFFIX)

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

# Create Windows resource file
$(RESOURCE_RC):
	@echo "Creating Windows resource file..."
	@echo "#include <windows.h>" > $(RESOURCE_RC)
	@echo "" >> $(RESOURCE_RC)
	@echo "VS_VERSION_INFO VERSIONINFO" >> $(RESOURCE_RC)
	@echo "FILEVERSION 1,0,0,0" >> $(RESOURCE_RC)
	@echo "PRODUCTVERSION 1,0,0,0" >> $(RESOURCE_RC)
	@echo "FILEFLAGSMASK VS_FFI_FILEFLAGSMASK" >> $(RESOURCE_RC)
	@echo "FILEFLAGS 0x0L" >> $(RESOURCE_RC)
	@echo "FILEOS VOS_NT_WINDOWS32" >> $(RESOURCE_RC)
	@echo "FILETYPE VFT_APP" >> $(RESOURCE_RC)
	@echo "FILESUBTYPE VFT2_UNKNOWN" >> $(RESOURCE_RC)
	@echo "BEGIN" >> $(RESOURCE_RC)
	@echo "    BLOCK \"StringFileInfo\"" >> $(RESOURCE_RC)
	@echo "    BEGIN" >> $(RESOURCE_RC)
	@echo "        BLOCK \"040904b0\"" >> $(RESOURCE_RC)
	@echo "        BEGIN" >> $(RESOURCE_RC)
	@echo "            VALUE \"CompanyName\", \"NURBS Editor Team\"" >> $(RESOURCE_RC)
	@echo "            VALUE \"FileDescription\", \"$(DESCRIPTION)\"" >> $(RESOURCE_RC)
	@echo "            VALUE \"FileVersion\", \"$(VERSION)\"" >> $(RESOURCE_RC)
	@echo "            VALUE \"InternalName\", \"$(PROJECT_NAME)\"" >> $(RESOURCE_RC)
	@echo "            VALUE \"LegalCopyright\", \"Copyright (C) 2024 NURBS Editor Team\"" >> $(RESOURCE_RC)
	@echo "            VALUE \"OriginalFilename\", \"$(PROJECT_NAME).exe\"" >> $(RESOURCE_RC)
	@echo "            VALUE \"ProductName\", \"NURBS Map Editor\"" >> $(RESOURCE_RC)
	@echo "            VALUE \"ProductVersion\", \"$(VERSION)\"" >> $(RESOURCE_RC)
	@echo "        END" >> $(RESOURCE_RC)
	@echo "    END" >> $(RESOURCE_RC)
	@echo "    BLOCK \"VarFileInfo\"" >> $(RESOURCE_RC)
	@echo "    BEGIN" >> $(RESOURCE_RC)
	@echo "        VALUE \"Translation\", 0x409, 1200" >> $(RESOURCE_RC)
	@echo "    END" >> $(RESOURCE_RC)
	@echo "END" >> $(RESOURCE_RC)

# Compile resource file
$(RESOURCE_OBJ): $(RESOURCE_RC) | $(OBJ_DIR)
ifeq ($(UNAME_S),Linux)
	@echo "Skipping Windows resources on Linux..."
	@touch $(RESOURCE_OBJ)
else
	@echo "Compiling Windows resources..."
	$(WINDRES) -i $(RESOURCE_RC) -o $(RESOURCE_OBJ)
endif

# Release target
$(TARGET): $(ALL_OBJECTS) $(RESOURCE_OBJ) | $(BIN_DIR)
	@echo "Linking $(PROJECT_NAME) for Windows..."
	@echo "Libraries: $(ALL_LIBS)"
	$(CC) $(LDFLAGS) -o $@ $^ $(ALL_LIBS)
	@echo "Windows build complete: $@"

# Debug target
$(DEBUG_TARGET): $(DEBUG_ALL_OBJECTS) $(RESOURCE_OBJ) | $(BIN_DIR)
	@echo "Linking $(PROJECT_NAME) (debug) for Windows..."
	$(CC) $(DEBUG_LDFLAGS) -o $@ $^ $(DEBUG_ALL_LIBS)
	@echo "Windows debug build complete: $@"

# Release object files
$(OBJ_DIR)/%.o: %.c | $(OBJ_DIR)
	@echo "Compiling $< for Windows..."
	$(CC) $(ALL_CFLAGS) -c $< -o $@

$(OBJ_DIR)/nurbs_%.o: $(NURBS_SRC_DIR)/%.c | $(OBJ_DIR)
	@echo "Compiling $< for Windows..."
	$(CC) $(ALL_CFLAGS) -c $< -o $@

# Debug object files
$(OBJ_DIR)/debug_%.o: %.c | $(OBJ_DIR)
	@echo "Compiling $< (debug) for Windows..."
	$(CC) $(DEBUG_ALL_CFLAGS) -c $< -o $@

$(OBJ_DIR)/debug_nurbs_%.o: $(NURBS_SRC_DIR)/%.c | $(OBJ_DIR)
	@echo "Compiling $< (debug) for Windows..."
	$(CC) $(DEBUG_ALL_CFLAGS) -c $< -o $@

# Clean build files
.PHONY: clean
clean:
	@echo "Cleaning Windows build files..."
	rm -rf $(BUILD_DIR)
	rm -f $(RESOURCE_RC)

# Package for Windows
.PHONY: package
package: release | $(DIST_DIR)
	@echo "Creating Windows package..."
	mkdir -p $(DIST_DIR)/$(PROJECT_NAME)-$(VERSION)-windows-$(TARGET_ARCH)
	
	# Copy executable
	cp $(TARGET) $(DIST_DIR)/$(PROJECT_NAME)-$(VERSION)-windows-$(TARGET_ARCH)/
	
	# Copy documentation
	cp README.md $(DIST_DIR)/$(PROJECT_NAME)-$(VERSION)-windows-$(TARGET_ARCH)/
	cp LICENSE $(DIST_DIR)/$(PROJECT_NAME)-$(VERSION)-windows-$(TARGET_ARCH)/
	cp CHANGELOG.md $(DIST_DIR)/$(PROJECT_NAME)-$(VERSION)-windows-$(TARGET_ARCH)/
	
	# Copy required DLLs
	@echo "Copying required DLLs..."
	-ldd $(TARGET) | grep mingw | awk '{print $$3}' | xargs -I {} cp {} $(DIST_DIR)/$(PROJECT_NAME)-$(VERSION)-windows-$(TARGET_ARCH)/
	
	# Create ZIP package
	cd $(DIST_DIR) && zip -r $(PROJECT_NAME)-$(VERSION)-windows-$(TARGET_ARCH).zip $(PROJECT_NAME)-$(VERSION)-windows-$(TARGET_ARCH)/
	@echo "Windows package created: $(DIST_DIR)/$(PROJECT_NAME)-$(VERSION)-windows-$(TARGET_ARCH).zip"

# Run the application
.PHONY: run
run: release
	@echo "Running $(PROJECT_NAME) on Windows..."
	$(TARGET)

# Run debug version
.PHONY: run-debug
run-debug: debug
	@echo "Running $(PROJECT_NAME) (debug) on Windows..."
	$(DEBUG_TARGET)

# Check dependencies
.PHONY: check-deps
check-deps:
	@echo "Checking Windows dependencies..."
	@echo -n "GTK+ 3.0: "
	@pkg-config --exists gtk+-3.0 && echo "OK" || echo "MISSING"
	@echo -n "OpenGL: "
	@pkg-config --exists gl && echo "OK" || (test -f $(MSYS2_PREFIX)/lib/libopengl32.a && echo "OK" || echo "MISSING")
	@echo -n "GLU: "
	@pkg-config --exists glu && echo "OK" || (test -f $(MSYS2_PREFIX)/lib/libglu32.a && echo "OK" || echo "MISSING")
	@echo -n "GLFW3: "
	@pkg-config --exists glfw3 && echo "OK" || (test -f $(MSYS2_PREFIX)/lib/libglfw3.a && echo "OK" || echo "MISSING")
	@echo "Windows dependency check complete"

# Install dependencies (MSYS2)
.PHONY: install-deps
install-deps:
	@echo "Installing dependencies for Windows (MSYS2)..."
	pacman -S --needed --noconfirm \
		mingw-w64-$(TARGET_ARCH)-toolchain \
		mingw-w64-$(TARGET_ARCH)-cmake \
		mingw-w64-$(TARGET_ARCH)-pkg-config \
		mingw-w64-$(TARGET_ARCH)-gtk3 \
		mingw-w64-$(TARGET_ARCH)-mesa \
		mingw-w64-$(TARGET_ARCH)-glfw \
		mingw-w64-$(TARGET_ARCH)-glew \
		mingw-w64-$(TARGET_ARCH)-cppcheck
	@echo "Windows dependencies installed"

# Static analysis
.PHONY: analyze
analyze:
	@echo "Running static analysis on Windows..."
	-cppcheck --enable=all --std=c11 --platform=win64 $(EDITOR_SOURCES)
	@echo "Windows static analysis complete"

# Test linking with minimal libraries
.PHONY: test-link
test-link: $(ALL_OBJECTS) | $(BIN_DIR)
	@echo "Testing minimal linking..."
	$(CC) $(LDFLAGS) -o build/bin/test_link.exe $^ -lkernel32 -luser32
	@echo "Minimal linking test passed"

# Help target
.PHONY: help
help:
	@echo "NURBS Map Editor - Windows Build System (MSYS2)"
	@echo "==============================================="
	@echo ""
	@echo "Available targets:"
	@echo "  all                 - Build release version (default)"
	@echo "  release             - Build optimized release version"
	@echo "  debug               - Build debug version with symbols"
	@echo "  clean               - Remove all build files"
	@echo "  package             - Create Windows ZIP package"
	@echo "  run                 - Run release version"
	@echo "  run-debug           - Run debug version"
	@echo "  check-deps          - Check for required dependencies"
	@echo "  install-deps        - Install dependencies via MSYS2"
	@echo "  analyze             - Run static analysis"
	@echo "  help                - Show this help"
	@echo ""
	@echo "Environment variables:"
	@echo "  TARGET_ARCH         - Target architecture (x86_64 or i686)"
	@echo ""
	@echo "Project: $(PROJECT_NAME) v$(VERSION)"
	@echo "Target Architecture: $(TARGET_ARCH)"
	@echo "MSYS2 Prefix: $(MSYS2_PREFIX)"

# Dependencies
-include $(ALL_OBJECTS:.o=.d)
-include $(DEBUG_ALL_OBJECTS:.o=.d)
