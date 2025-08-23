#include "fps_engine.h"
#include <string.h>

// Matrix operations (4x4 matrices in column-major order)
void matrix_identity(float *matrix) {
    memset(matrix, 0, 16 * sizeof(float));
    matrix[0] = matrix[5] = matrix[10] = matrix[15] = 1.0f;
}

void matrix_perspective(float *matrix, float fov, float aspect, float near, float far) {
    memset(matrix, 0, 16 * sizeof(float));
    
    float tan_half_fov = tanf(fov * M_PI / 360.0f);
    
    matrix[0] = 1.0f / (aspect * tan_half_fov);
    matrix[5] = 1.0f / tan_half_fov;
    matrix[10] = -(far + near) / (far - near);
    matrix[11] = -1.0f;
    matrix[14] = -(2.0f * far * near) / (far - near);
}

void matrix_look_at(float *matrix, Vector3 eye, Vector3 center, Vector3 up) {
    Vector3 f = vector3_normalize(vector3_subtract(center, eye));
    Vector3 s = vector3_normalize(vector3_cross(f, up));
    Vector3 u = vector3_cross(s, f);
    
    matrix_identity(matrix);
    
    matrix[0] = s.x;
    matrix[4] = s.y;
    matrix[8] = s.z;
    matrix[1] = u.x;
    matrix[5] = u.y;
    matrix[9] = u.z;
    matrix[2] = -f.x;
    matrix[6] = -f.y;
    matrix[10] = -f.z;
    matrix[12] = -vector3_dot(s, eye);
    matrix[13] = -vector3_dot(u, eye);
    matrix[14] = vector3_dot(f, eye);
}

void matrix_translate(float *matrix, Vector3 translation) {
    matrix[12] += matrix[0] * translation.x + matrix[4] * translation.y + matrix[8] * translation.z;
    matrix[13] += matrix[1] * translation.x + matrix[5] * translation.y + matrix[9] * translation.z;
    matrix[14] += matrix[2] * translation.x + matrix[6] * translation.y + matrix[10] * translation.z;
    matrix[15] += matrix[3] * translation.x + matrix[7] * translation.y + matrix[11] * translation.z;
}

void matrix_rotate(float *matrix, float angle, Vector3 axis) {
    float cos_a = cosf(angle);
    float sin_a = sinf(angle);
    float one_minus_cos = 1.0f - cos_a;
    
    axis = vector3_normalize(axis);
    
    float rotation[16];
    matrix_identity(rotation);
    
    rotation[0] = cos_a + axis.x * axis.x * one_minus_cos;
    rotation[1] = axis.y * axis.x * one_minus_cos + axis.z * sin_a;
    rotation[2] = axis.z * axis.x * one_minus_cos - axis.y * sin_a;
    
    rotation[4] = axis.x * axis.y * one_minus_cos - axis.z * sin_a;
    rotation[5] = cos_a + axis.y * axis.y * one_minus_cos;
    rotation[6] = axis.z * axis.y * one_minus_cos + axis.x * sin_a;
    
    rotation[8] = axis.x * axis.z * one_minus_cos + axis.y * sin_a;
    rotation[9] = axis.y * axis.z * one_minus_cos - axis.x * sin_a;
    rotation[10] = cos_a + axis.z * axis.z * one_minus_cos;
    
    float result[16];
    matrix_multiply(result, matrix, rotation);
    memcpy(matrix, result, 16 * sizeof(float));
}

void matrix_scale(float *matrix, Vector3 scale) {
    matrix[0] *= scale.x;
    matrix[1] *= scale.x;
    matrix[2] *= scale.x;
    matrix[3] *= scale.x;
    
    matrix[4] *= scale.y;
    matrix[5] *= scale.y;
    matrix[6] *= scale.y;
    matrix[7] *= scale.y;
    
    matrix[8] *= scale.z;
    matrix[9] *= scale.z;
    matrix[10] *= scale.z;
    matrix[11] *= scale.z;
}

