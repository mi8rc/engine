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
BUILD_MODE ?= auto

# Enhanced platform detection with build mode support
# Usage: make BUILD_MODE=windows (to force Windows build)
#        make BUILD_MODE=linux  (to force Linux build)
#        make                   (auto-detect)

# Set IS_WINDOWS based on platform detection and build mode
ifeq ($(BUILD_MODE),windows)
    IS_WINDOWS = 1
    DETECTED_PLATFORM = Windows (forced)
else ifeq ($(BUILD_MODE),linux)
    IS_WINDOWS = 0
    DETECTED_PLATFORM = Linux (forced)
else ifneq ($(findstring MINGW,$(UNAME_S)),)
    IS_WINDOWS = 1
    DETECTED_PLATFORM = Windows/MSYS2 ($(UNAME_S))
else ifeq ($(OS),Windows_NT)
    IS_WINDOWS = 1
    DETECTED_PLATFORM = Windows ($(OS))
else
    IS_WINDOWS = 0
    DETECTED_PLATFORM = Unix-like ($(UNAME_S))
endif

# Platform-specific settings based on IS_WINDOWS
ifeq ($(IS_WINDOWS),1)
    # Windows/MSYS2 settings
    EXE_SUFFIX = .exe
    TARGET_ARCH ?= x86_64
    MSYS2_PREFIX = /mingw64
    ifeq ($(TARGET_ARCH),i686)
        MSYS2_PREFIX = /mingw32
    endif
    PLATFORM_CFLAGS = -DWIN32 -D_WIN32_WINNT=0x0601
    PLATFORM_LDFLAGS = -Wl,--subsystem,console
    WINDRES = windres
    
    # Windows linking strategy: use minimal additional libs since GTK provides most
    WIN_LIBS = 
    LINK_STRATEGY = windows_safe
else
    # Linux/Unix settings  
    EXE_SUFFIX = 
    PLATFORM_CFLAGS = -DLINUX
    PLATFORM_LDFLAGS = 
    WINDRES = 
    WIN_LIBS = 
    LINK_STRATEGY = unix_standard
endif

# Compiler settings
CC = gcc
CXX = g++
CFLAGS = -std=c11 -Wall -Wextra -Wpedantic -O2 -g $(PLATFORM_CFLAGS)
CXXFLAGS = -std=c++17 -Wall -Wextra -Wpedantic -O2 -g $(PLATFORM_CFLAGS)
LDFLAGS = $(PLATFORM_LDFLAGS)

# Windows-specific fixes for glibc compatibility
ifeq ($(IS_WINDOWS),1)
    # Disable glibc fortify functions that don't exist on Windows
    CFLAGS += -U_FORTIFY_SOURCE -D_FORTIFY_SOURCE=0
    CXXFLAGS += -U_FORTIFY_SOURCE -D_FORTIFY_SOURCE=0
    # Use GLEW_STATIC for Windows to avoid DLL dependencies
    CFLAGS += -DGLEW_STATIC
    CXXFLAGS += -DGLEW_STATIC
endif

# Debug build settings
DEBUG_CFLAGS = -std=c11 -Wall -Wextra -Wpedantic -Og -g3 -DDEBUG $(PLATFORM_CFLAGS)
DEBUG_CXXFLAGS = -std=c++17 -Wall -Wextra -Wpedantic -Og -g3 -DDEBUG $(PLATFORM_CFLAGS)

# Apply Windows fixes to debug builds too
ifeq ($(IS_WINDOWS),1)
    DEBUG_CFLAGS += -U_FORTIFY_SOURCE -D_FORTIFY_SOURCE=0 -DGLEW_STATIC
    DEBUG_CXXFLAGS += -U_FORTIFY_SOURCE -D_FORTIFY_SOURCE=0 -DGLEW_STATIC
    # Windows debug should use console subsystem for better debugging
    DEBUG_LDFLAGS = -Wl,--subsystem,console
else
    DEBUG_LDFLAGS = $(PLATFORM_LDFLAGS)
endif

# Package dependencies
GTK_CFLAGS = $(shell pkg-config --cflags gtk+-3.0 2>/dev/null)
GTK_LIBS = $(shell pkg-config --libs gtk+-3.0 2>/dev/null)

ifeq ($(IS_WINDOWS),0)
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
ifeq ($(IS_WINDOWS),0)
    MATH_LIBS = -lm
    PTHREAD_LIBS = -lpthread
else
    # Windows/MinGW includes math functions in standard library
    MATH_LIBS = 
    # Windows doesn't need explicit pthread linking with mingw
    PTHREAD_LIBS = 
endif

