# NURBS Map Editor Makefile
# Supports both Linux and Windows (MSYS2)

# Project settings
PROJECT_NAME = NurbsMapEditor
VERSION = 1.0.0

# Directories
SRCDIR = src
BUILDDIR = build
OBJDIR = $(BUILDDIR)/obj
BINDIR = $(BUILDDIR)/bin
INSTALLDIR = /usr/local

# Source files
SOURCES = $(wildcard $(SRCDIR)/*.c)
OBJECTS = $(SOURCES:$(SRCDIR)/%.c=$(OBJDIR)/%.o)
HEADERS = $(wildcard $(SRCDIR)/*.h)

# Detect platform
ifeq ($(OS),Windows_NT)
    PLATFORM = windows
    EXE_EXT = .exe
    LIB_EXT = .dll
else
    UNAME_S := $(shell uname -s)
    ifeq ($(UNAME_S),Linux)
        PLATFORM = linux
        EXE_EXT = 
        LIB_EXT = .so
    endif
endif

# Compiler settings
CC = gcc
CFLAGS = -std=c11 -Wall -Wextra -Iinclude -I$(SRCDIR)

# Debug/Release flags
ifeq ($(DEBUG),1)
    CFLAGS += -g -O0 -DDEBUG
    BUILD_TYPE = debug
else
    CFLAGS += -O3 -DNDEBUG
    BUILD_TYPE = release
endif

# Platform-specific settings
ifeq ($(PLATFORM),windows)
    # Windows (MSYS2) settings
    PKG_CONFIG = pkg-config
    GTK_CFLAGS = $(shell $(PKG_CONFIG) --cflags gtk+-3.0)
    GTK_LIBS = $(shell $(PKG_CONFIG) --libs gtk+-3.0)
    GLEW_CFLAGS = $(shell $(PKG_CONFIG) --cflags glew)
    GLEW_LIBS = $(shell $(PKG_CONFIG) --libs glew)
    OPENGL_LIBS = -lopengl32 -lglu32
    MATH_LIBS = -lm
    
    # Windows-specific flags
    CFLAGS += -DWIN32 -D_WIN32_WINNT=0x0601
    
    # Hide console in release mode
    ifneq ($(DEBUG),1)
        LDFLAGS += -mwindows
    endif
else
    # Linux settings
    PKG_CONFIG = pkg-config
    GTK_CFLAGS = $(shell $(PKG_CONFIG) --cflags gtk+-3.0)
    GTK_LIBS = $(shell $(PKG_CONFIG) --libs gtk+-3.0)
    GLEW_CFLAGS = $(shell $(PKG_CONFIG) --cflags glew)
    GLEW_LIBS = $(shell $(PKG_CONFIG) --libs glew)
    OPENGL_LIBS = -lGL -lGLU
    MATH_LIBS = -lm
    
    # Linux-specific flags
    CFLAGS += -DLINUX
endif

# Combine all flags
ALL_CFLAGS = $(CFLAGS) $(GTK_CFLAGS) $(GLEW_CFLAGS)
ALL_LIBS = $(GTK_LIBS) $(GLEW_LIBS) $(OPENGL_LIBS) $(MATH_LIBS)

# Target executable
TARGET = $(BINDIR)/$(PROJECT_NAME)$(EXE_EXT)

# Default target
.PHONY: all
all: $(TARGET)

# Create directories
$(OBJDIR):
	@mkdir -p $(OBJDIR)

$(BINDIR):
	@mkdir -p $(BINDIR)

# Compile object files
$(OBJDIR)/%.o: $(SRCDIR)/%.c $(HEADERS) | $(OBJDIR)
	@echo "Compiling $<..."
	@$(CC) $(ALL_CFLAGS) -c $< -o $@

# Link executable
$(TARGET): $(OBJECTS) | $(BINDIR)
	@echo "Linking $(TARGET)..."
	@$(CC) $(OBJECTS) -o $@ $(ALL_LIBS) $(LDFLAGS)
	@echo "Build completed: $(TARGET)"

# Debug build
.PHONY: debug
debug:
	@$(MAKE) DEBUG=1

# Release build
.PHONY: release
release:
	@$(MAKE) DEBUG=0

# Clean build files
.PHONY: clean
clean:
	@echo "Cleaning build files..."
	@rm -rf $(BUILDDIR)
	@echo "Clean completed."

# Install (Linux only)
.PHONY: install
install: $(TARGET)
ifeq ($(PLATFORM),linux)
	@echo "Installing to $(INSTALLDIR)..."
	@install -d $(INSTALLDIR)/bin
	@install -m 755 $(TARGET) $(INSTALLDIR)/bin/
	@install -d $(INSTALLDIR)/share/$(PROJECT_NAME)
	@cp -r resources $(INSTALLDIR)/share/$(PROJECT_NAME)/ 2>/dev/null || true
	@echo "Installation completed."
else
	@echo "Install target not supported on Windows. Use 'make package' instead."
endif

# Uninstall (Linux only)
.PHONY: uninstall
uninstall:
ifeq ($(PLATFORM),linux)
	@echo "Uninstalling from $(INSTALLDIR)..."
	@rm -f $(INSTALLDIR)/bin/$(PROJECT_NAME)
	@rm -rf $(INSTALLDIR)/share/$(PROJECT_NAME)
	@echo "Uninstall completed."
else
	@echo "Uninstall target not supported on Windows."
endif

# Package for distribution
.PHONY: package
package: release
	@echo "Creating package..."
	@mkdir -p $(BUILDDIR)/package/$(PROJECT_NAME)-$(VERSION)
	@cp $(TARGET) $(BUILDDIR)/package/$(PROJECT_NAME)-$(VERSION)/
	@cp -r resources $(BUILDDIR)/package/$(PROJECT_NAME)-$(VERSION)/ 2>/dev/null || true
	@cp README.md $(BUILDDIR)/package/$(PROJECT_NAME)-$(VERSION)/ 2>/dev/null || true
ifeq ($(PLATFORM),windows)
	@cd $(BUILDDIR)/package && zip -r $(PROJECT_NAME)-$(VERSION)-windows-x64.zip $(PROJECT_NAME)-$(VERSION)
	@echo "Windows package: $(BUILDDIR)/package/$(PROJECT_NAME)-$(VERSION)-windows-x64.zip"
else
	@cd $(BUILDDIR)/package && tar -czf $(PROJECT_NAME)-$(VERSION)-linux-x64.tar.gz $(PROJECT_NAME)-$(VERSION)
	@echo "Linux package: $(BUILDDIR)/package/$(PROJECT_NAME)-$(VERSION)-linux-x64.tar.gz"
endif

# Run the application
.PHONY: run
run: $(TARGET)
	@echo "Running $(PROJECT_NAME)..."
	@cd $(BINDIR) && ./$(PROJECT_NAME)$(EXE_EXT)

# Check dependencies
.PHONY: deps
deps:
	@echo "Checking dependencies..."
ifeq ($(PLATFORM),windows)
	@echo "Windows (MSYS2) dependencies:"
	@echo "  Required packages:"
	@echo "    mingw-w64-x86_64-gcc"
	@echo "    mingw-w64-x86_64-pkg-config"
	@echo "    mingw-w64-x86_64-gtk3"
	@echo "    mingw-w64-x86_64-glew"
	@echo ""
	@echo "  Install with: pacman -S mingw-w64-x86_64-gcc mingw-w64-x86_64-pkg-config mingw-w64-x86_64-gtk3 mingw-w64-x86_64-glew"
else
	@echo "Linux dependencies:"
	@echo "  Required packages:"
	@echo "    build-essential"
	@echo "    pkg-config"
	@echo "    libgtk-3-dev"
	@echo "    libglew-dev"
	@echo "    libgl1-mesa-dev"
	@echo ""
	@echo "  Install with: sudo apt-get install build-essential pkg-config libgtk-3-dev libglew-dev libgl1-mesa-dev"
endif

# Show build info
.PHONY: info
info:
	@echo "Build Information:"
	@echo "  Project: $(PROJECT_NAME) v$(VERSION)"
	@echo "  Platform: $(PLATFORM)"
	@echo "  Compiler: $(CC)"
	@echo "  Build Type: $(BUILD_TYPE)"
	@echo "  Source Files: $(words $(SOURCES))"
	@echo "  Target: $(TARGET)"
	@echo ""
	@echo "Available targets:"
	@echo "  all      - Build the project (default)"
	@echo "  debug    - Build with debug symbols"
	@echo "  release  - Build optimized release"
	@echo "  clean    - Remove build files"
	@echo "  install  - Install to system (Linux only)"
	@echo "  package  - Create distribution package"
	@echo "  run      - Build and run the application"
	@echo "  deps     - Show dependency information"
	@echo "  info     - Show this information"

# Help target
.PHONY: help
help: info