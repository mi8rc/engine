#include "nurbs.h"
#include <math.h>

NurbsCurve *nurbs_curve_new(int degree, int num_control_points) {
    NurbsCurve *curve = g_malloc0(sizeof(NurbsCurve));
    
    curve->degree = degree;
    curve->num_control_points = num_control_points;
    curve->num_knots = num_control_points + degree + 1;
    
    curve->control_points = g_malloc0(num_control_points * sizeof(ControlPoint));
    curve->knots = g_malloc0(curve->num_knots * sizeof(float));
    
    // Initialize with default values
    for (int i = 0; i < num_control_points; i++) {
        curve->control_points[i].w = 1.0f; // Default weight
    }
    
    // Generate uniform knot vector
    generate_uniform_knots(curve->knots, curve->num_knots, degree);
    
    curve->dirty = TRUE;
    curve->vao = curve->vbo = 0;
    curve->vertices = NULL;
    curve->num_vertices = 0;
    
    return curve;
}

void nurbs_curve_free(NurbsCurve *curve) {
    if (!curve) return;
    
    g_free(curve->control_points);
    g_free(curve->knots);
    g_free(curve->vertices);
    
    if (curve->vao) glDeleteVertexArrays(1, &curve->vao);
    if (curve->vbo) glDeleteBuffers(1, &curve->vbo);
    
    g_free(curve);
}

void nurbs_curve_set_control_point(NurbsCurve *curve, int index, float x, float y, float z, float w) {
    if (!curve || index < 0 || index >= curve->num_control_points) return;
    
    curve->control_points[index].x = x;
    curve->control_points[index].y = y;
    curve->control_points[index].z = z;
    curve->control_points[index].w = w;
    curve->dirty = TRUE;
}

void nurbs_curve_set_knot(NurbsCurve *curve, int index, float value) {
    if (!curve || index < 0 || index >= curve->num_knots) return;
    
    curve->knots[index] = value;
    curve->dirty = TRUE;
}

float nurbs_basis_function(int i, int degree, float t, float *knots) {
    if (degree == 0) {
        return (t >= knots[i] && t < knots[i + 1]) ? 1.0f : 0.0f;
    }
    
    float left = 0.0f, right = 0.0f;
    
    if (knots[i + degree] != knots[i]) {
        left = (t - knots[i]) / (knots[i + degree] - knots[i]) * 
               nurbs_basis_function(i, degree - 1, t, knots);
    }
    
    if (knots[i + degree + 1] != knots[i + 1]) {
        right = (knots[i + degree + 1] - t) / (knots[i + degree + 1] - knots[i + 1]) * 
                nurbs_basis_function(i + 1, degree - 1, t, knots);
    }
    
    return left + right;
}

void evaluate_nurbs_curve(NurbsCurve *curve, float t, float *point) {
    if (!curve || !point) return;
    
    float w_sum = 0.0f;
    point[0] = point[1] = point[2] = 0.0f;
    
    for (int i = 0; i < curve->num_control_points; i++) {
        float basis = nurbs_basis_function(i, curve->degree, t, curve->knots);
        float weight = curve->control_points[i].w * basis;
        
        point[0] += curve->control_points[i].x * weight;
        point[1] += curve->control_points[i].y * weight;
        point[2] += curve->control_points[i].z * weight;
        w_sum += weight;
    }
    
    if (w_sum > 0.0f) {
        point[0] /= w_sum;
        point[1] /= w_sum;
        point[2] /= w_sum;
    }
}

void nurbs_curve_tessellate(NurbsCurve *curve, int resolution) {
    if (!curve) return;
    
    // Free old vertex data
    g_free(curve->vertices);
    
    // Allocate new vertex data (position + normal + color)
    curve->num_vertices = resolution * 6; // 3 for pos, 3 for color
    curve->vertices = g_malloc0(curve->num_vertices * sizeof(float));
    
    float t_start = curve->knots[curve->degree];
    float t_end = curve->knots[curve->num_control_points];
    float t_step = (t_end - t_start) / (resolution - 1);
    
    for (int i = 0; i < resolution; i++) {
        float t = t_start + i * t_step;
        float point[3];
        
        evaluate_nurbs_curve(curve, t, point);
        
        // Position
        curve->vertices[i * 6 + 0] = point[0];
        curve->vertices[i * 6 + 1] = point[1];
        curve->vertices[i * 6 + 2] = point[2];
        
        // Color (default blue)
        curve->vertices[i * 6 + 3] = 0.0f; // R
        curve->vertices[i * 6 + 4] = 0.5f; // G
        curve->vertices[i * 6 + 5] = 1.0f; // B
    }
    
    // Update OpenGL buffers
    if (!curve->vao) {
        glGenVertexArrays(1, &curve->vao);
        glGenBuffers(1, &curve->vbo);
    }
    
    glBindVertexArray(curve->vao);
    glBindBuffer(GL_ARRAY_BUFFER, curve->vbo);
    glBufferData(GL_ARRAY_BUFFER, curve->num_vertices * sizeof(float), 
                 curve->vertices, GL_DYNAMIC_DRAW);
    
    // Position attribute
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(0);
    
    // Color attribute
    glVertexAttribPointer(2, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), 
                         (void*)(3 * sizeof(float)));
    glEnableVertexAttribArray(2);
    
    glBindBuffer(GL_ARRAY_BUFFER, 0);
    glBindVertexArray(0);
    
    curve->dirty = FALSE;
}