# Combined flags
ALL_CFLAGS = $(CFLAGS) $(GTK_CFLAGS) $(GL_CFLAGS) $(GLFW_CFLAGS) $(JSON_CFLAGS) -I$(NURBS_SRC_DIR)
DEBUG_ALL_CFLAGS = $(DEBUG_CFLAGS) $(GTK_CFLAGS) $(GL_CFLAGS) $(GLFW_CFLAGS) $(JSON_CFLAGS) -I$(NURBS_SRC_DIR)

# Windows-specific linking strategy
ifeq ($(IS_WINDOWS),1)
    # For Windows, create a filtered library list to avoid duplicates
    # Extract unique libraries from GTK to avoid conflicts
    GTK_SYSTEM_LIBS := $(filter -luser32 -lkernel32 -lgdi32 -lwinspool -lcomdlg32 -ladvapi32 -lshell32 -lole32 -loleaut32 -luuid,$(GTK_LIBS))
    GTK_CLEAN_LIBS := $(filter-out $(GTK_SYSTEM_LIBS),$(GTK_LIBS))
    
    # Use a careful library order for Windows - GTK libs first, then others
    ALL_LIBS = $(GTK_LIBS) $(GL_LIBS) $(GLFW_LIBS) $(JSON_LIBS) $(MATH_LIBS) $(PTHREAD_LIBS)
    DEBUG_ALL_LIBS = $(GTK_LIBS) $(GL_LIBS) $(GLFW_LIBS) $(JSON_LIBS) $(MATH_LIBS) $(PTHREAD_LIBS)
    
    # Windows-specific linker flags for better error reporting and compatibility
    WINDOWS_LINK_FLAGS = -Wl,--enable-stdcall-fixup -Wl,--disable-auto-import
else
    # Standard Unix linking
    ALL_LIBS = $(GTK_LIBS) $(GL_LIBS) $(GLFW_LIBS) $(JSON_LIBS) $(WIN_LIBS) $(MATH_LIBS) $(PTHREAD_LIBS)
    DEBUG_ALL_LIBS = $(GTK_LIBS) $(GL_LIBS) $(GLFW_LIBS) $(JSON_LIBS) $(WIN_LIBS) $(MATH_LIBS) $(PTHREAD_LIBS)
    WINDOWS_LINK_FLAGS = 
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
ifeq ($(IS_WINDOWS),0)
	@echo "Skipping Windows resources on Linux..."
	@touch $(RESOURCE_OBJ)
else
	@echo "Compiling Windows resources..."
	$(WINDRES) -i $(RESOURCE_RC) -o $(RESOURCE_OBJ)
endif

# Release target
$(TARGET): $(ALL_OBJECTS) $(RESOURCE_OBJ) | $(BIN_DIR)
ifeq ($(IS_WINDOWS),1)
	@echo "Linking $(PROJECT_NAME) for Windows using enhanced strategy..."
	@echo "Platform: $(DETECTED_PLATFORM)"
	@echo "Libraries: $(ALL_LIBS)"
	$(CC) $(LDFLAGS) $(WINDOWS_LINK_FLAGS) -o $@ $^ $(ALL_LIBS)
	@echo "Windows build complete: $@"
else
	@echo "Linking $(PROJECT_NAME) for $(DETECTED_PLATFORM)..."
	$(CC) $(LDFLAGS) -o $@ $^ $(ALL_LIBS)
	@echo "Build complete: $@"
endif

# Debug target
$(DEBUG_TARGET): $(DEBUG_ALL_OBJECTS) $(RESOURCE_OBJ) | $(BIN_DIR)
ifeq ($(IS_WINDOWS),1)
	@echo "Linking $(PROJECT_NAME) (debug) for Windows using enhanced strategy..."
	@echo "Platform: $(DETECTED_PLATFORM)"
	@echo "Debug Libraries: $(DEBUG_ALL_LIBS)"
	$(CC) $(DEBUG_LDFLAGS) $(WINDOWS_LINK_FLAGS) -o $@ $^ $(DEBUG_ALL_LIBS)
	@echo "Windows debug build complete: $@"
else
	@echo "Linking $(PROJECT_NAME) (debug) for $(DETECTED_PLATFORM)..."
	$(CC) $(DEBUG_LDFLAGS) -o $@ $^ $(DEBUG_ALL_LIBS)
	@echo "Debug build complete: $@"
endif

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

