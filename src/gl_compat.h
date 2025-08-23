#ifndef GL_COMPAT_H
#define GL_COMPAT_H

#include <GL/gl.h>
#include <GL/glu.h>
#include <GLFW/glfw3.h>
#include <stdio.h>

// OpenGL constants we need
#ifndef GL_ARRAY_BUFFER
#define GL_ARRAY_BUFFER 0x8892
#define GL_ELEMENT_ARRAY_BUFFER 0x8893
#define GL_STATIC_DRAW 0x88E4
#define GL_VERTEX_SHADER 0x8B31
#define GL_FRAGMENT_SHADER 0x8B30
#define GL_COMPILE_STATUS 0x8B81
#define GL_LINK_STATUS 0x8B82
#endif

// Modern OpenGL availability flag
extern int gl_modern_available;

// Function pointers for modern OpenGL functions
extern void (*gl_GenVertexArrays)(GLsizei n, GLuint *arrays);
extern void (*gl_BindVertexArray)(GLuint array);
extern void (*gl_DeleteVertexArrays)(GLsizei n, const GLuint *arrays);
extern void (*gl_GenBuffers)(GLsizei n, GLuint *buffers);
extern void (*gl_BindBuffer)(GLenum target, GLuint buffer);
extern void (*gl_BufferData)(GLenum target, GLsizeiptr size, const void *data, GLenum usage);
extern void (*gl_DeleteBuffers)(GLsizei n, const GLuint *buffers);
extern void (*gl_VertexAttribPointer)(GLuint index, GLint size, GLenum type, GLboolean normalized, GLsizei stride, const void *pointer);
extern void (*gl_EnableVertexAttribArray)(GLuint index);
extern GLuint (*gl_CreateShader)(GLenum type);
extern void (*gl_ShaderSource)(GLuint shader, GLsizei count, const GLchar *const*string, const GLint *length);
extern void (*gl_CompileShader)(GLuint shader);
extern void (*gl_GetShaderiv)(GLuint shader, GLenum pname, GLint *params);
extern void (*gl_GetShaderInfoLog)(GLuint shader, GLsizei bufSize, GLsizei *length, GLchar *infoLog);
extern GLuint (*gl_CreateProgram)(void);
extern void (*gl_AttachShader)(GLuint program, GLuint shader);
extern void (*gl_LinkProgram)(GLuint program);
extern void (*gl_GetProgramiv)(GLuint program, GLenum pname, GLint *params);
extern void (*gl_GetProgramInfoLog)(GLuint program, GLsizei bufSize, GLsizei *length, GLchar *infoLog);
extern void (*gl_DeleteShader)(GLuint shader);
extern void (*gl_UseProgram)(GLuint program);
extern GLint (*gl_GetUniformLocation)(GLuint program, const GLchar *name);
extern void (*gl_UniformMatrix4fv)(GLint location, GLsizei count, GLboolean transpose, const GLfloat *value);
extern void (*gl_Uniform3f)(GLint location, GLfloat v0, GLfloat v1, GLfloat v2);
extern void (*gl_Uniform1f)(GLint location, GLfloat v0);
extern void (*gl_Uniform1i)(GLint location, GLint v0);

// Initialize OpenGL compatibility layer
int gl_compat_init(void);

// Fallback rendering functions for legacy OpenGL
void gl_compat_render_nurbs_surface(float *vertices, int vertex_count, unsigned int *indices, int index_count);
void gl_compat_set_material(float ambient[3], float diffuse[3], float specular[3], float shininess);
void gl_compat_set_light(int light_id, float position[3], float color[3], float intensity);

#endif // GL_COMPAT_H