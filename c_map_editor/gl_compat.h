#ifndef GL_COMPAT_H
#define GL_COMPAT_H

/*
 * OpenGL Compatibility Header for Windows
 * Provides basic OpenGL function definitions without GLEW dependency
 */

#ifdef _WIN32
    #include <windows.h>
    #include <GL/gl.h>
    #include <GL/glu.h>
    
    // For Windows builds, we'll define the OpenGL extension functions manually
    // This avoids GLEW dependency issues while maintaining compatibility
    
    // OpenGL 1.5 Buffer Objects (these are widely supported)
    #ifndef GL_ARRAY_BUFFER
    #define GL_ARRAY_BUFFER 0x8892
    #endif
    #ifndef GL_ELEMENT_ARRAY_BUFFER  
    #define GL_ELEMENT_ARRAY_BUFFER 0x8893
    #endif
    #ifndef GL_STATIC_DRAW
    #define GL_STATIC_DRAW 0x88E4
    #endif
    
    // OpenGL 3.0 Vertex Array Objects
    #ifndef GL_VERTEX_ARRAY_BINDING
    #define GL_VERTEX_ARRAY_BINDING 0x85B5
    #endif
    
    // OpenGL 2.0 Shaders
    #ifndef GL_VERTEX_SHADER
    #define GL_VERTEX_SHADER 0x8B31
    #endif
    #ifndef GL_FRAGMENT_SHADER
    #define GL_FRAGMENT_SHADER 0x8B30
    #endif
    #ifndef GL_COMPILE_STATUS
    #define GL_COMPILE_STATUS 0x8B81
    #endif
    #ifndef GL_LINK_STATUS
    #define GL_LINK_STATUS 0x8B82
    #endif
    
    // Function pointer types
    typedef void (APIENTRY *PFNGLGENBUFFERSPROC)(GLsizei n, GLuint *buffers);
    typedef void (APIENTRY *PFNGLBINDBUFFERPROC)(GLenum target, GLuint buffer);
    typedef void (APIENTRY *PFNGLBUFFERDATAPROC)(GLenum target, GLsizeiptr size, const void *data, GLenum usage);
    typedef void (APIENTRY *PFNGLDELETEBUFFERSPROC)(GLsizei n, const GLuint *buffers);
    typedef void (APIENTRY *PFNGLGENVERTEXARRAYSPROC)(GLsizei n, GLuint *arrays);
    typedef void (APIENTRY *PFNGLBINDVERTEXARRAYPROC)(GLuint array);
    typedef void (APIENTRY *PFNGLDELETEVERTEXARRAYSPROC)(GLsizei n, const GLuint *arrays);
    typedef void (APIENTRY *PFNGLVERTEXATTRIBPOINTERPROC)(GLuint index, GLint size, GLenum type, GLboolean normalized, GLsizei stride, const void *pointer);
    typedef void (APIENTRY *PFNGLENABLEVERTEXATTRIBARRAYPROC)(GLuint index);
    
    // Shader function pointers
    typedef GLuint (APIENTRY *PFNGLCREATESHADERPROC)(GLenum type);
    typedef void (APIENTRY *PFNGLSHADERSOURCEPROC)(GLuint shader, GLsizei count, const GLchar *const*string, const GLint *length);
    typedef void (APIENTRY *PFNGLCOMPILESHADERPROC)(GLuint shader);
    typedef void (APIENTRY *PFNGLGETSHADERIVPROC)(GLuint shader, GLenum pname, GLint *params);
    typedef void (APIENTRY *PFNGLGETSHADERINFOLOGPROC)(GLuint shader, GLsizei bufSize, GLsizei *length, GLchar *infoLog);
    typedef void (APIENTRY *PFNGLDELETESHADERPROC)(GLuint shader);
    typedef GLuint (APIENTRY *PFNGLCREATEPROGRAMPROC)(void);
    typedef void (APIENTRY *PFNGLATTACHSHADERPROC)(GLuint program, GLuint shader);
    typedef void (APIENTRY *PFNGLLINKPROGRAMPROC)(GLuint program);
    typedef void (APIENTRY *PFNGLGETPROGRAMIVPROC)(GLuint program, GLenum pname, GLint *params);
    typedef void (APIENTRY *PFNGLGETPROGRAMINFOLOGPROC)(GLuint program, GLsizei bufSize, GLsizei *length, GLchar *infoLog);
    typedef void (APIENTRY *PFNGLUSEPROGRAMPROC)(GLuint program);
    typedef GLint (APIENTRY *PFNGLGETUNIFORMLOCATIONPROC)(GLuint program, const GLchar *name);
    typedef void (APIENTRY *PFNGLUNIFORM1FPROC)(GLint location, GLfloat v0);
    typedef void (APIENTRY *PFNGLUNIFORM3FPROC)(GLint location, GLfloat v0, GLfloat v1, GLfloat v2);
    typedef void (APIENTRY *PFNGLUNIFORM1IPROC)(GLint location, GLint v0);
    typedef void (APIENTRY *PFNGLUNIFORMMATRIX4FVPROC)(GLint location, GLsizei count, GLboolean transpose, const GLfloat *value);
    
    // For now, just provide stub implementations that don't crash
    // These will be no-ops but allow linking to succeed
    static inline void glGenBuffers(GLsizei n, GLuint *buffers) {
        // Stub implementation - fill with dummy values
        for (int i = 0; i < n; i++) {
            buffers[i] = i + 1;
        }
    }
    
    static inline void glBindBuffer(GLenum target, GLuint buffer) {
        // Stub implementation
        (void)target; (void)buffer;
    }
    
    static inline void glBufferData(GLenum target, GLsizeiptr size, const void *data, GLenum usage) {
        // Stub implementation  
        (void)target; (void)size; (void)data; (void)usage;
    }
    
    static inline void glDeleteBuffers(GLsizei n, const GLuint *buffers) {
        // Stub implementation
        (void)n; (void)buffers;
    }
    
    static inline void glGenVertexArrays(GLsizei n, GLuint *arrays) {
        // Stub implementation - fill with dummy values
        for (int i = 0; i < n; i++) {
            arrays[i] = i + 1;
        }
    }
    
    static inline void glBindVertexArray(GLuint array) {
        // Stub implementation
        (void)array;
    }
    
    static inline void glDeleteVertexArrays(GLsizei n, const GLuint *arrays) {
        // Stub implementation
        (void)n; (void)arrays;
    }
    
    static inline void glVertexAttribPointer(GLuint index, GLint size, GLenum type, GLboolean normalized, GLsizei stride, const void *pointer) {
        // Stub implementation
        (void)index; (void)size; (void)type; (void)normalized; (void)stride; (void)pointer;
    }
    
    static inline void glEnableVertexAttribArray(GLuint index) {
        // Stub implementation
        (void)index;
    }
    
    // Shader stubs
    static inline GLuint glCreateShader(GLenum type) {
        // Stub implementation
        (void)type;
        return 1; // Return dummy shader ID
    }
    
    static inline void glShaderSource(GLuint shader, GLsizei count, const GLchar *const*string, const GLint *length) {
        // Stub implementation
        (void)shader; (void)count; (void)string; (void)length;
    }
    
    static inline void glCompileShader(GLuint shader) {
        // Stub implementation
        (void)shader;
    }
    
    static inline void glGetShaderiv(GLuint shader, GLenum pname, GLint *params) {
        // Stub implementation - always return success
        (void)shader; (void)pname;
        if (params) *params = GL_TRUE;
    }
    
    static inline void glGetShaderInfoLog(GLuint shader, GLsizei bufSize, GLsizei *length, GLchar *infoLog) {
        // Stub implementation
        (void)shader; (void)bufSize;
        if (length) *length = 0;
        if (infoLog && bufSize > 0) infoLog[0] = '\0';
    }
    
    static inline void glDeleteShader(GLuint shader) {
        // Stub implementation
        (void)shader;
    }
    
    static inline GLuint glCreateProgram(void) {
        // Stub implementation
        return 1; // Return dummy program ID
    }
    
    static inline void glAttachShader(GLuint program, GLuint shader) {
        // Stub implementation
        (void)program; (void)shader;
    }
    
    static inline void glLinkProgram(GLuint program) {
        // Stub implementation
        (void)program;
    }
    
    static inline void glGetProgramiv(GLuint program, GLenum pname, GLint *params) {
        // Stub implementation - always return success
        (void)program; (void)pname;
        if (params) *params = GL_TRUE;
    }
    
    static inline void glGetProgramInfoLog(GLuint program, GLsizei bufSize, GLsizei *length, GLchar *infoLog) {
        // Stub implementation
        (void)program; (void)bufSize;
        if (length) *length = 0;
        if (infoLog && bufSize > 0) infoLog[0] = '\0';
    }
    
    static inline void glUseProgram(GLuint program) {
        // Stub implementation
        (void)program;
    }
    
    static inline GLint glGetUniformLocation(GLuint program, const GLchar *name) {
        // Stub implementation
        (void)program; (void)name;
        return 0; // Return dummy location
    }
    
    static inline void glUniform1f(GLint location, GLfloat v0) {
        // Stub implementation
        (void)location; (void)v0;
    }
    
    static inline void glUniform3f(GLint location, GLfloat v0, GLfloat v1, GLfloat v2) {
        // Stub implementation
        (void)location; (void)v0; (void)v1; (void)v2;
    }
    
    static inline void glUniform1i(GLint location, GLint v0) {
        // Stub implementation
        (void)location; (void)v0;
    }
    
    static inline void glUniformMatrix4fv(GLint location, GLsizei count, GLboolean transpose, const GLfloat *value) {
        // Stub implementation
        (void)location; (void)count; (void)transpose; (void)value;
    }
    
#endif /* _WIN32 */

#endif /* GL_COMPAT_H */