# Debug platform detection
.PHONY: debug-platform
debug-platform:
	@echo "=== Platform Detection ==="
	@echo "UNAME_S: $(UNAME_S)"
	@echo "OS: $(OS)"
	@echo "BUILD_MODE: $(BUILD_MODE)"
	@echo "IS_WINDOWS: $(IS_WINDOWS)"
	@echo "DETECTED_PLATFORM: $(DETECTED_PLATFORM)"
	@echo "EXE_SUFFIX: $(EXE_SUFFIX)"
	@echo "LINK_STRATEGY: $(LINK_STRATEGY)"
	@echo ""
	@echo "=== Compilation Settings ==="
	@echo "PLATFORM_CFLAGS: $(PLATFORM_CFLAGS)"
	@echo "PLATFORM_LDFLAGS: $(PLATFORM_LDFLAGS)"
	@echo "WINDOWS_LINK_FLAGS: $(WINDOWS_LINK_FLAGS)"
	@echo ""
	@echo "=== Libraries ==="
	@echo "GTK_LIBS: $(GTK_LIBS)"
	@echo "GTK_CLEAN_LIBS: $(GTK_CLEAN_LIBS)"
	@echo "GL_LIBS: $(GL_LIBS)"
	@echo "ALL_LIBS: $(ALL_LIBS)"

# Test Windows linking incrementally
.PHONY: test-link-windows
test-link-windows: $(ALL_OBJECTS) | $(BIN_DIR)
	@echo "Testing Windows linking incrementally..."
	@echo "Step 1: Basic GTK linking..."
	-$(CC) $(LDFLAGS) -o build/bin/test_gtk.exe $^ $(GTK_CLEAN_LIBS)
	@echo "Step 2: Adding OpenGL..."
	-$(CC) $(LDFLAGS) -o build/bin/test_gl.exe $^ $(GTK_CLEAN_LIBS) $(GL_LIBS)
	@echo "Step 3: Adding GLFW..."
	-$(CC) $(LDFLAGS) -o build/bin/test_glfw.exe $^ $(GTK_CLEAN_LIBS) $(GL_LIBS) $(GLFW_LIBS)
	@echo "Step 4: Full linking..."
	-$(CC) $(LDFLAGS) $(WINDOWS_LINK_FLAGS) -o build/bin/test_full.exe $^ $(ALL_LIBS)
	@echo "Incremental linking test complete"

# Test minimal linking
.PHONY: test-link
test-link: $(ALL_OBJECTS) | $(BIN_DIR)
	@echo "Testing minimal linking..."
	$(CC) $(LDFLAGS) -o build/bin/test_link$(EXE_SUFFIX) $^ -lkernel32 -luser32
	@echo "Minimal linking test passed"

# Force Windows build mode
.PHONY: windows
windows:
	@echo "Building for Windows (forced mode)..."
	$(MAKE) BUILD_MODE=windows release

# Force Linux build mode  
.PHONY: linux
linux:
	@echo "Building for Linux (forced mode)..."
	$(MAKE) BUILD_MODE=linux release

# Help target
.PHONY: help
help:
	@echo "NURBS Map Editor - Enhanced Cross-Platform Build System"
	@echo "======================================================"
	@echo ""
	@echo "Current Platform: $(DETECTED_PLATFORM)"
	@echo "Build Mode: $(BUILD_MODE)"
	@echo ""
	@echo "Build Targets:"
	@echo "  all                 - Build release version (default)"
	@echo "  release             - Build optimized release version"
	@echo "  debug               - Build debug version with symbols"
	@echo "  windows             - Force Windows build (make BUILD_MODE=windows)"
	@echo "  linux               - Force Linux build (make BUILD_MODE=linux)"
	@echo "  clean               - Remove all build files"
	@echo ""
	@echo "Debug & Testing:"
	@echo "  debug-platform      - Show platform detection info"
	@echo "  test-link           - Test minimal linking"
	@echo "  test-link-windows   - Test Windows linking incrementally"
	@echo "  check-deps          - Check for required dependencies"
	@echo ""
	@echo "Runtime:"
	@echo "  run                 - Run release version"
	@echo "  run-debug           - Run debug version"
	@echo ""
	@echo "Packaging & Analysis:"
	@echo "  package             - Create distribution package"
	@echo "  install-deps        - Install dependencies"
	@echo "  analyze             - Run static analysis"
	@echo "  help                - Show this help"
	@echo ""
	@echo "Build Modes:"
	@echo "  make                - Auto-detect platform"
	@echo "  make BUILD_MODE=windows - Force Windows/MSYS2 build"
	@echo "  make BUILD_MODE=linux   - Force Linux/Unix build"
	@echo ""
	@echo "Project: $(PROJECT_NAME) v$(VERSION)"

# Dependencies
-include $(ALL_OBJECTS:.o=.d)
-include $(DEBUG_ALL_OBJECTS:.o=.d)
