#ifndef WIN_COMPAT_H
#define WIN_COMPAT_H

/*
 * Windows Compatibility Header
 * Minimal fixes for specific cross-compilation issues
 * DO NOT include this in files that use GTK!
 */

// Only apply these fixes when specifically needed for Windows builds
#if defined(_WIN32) || defined(__MINGW32__) || defined(__MINGW64__)
    
    // Math constants
    #ifndef M_PI
    #define M_PI 3.14159265358979323846
    #endif
    
    // Only redefine ctype functions if we detect cross-compilation issues
    // This is a targeted fix for the iges_loader.c __ctype_b_loc issue
    #ifdef NEED_CTYPE_FIX
        // Safe ctype implementations for cross-compilation
        #ifdef isspace
        #undef isspace
        #endif
        #define isspace(c) ((c) == ' ' || (c) == '\t' || (c) == '\n' || (c) == '\r' || (c) == '\f' || (c) == '\v')
    #endif
    
#else
    // Non-Windows systems
    #ifndef M_PI
    #define M_PI 3.14159265358979323846
    #endif
#endif

#endif /* WIN_COMPAT_H */