void matrix_multiply(float *result, float *a, float *b) {
    for (int i = 0; i < 4; i++) {
        for (int j = 0; j < 4; j++) {
            result[i * 4 + j] = 0.0f;
            for (int k = 0; k < 4; k++) {
                result[i * 4 + j] += a[i * 4 + k] * b[k * 4 + j];
            }
        }
    }
}

// Shader management
unsigned int compile_shader(unsigned int type, const char *source) {
    unsigned int shader = glCreateShader(type);
    glShaderSource(shader, 1, &source, NULL);
    glCompileShader(shader);
    
    int success;
    char info_log[512];
    glGetShaderiv(shader, GL_COMPILE_STATUS, &success);
    if (!success) {
        glGetShaderInfoLog(shader, 512, NULL, info_log);
        fprintf(stderr, "Shader compilation failed: %s\n", info_log);
        return 0;
    }
    
    return shader;
}

int load_shader_program(ShaderProgram *program, const char *vertex_source, const char *fragment_source) {
    unsigned int vertex_shader = compile_shader(GL_VERTEX_SHADER, vertex_source);
    unsigned int fragment_shader = compile_shader(GL_FRAGMENT_SHADER, fragment_source);
    
    if (vertex_shader == 0 || fragment_shader == 0) {
        return -1;
    }
    
    program->program_id = glCreateProgram();
    glAttachShader(program->program_id, vertex_shader);
    glAttachShader(program->program_id, fragment_shader);
    glLinkProgram(program->program_id);
    
    int success;
    char info_log[512];
    glGetProgramiv(program->program_id, GL_LINK_STATUS, &success);
    if (!success) {
        glGetProgramInfoLog(program->program_id, 512, NULL, info_log);
        fprintf(stderr, "Shader program linking failed: %s\n", info_log);
        return -1;
    }
    
    glDeleteShader(vertex_shader);
    glDeleteShader(fragment_shader);
    
    // Get uniform locations
    program->model_loc = glGetUniformLocation(program->program_id, "model");
    program->view_loc = glGetUniformLocation(program->program_id, "view");
    program->projection_loc = glGetUniformLocation(program->program_id, "projection");
    
    program->material_ambient_loc = glGetUniformLocation(program->program_id, "materialAmbient");
    program->material_diffuse_loc = glGetUniformLocation(program->program_id, "materialDiffuse");
    program->material_specular_loc = glGetUniformLocation(program->program_id, "materialSpecular");
    program->material_shininess_loc = glGetUniformLocation(program->program_id, "materialShininess");
    
    program->camera_pos_loc = glGetUniformLocation(program->program_id, "cameraPos");
    program->num_lights_loc = glGetUniformLocation(program->program_id, "numLights");
    
    // Get light uniform locations
    for (int i = 0; i < MAX_LIGHTS; i++) {
        char uniform_name[64];
        sprintf(uniform_name, "lightPos[%d]", i);
        program->light_pos_loc[i] = glGetUniformLocation(program->program_id, uniform_name);
        
        sprintf(uniform_name, "lightColor[%d]", i);
        program->light_color_loc[i] = glGetUniformLocation(program->program_id, uniform_name);
        
        sprintf(uniform_name, "lightIntensity[%d]", i);
        program->light_intensity_loc[i] = glGetUniformLocation(program->program_id, uniform_name);
    }
    
    return 0;
}

void use_shader_program(ShaderProgram *program) {
    glUseProgram(program->program_id);
}

void set_uniform_matrix4(int location, float *matrix) {
    glUniformMatrix4fv(location, 1, GL_FALSE, matrix);
}

void set_uniform_vector3(int location, Vector3 vector) {
    glUniform3f(location, vector.x, vector.y, vector.z);
}

void set_uniform_float(int location, float value) {
    glUniform1f(location, value);
}

void set_uniform_int(int location, int value) {
    glUniform1i(location, value);
}