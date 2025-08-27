#ifndef WIN_COMPAT_H
#define WIN_COMPAT_H

/*
 * Windows Compatibility Header
 * Provides cross-platform compatibility fixes for Windows builds
 */

#if defined(_WIN32) || defined(__MINGW32__) || defined(__MINGW64__)
    // Include Windows headers first to avoid conflicts
    #ifndef WIN32_LEAN_AND_MEAN
    #define WIN32_LEAN_AND_MEAN
    #endif
    #include <windows.h>
    
    // Disable glibc fortify functions that don't exist on Windows
    #ifdef _FORTIFY_SOURCE
    #undef _FORTIFY_SOURCE
    #endif
    #define _FORTIFY_SOURCE 0
    
    // Force use of MinGW ANSI stdio to avoid glibc conflicts
    #ifndef __USE_MINGW_ANSI_STDIO
    #define __USE_MINGW_ANSI_STDIO 1
    #endif
    
    // GLEW static linking
    #ifndef GLEW_STATIC
    #define GLEW_STATIC
    #endif
    
    // Include GLEW before OpenGL to avoid conflicts
    #include <GL/glew.h>
    
    // Fix for standard C library functions - include after Windows headers
    #include <stdio.h>
    #include <stdlib.h>
    #include <string.h>
    #include <ctype.h>
    
    // Override problematic glibc functions with safe MinGW versions
    #ifdef isspace
    #undef isspace
    #endif
    #ifdef isdigit
    #undef isdigit  
    #endif
    #ifdef isalpha
    #undef isalpha
    #endif
    #ifdef isalnum
    #undef isalnum
    #endif
    
    // Cross-platform ctype functions - safe for cross-compilation
    #define isspace(c) ((c) == ' ' || (c) == '\t' || (c) == '\n' || (c) == '\r' || (c) == '\f' || (c) == '\v')
    #define isdigit(c) ((c) >= '0' && (c) <= '9')
    #define isalpha(c) (((c) >= 'a' && (c) <= 'z') || ((c) >= 'A' && (c) <= 'Z'))
    #define isalnum(c) (isalpha(c) || isdigit(c))
    
    // Math constants
    #ifndef M_PI
    #define M_PI 3.14159265358979323846
    #endif
    
#else
    // Linux/Unix systems
    #include <ctype.h>
    #include <math.h>
    
    #ifndef M_PI
    #define M_PI 3.14159265358979323846
    #endif
#endif

#endif /* WIN_COMPAT_H */