void nurbs_curve_render(NurbsCurve *curve) {
    if (!curve || !curve->vao) return;
    
    if (curve->dirty) {
        nurbs_curve_tessellate(curve, 100);
    }
    
    glBindVertexArray(curve->vao);
    glDrawArrays(GL_LINE_STRIP, 0, curve->num_vertices / 6);
    glBindVertexArray(0);
}

NurbsSurface *nurbs_surface_new(int degree_u, int degree_v, int num_u, int num_v) {
    NurbsSurface *surface = g_malloc0(sizeof(NurbsSurface));
    
    surface->degree_u = degree_u;
    surface->degree_v = degree_v;
    surface->num_control_points_u = num_u;
    surface->num_control_points_v = num_v;
    surface->num_knots_u = num_u + degree_u + 1;
    surface->num_knots_v = num_v + degree_v + 1;
    
    // Allocate control points as 2D array
    surface->control_points = g_malloc0(num_u * sizeof(ControlPoint*));
    for (int i = 0; i < num_u; i++) {
        surface->control_points[i] = g_malloc0(num_v * sizeof(ControlPoint));
        for (int j = 0; j < num_v; j++) {
            surface->control_points[i][j].w = 1.0f; // Default weight
        }
    }
    
    surface->knots_u = g_malloc0(surface->num_knots_u * sizeof(float));
    surface->knots_v = g_malloc0(surface->num_knots_v * sizeof(float));
    
    // Generate uniform knot vectors
    generate_uniform_knots(surface->knots_u, surface->num_knots_u, degree_u);
    generate_uniform_knots(surface->knots_v, surface->num_knots_v, degree_v);
    
    surface->dirty = TRUE;
    surface->vao = surface->vbo = surface->ebo = 0;
    surface->vertices = NULL;
    surface->indices = NULL;
    surface->num_vertices = surface->num_indices = 0;
    
    return surface;
}

void nurbs_surface_free(NurbsSurface *surface) {
    if (!surface) return;
    
    for (int i = 0; i < surface->num_control_points_u; i++) {
        g_free(surface->control_points[i]);
    }
    g_free(surface->control_points);
    g_free(surface->knots_u);
    g_free(surface->knots_v);
    g_free(surface->vertices);
    g_free(surface->indices);
    
    if (surface->vao) glDeleteVertexArrays(1, &surface->vao);
    if (surface->vbo) glDeleteBuffers(1, &surface->vbo);
    if (surface->ebo) glDeleteBuffers(1, &surface->ebo);
    
    g_free(surface);
}

void evaluate_nurbs_surface(NurbsSurface *surface, float u, float v, float *point) {
    if (!surface || !point) return;
    
    float w_sum = 0.0f;
    point[0] = point[1] = point[2] = 0.0f;
    
    for (int i = 0; i < surface->num_control_points_u; i++) {
        for (int j = 0; j < surface->num_control_points_v; j++) {
            float basis_u = nurbs_basis_function(i, surface->degree_u, u, surface->knots_u);
            float basis_v = nurbs_basis_function(j, surface->degree_v, v, surface->knots_v);
            float basis = basis_u * basis_v;
            float weight = surface->control_points[i][j].w * basis;
            
            point[0] += surface->control_points[i][j].x * weight;
            point[1] += surface->control_points[i][j].y * weight;
            point[2] += surface->control_points[i][j].z * weight;
            w_sum += weight;
        }
    }
    
    if (w_sum > 0.0f) {
        point[0] /= w_sum;
        point[1] /= w_sum;
        point[2] /= w_sum;
    }
}

