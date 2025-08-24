CC = gcc
CFLAGS = -Wall -Wextra -std=c99 -O2 -g
INCLUDES = -Isrc/

# Detect platform and set appropriate libraries
ifeq ($(OS),Windows_NT)
    # Windows/MSYS2 libraries
    LIBS = -lglfw3 -lopengl32 -lglu32 -lm -ljson-c
    # Add Windows-specific libraries
    LIBS += -lgdi32 -luser32 -lkernel32
else
    UNAME_S := $(shell uname -s)
    ifeq ($(UNAME_S),Linux)
        # Linux libraries
        LIBS = -lglfw -lGL -lGLU -lm -ljson-c
    endif
    ifeq ($(UNAME_S),Darwin)
        # macOS libraries
        LIBS = -lglfw -framework OpenGL -lm -ljson-c
    endif
endif

# Directories
SRCDIR = src
OBJDIR = obj
BINDIR = bin

# Source files
SOURCES = $(wildcard $(SRCDIR)/*.c)
OBJECTS = $(SOURCES:$(SRCDIR)/%.c=$(OBJDIR)/%.o)
TARGET = $(BINDIR)/nurbs_fps_game

# Default target
all: $(TARGET)

# Create directories if they don't exist
$(OBJDIR):
	mkdir -p $(OBJDIR)

$(BINDIR):
	mkdir -p $(BINDIR)

# Compile object files
$(OBJDIR)/%.o: $(SRCDIR)/%.c | $(OBJDIR)
	$(CC) $(CFLAGS) $(INCLUDES) -c $< -o $@

# Link executable
$(TARGET): $(OBJECTS) | $(BINDIR)
	$(CC) $(OBJECTS) $(LIBS) -o $@

# Install dependencies (Ubuntu/Debian)
install-deps:
	sudo apt-get update
	sudo apt-get install -y libglfw3-dev libgl1-mesa-dev libglu1-mesa-dev build-essential libjson-c-dev

# Install dependencies (Fedora/RHEL)
install-deps-fedora:
	sudo dnf install -y glfw-devel mesa-libGL-devel mesa-libGLU-devel gcc make

# Install dependencies (Arch Linux)
install-deps-arch:
	sudo pacman -S glfw-x11 mesa glu gcc make

# Clean build files
clean:
	rm -rf $(OBJDIR) $(BINDIR)

# Run the game
run: $(TARGET)
	./$(TARGET)

# Run with sample map
run-sample: $(TARGET)
	./$(TARGET) sample_maps/demo_scene.map

# Run the map editor
run-editor:
	python3 run_editor.py

# Setup Python editor dependencies
setup-editor:
	cd map_editor && pip install -r requirements.txt

# Debug build
debug: CFLAGS += -DDEBUG -g3
debug: $(TARGET)

# Release build
release: CFLAGS += -DNDEBUG -O3
release: $(TARGET)

.PHONY: all clean run debug release install-deps install-deps-fedora install-deps-arch