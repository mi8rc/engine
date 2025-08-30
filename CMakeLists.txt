cmake_minimum_required(VERSION 3.15)
project(NURBSMapEditor VERSION 1.0.0 LANGUAGES C)

set(CMAKE_C_STANDARD 11)
set(CMAKE_C_STANDARD_REQUIRED ON)

# Find required packages
find_package(PkgConfig REQUIRED)

# Try to find OpenGL
find_package(OpenGL)
if(NOT OpenGL_FOUND)
    # Fallback for systems without proper OpenGL packages
    set(OPENGL_LIBRARIES GL GLU)
endif()

# Try to find GLFW
find_package(glfw3)
if(NOT glfw3_FOUND)
    # Try pkg-config as fallback
    pkg_check_modules(GLFW3 glfw3)
    if(GLFW3_FOUND)
        set(GLFW_LIBRARIES ${GLFW3_LIBRARIES})
        include_directories(${GLFW3_INCLUDE_DIRS})
    else()
        # Last resort - assume system has glfw3
        set(GLFW_LIBRARIES glfw)
    endif()
else()
    set(GLFW_LIBRARIES glfw)
endif()

# Include directories
include_directories(${CMAKE_SOURCE_DIR}/src)

# Source files - using existing C implementation
set(SOURCES
    src/main.c
    src/nurbs.c
    src/fps_engine.c
    src/map_loader.c
    src/math_utils.c
)

# Create executable
add_executable(${PROJECT_NAME} ${SOURCES})

# Link libraries
if(OpenGL_FOUND)
    target_link_libraries(${PROJECT_NAME} OpenGL::GL OpenGL::GLU)
else()
    target_link_libraries(${PROJECT_NAME} ${OPENGL_LIBRARIES})
endif()

target_link_libraries(${PROJECT_NAME} 
    ${GLFW_LIBRARIES}
    m
)

# Compiler flags for better compatibility
if(WIN32)
    target_compile_definitions(${PROJECT_NAME} PRIVATE _WIN32)
endif()

# Set output directory
set_target_properties(${PROJECT_NAME} PROPERTIES
    RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}
)