void nurbs_surface_tessellate(NurbsSurface *surface, int resolution_u, int resolution_v) {
    if (!surface) return;
    
    // Free old data
    g_free(surface->vertices);
    g_free(surface->indices);
    
    // Allocate new vertex data
    surface->num_vertices = resolution_u * resolution_v * 9; // pos(3) + normal(3) + color(3)
    surface->vertices = g_malloc0(surface->num_vertices * sizeof(float));
    
    surface->num_indices = (resolution_u - 1) * (resolution_v - 1) * 6; // 2 triangles per quad
    surface->indices = g_malloc0(surface->num_indices * sizeof(unsigned int));
    
    float u_start = surface->knots_u[surface->degree_u];
    float u_end = surface->knots_u[surface->num_control_points_u];
    float v_start = surface->knots_v[surface->degree_v];
    float v_end = surface->knots_v[surface->num_control_points_v];
    
    float u_step = (u_end - u_start) / (resolution_u - 1);
    float v_step = (v_end - v_start) / (resolution_v - 1);
    
    // Generate vertices
    for (int i = 0; i < resolution_u; i++) {
        for (int j = 0; j < resolution_v; j++) {
            float u = u_start + i * u_step;
            float v = v_start + j * v_step;
            float point[3];
            
            evaluate_nurbs_surface(surface, u, v, point);
            
            int vertex_index = (i * resolution_v + j) * 9;
            
            // Position
            surface->vertices[vertex_index + 0] = point[0];
            surface->vertices[vertex_index + 1] = point[1];
            surface->vertices[vertex_index + 2] = point[2];
            
            // Normal (simplified - should compute proper surface normal)
            surface->vertices[vertex_index + 3] = 0.0f;
            surface->vertices[vertex_index + 4] = 1.0f;
            surface->vertices[vertex_index + 5] = 0.0f;
            
            // Color (default green)
            surface->vertices[vertex_index + 6] = 0.0f;
            surface->vertices[vertex_index + 7] = 0.8f;
            surface->vertices[vertex_index + 8] = 0.2f;
        }
    }
    
    // Generate indices
    int index = 0;
    for (int i = 0; i < resolution_u - 1; i++) {
        for (int j = 0; j < resolution_v - 1; j++) {
            int v0 = i * resolution_v + j;
            int v1 = v0 + 1;
            int v2 = (i + 1) * resolution_v + j;
            int v3 = v2 + 1;
            
            // First triangle
            surface->indices[index++] = v0;
            surface->indices[index++] = v1;
            surface->indices[index++] = v2;
            
            // Second triangle
            surface->indices[index++] = v1;
            surface->indices[index++] = v3;
            surface->indices[index++] = v2;
        }
    }
    
    // Update OpenGL buffers
    if (!surface->vao) {
        glGenVertexArrays(1, &surface->vao);
        glGenBuffers(1, &surface->vbo);
        glGenBuffers(1, &surface->ebo);
    }
    
    glBindVertexArray(surface->vao);
    
    glBindBuffer(GL_ARRAY_BUFFER, surface->vbo);
    glBufferData(GL_ARRAY_BUFFER, surface->num_vertices * sizeof(float),
                 surface->vertices, GL_DYNAMIC_DRAW);
    
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, surface->ebo);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, surface->num_indices * sizeof(unsigned int),
                 surface->indices, GL_DYNAMIC_DRAW);
    
    // Position attribute
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 9 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(0);
    
    // Normal attribute
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 9 * sizeof(float), 
                         (void*)(3 * sizeof(float)));
    glEnableVertexAttribArray(1);
    
    // Color attribute
    glVertexAttribPointer(2, 3, GL_FLOAT, GL_FALSE, 9 * sizeof(float), 
                         (void*)(6 * sizeof(float)));
    glEnableVertexAttribArray(2);
    
    glBindBuffer(GL_ARRAY_BUFFER, 0);
    glBindVertexArray(0);
    
    surface->dirty = FALSE;
}

void nurbs_surface_render(NurbsSurface *surface) {
    if (!surface || !surface->vao) return;
    
    if (surface->dirty) {
        nurbs_surface_tessellate(surface, 20, 20);
    }
    
    glBindVertexArray(surface->vao);
    glDrawElements(GL_TRIANGLES, surface->num_indices, GL_UNSIGNED_INT, 0);
    glBindVertexArray(0);
}

MapObject *map_object_new(ObjectType type, const char *name) {
    MapObject *object = g_malloc0(sizeof(MapObject));
    
    object->type = type;
    object->name = g_strdup(name ? name : "Untitled");
    object->visible = TRUE;
    object->selected = FALSE;
    
    // Initialize transform
    object->position[0] = object->position[1] = object->position[2] = 0.0f;
    object->rotation[0] = object->rotation[1] = object->rotation[2] = 0.0f;
    object->scale[0] = object->scale[1] = object->scale[2] = 1.0f;
    
    object->parent = NULL;
    
    switch (type) {
        case OBJECT_CURVE:
            object->data.curve = NULL;
            break;
        case OBJECT_SURFACE:
            object->data.surface = NULL;
            break;
        case OBJECT_GROUP:
            object->data.children = NULL;
            break;
    }
    
    return object;
}

void map_object_free(MapObject *object) {
    if (!object) return;
    
    g_free(object->name);
    
    switch (object->type) {
        case OBJECT_CURVE:
            nurbs_curve_free(object->data.curve);
            break;
        case OBJECT_SURFACE:
            nurbs_surface_free(object->data.surface);
            break;
        case OBJECT_GROUP:
            g_list_free_full(object->data.children, (GDestroyNotify)map_object_free);
            break;
    }
    
    g_free(object);
}

void generate_uniform_knots(float *knots, int num_knots, int degree) {
    if (!knots) return;
    
    int num_interior = num_knots - 2 * (degree + 1);
    
    // Set initial knots to 0
    for (int i = 0; i <= degree; i++) {
        knots[i] = 0.0f;
    }
    
    // Set interior knots
    for (int i = 0; i < num_interior; i++) {
        knots[degree + 1 + i] = (float)(i + 1) / (num_interior + 1);
    }
    
    // Set final knots to 1
    for (int i = 0; i <= degree; i++) {
        knots[num_knots - degree - 1 + i] = 1.0f;
    }
}