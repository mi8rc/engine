cmake_minimum_required(VERSION 3.16)
project(NurbsMapEditor C)

set(CMAKE_C_STANDARD 11)
set(CMAKE_C_STANDARD_REQUIRED ON)

# Find required packages
find_package(PkgConfig REQUIRED)

# GTK3
pkg_check_modules(GTK3 REQUIRED gtk+-3.0)

# OpenGL
find_package(OpenGL REQUIRED)

# GLEW for OpenGL extensions
if(WIN32 AND MINGW)
    # On Windows with MSYS2/MinGW, use pkg-config for GLEW
    pkg_check_modules(GLEW REQUIRED glew)
    set(GLEW_LIBRARIES ${GLEW_LIBRARIES})
else()
    # On Linux or other systems, use find_package
    find_package(GLEW REQUIRED)
    set(GLEW_LIBRARIES GLEW::GLEW)
endif()

# Include directories
include_directories(${GTK3_INCLUDE_DIRS})
include_directories(${GLEW_INCLUDE_DIRS})
include_directories(src)

# Link directories
link_directories(${GTK3_LIBRARY_DIRS})
link_directories(${GLEW_LIBRARY_DIRS})

# Compiler flags
add_definitions(${GTK3_CFLAGS_OTHER})
add_definitions(${GLEW_CFLAGS_OTHER})

# Source files
set(SOURCES
    src/main.c
    src/window.c
    src/viewport.c
    src/nurbs.c
    src/ui_builder.c
    src/property_panel.c
    src/hierarchy_panel.c
    src/file_io.c
    src/renderer.c
    src/tools.c
)

# Header files
set(HEADERS
    src/main.h
    src/window.h
    src/viewport.h
    src/nurbs.h
    src/ui_builder.h
    src/property_panel.h
    src/hierarchy_panel.h
    src/file_io.h
    src/renderer.h
    src/tools.h
)

# Create executable
add_executable(${PROJECT_NAME} ${SOURCES} ${HEADERS})

# Link libraries
target_link_libraries(${PROJECT_NAME}
    ${GTK3_LIBRARIES}
    ${OPENGL_LIBRARIES}
    ${GLEW_LIBRARIES}
    m
)

# Windows-specific configuration
if(WIN32)
    # Hide console window for release builds
    if(CMAKE_BUILD_TYPE STREQUAL "Release")
        set_target_properties(${PROJECT_NAME} PROPERTIES
            WIN32_EXECUTABLE TRUE
        )
    endif()
    
    # Copy GTK3 and other DLLs for distribution
    if(MINGW)
        # Add custom target to copy required DLLs
        add_custom_command(TARGET ${PROJECT_NAME} POST_BUILD
            COMMAND ${CMAKE_COMMAND} -E echo "Built with MSYS2/MinGW64"
        )
    endif()
endif()

# Install target
install(TARGETS ${PROJECT_NAME} DESTINATION bin)

# Install resources
install(DIRECTORY resources/ DESTINATION share/${PROJECT_NAME}/resources)

# CPack configuration for packaging
set(CPACK_PACKAGE_NAME "NurbsMapEditor")
set(CPACK_PACKAGE_VERSION "1.0.0")
set(CPACK_PACKAGE_DESCRIPTION_SUMMARY "Professional 3D NURBS-based map editor with IGES support")
set(CPACK_PACKAGE_VENDOR "NURBS Map Editor Team")

if(WIN32)
    set(CPACK_GENERATOR "ZIP")
else()
    set(CPACK_GENERATOR "TGZ")
endif()

include(CPack)