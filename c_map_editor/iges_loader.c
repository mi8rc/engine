/*
 * IGES Loader Implementation
 * Full IGES file format support for NURBS Map Editor
 */

#include "iges_loader.h"
#include <string.h>
#include <ctype.h>
#include <errno.h>

// Global error tracking
IgesError iges_last_error = IGES_ERROR_NONE;

// Helper function declarations
static bool iges_read_record(FILE *fp, char *buffer, size_t buffer_size, char expected_section);
static void iges_parse_global_parameter(const char *param, int index, IgesGlobalData *global);
static IgesDirectoryEntry* iges_parse_directory_entry(const char *line1, const char *line2);
static IgesParameterData* iges_parse_parameter_entry(FILE *fp, int start_line, int line_count);
static bool iges_is_supported_entity(int entity_type);

IgesModel* iges_load_file(const char *filename) {
    iges_last_error = IGES_ERROR_NONE;
    
    FILE *fp = fopen(filename, "r");
    if (!fp) {
        iges_last_error = IGES_ERROR_FILE_NOT_FOUND;
        return NULL;
    }
    
    IgesModel *model = calloc(1, sizeof(IgesModel));
    if (!model) {
        iges_last_error = IGES_ERROR_MEMORY_ALLOCATION;
        fclose(fp);
        return NULL;
    }
    
    strncpy(model->filename, filename, sizeof(model->filename) - 1);
    
    // Parse each section in order
    if (!iges_parse_start_section(fp, model)) {
        iges_free_model(model);
        fclose(fp);
        return NULL;
    }
    
    if (!iges_parse_global_section(fp, model)) {
        iges_free_model(model);
        fclose(fp);
        return NULL;
    }
    
    if (!iges_parse_directory_section(fp, model)) {
        iges_free_model(model);
        fclose(fp);
        return NULL;
    }
    
    if (!iges_parse_parameter_section(fp, model)) {
        iges_free_model(model);
        fclose(fp);
        return NULL;
    }
    
    if (!iges_parse_terminate_section(fp, model)) {
        iges_free_model(model);
        fclose(fp);
        return NULL;
    }
    
    fclose(fp);
    
    // Convert IGES entities to NURBS structures
    if (!iges_convert_entities_to_nurbs(model)) {
        iges_free_model(model);
        return NULL;
    }
    
    return model;
}

void iges_free_model(IgesModel *model) {
    if (!model) return;
    
    // Free directory entries
    IgesDirectoryEntry *dir_entry = model->directory_entries;
    while (dir_entry) {
        IgesDirectoryEntry *next = dir_entry->next;
        free(dir_entry);
        dir_entry = next;
    }
    
    // Free parameter data
    IgesParameterData *param_data = model->parameter_data;
    while (param_data) {
        IgesParameterData *next = param_data->next;
        free(param_data->data);
        free(param_data);
        param_data = next;
    }
    
    // Free NURBS surfaces
    if (model->surfaces) {
        for (int i = 0; i < model->surface_count; i++) {
            if (model->surfaces[i]) {
                free_nurbs_surface(model->surfaces[i]);
            }
        }
        free(model->surfaces);
    }
    
    // Free NURBS curves
    if (model->curves) {
        for (int i = 0; i < model->curve_count; i++) {
            if (model->curves[i]) {
                free(model->curves[i]);
            }
        }
        free(model->curves);
    }
    
    free(model);
}

bool iges_parse_start_section(FILE *fp, IgesModel *model) {
    char buffer[82];
    
    // The start section contains human-readable information
    // We'll just skip through it until we reach the global section
    while (iges_read_record(fp, buffer, sizeof(buffer), 'S')) {
        // Just consume start section records
        // In a full implementation, we might extract useful metadata here
    }
    
    return true;
}

bool iges_parse_global_section(FILE *fp, IgesModel *model) {
    char buffer[82];
    char global_data[8192] = {0}; // Buffer for concatenated global data
    int global_length = 0;
    
    // Read all global section records and concatenate them
    while (iges_read_record(fp, buffer, sizeof(buffer), 'G')) {
        // Remove the section identifier and sequence number
        buffer[72] = '\0'; // Truncate at column 72
        
        int len = strlen(buffer);
        if (global_length + len < sizeof(global_data) - 1) {
            strcat(global_data, buffer);
            global_length += len;
        }
    }
    
    // Parse global parameters
    char delimiter = global_data[0]; // First character is parameter delimiter
    model->global_data.parameter_delimiter = delimiter;
    
    // Split global data into parameters
    char *token = strtok(global_data + 1, &delimiter); // Skip first delimiter
    int param_index = 0;
    
    while (token && param_index < 26) { // IGES has 26 global parameters
        iges_parse_global_parameter(token, param_index, &model->global_data);
        token = strtok(NULL, &delimiter);
        param_index++;
    }
    
    return true;
}

bool iges_parse_directory_section(FILE *fp, IgesModel *model) {
    char buffer1[82], buffer2[82];
    IgesDirectoryEntry *last_entry = NULL;
    
    // Directory entries come in pairs of 80-character records
    while (iges_read_record(fp, buffer1, sizeof(buffer1), 'D')) {
        if (!iges_read_record(fp, buffer2, sizeof(buffer2), 'D')) {
            iges_last_error = IGES_ERROR_INVALID_FORMAT;
            return false;
        }
        
        IgesDirectoryEntry *entry = iges_parse_directory_entry(buffer1, buffer2);
        if (!entry) {
            continue; // Skip unsupported entities
        }
        
        // Add to linked list
        if (!model->directory_entries) {
            model->directory_entries = entry;
        } else {
            last_entry->next = entry;
        }
        last_entry = entry;
        model->total_entities++;
    }
    
    return true;
}

bool iges_parse_parameter_section(FILE *fp, IgesModel *model) {
    char buffer[82];
    IgesParameterData *last_param = NULL;
    
    // Parameter data is referenced by directory entries
    IgesDirectoryEntry *dir_entry = model->directory_entries;
    while (dir_entry) {
        if (dir_entry->parameter_line_count > 0) {
            IgesParameterData *param = iges_parse_parameter_entry(
                fp, dir_entry->parameter_data_pointer, dir_entry->parameter_line_count
            );
            
            if (param) {
                param->directory_pointer = dir_entry->sequence_number;
                
                if (!model->parameter_data) {
                    model->parameter_data = param;
                } else {
                    last_param->next = param;
                }
                last_param = param;
            }
        }
        dir_entry = dir_entry->next;
    }
    
    return true;
}

bool iges_parse_terminate_section(FILE *fp, IgesModel *model) {
    char buffer[82];
    
    // Terminate section contains counts and should match our parsed data
    if (iges_read_record(fp, buffer, sizeof(buffer), 'T')) {
        // Parse the counts from the terminate record
        // Format: S###G###D###P###T###
        // We could validate our counts here
    }
    
    return true;
}

// Entity parsing functions
bool iges_parse_rational_bspline_surface(IgesParameterData *param, NURBSSurface **surface) {
    if (!param || !param->data) {
        iges_last_error = IGES_ERROR_PARSE_ERROR;
        return false;
    }
    
    *surface = calloc(1, sizeof(NURBSSurface));
    if (!*surface) {
        iges_last_error = IGES_ERROR_MEMORY_ALLOCATION;
        return false;
    }
    
    // Parse parameter data for NURBS surface
    char *data = param->data;
    char *tokens[1024];
    int token_count = 0;
    
    if (!iges_parse_parameter_line(data, ',', tokens, &token_count)) {
        free(*surface);
        *surface = NULL;
        return false;
    }
    
    if (token_count < 11) {
        iges_last_error = IGES_ERROR_PARSE_ERROR;
        free(*surface);
        *surface = NULL;
        return false;
    }
    
    // Parse NURBS surface parameters
    int idx = 0;
    int K1 = iges_parse_int(tokens[idx++]); // Upper index of first sum
    int K2 = iges_parse_int(tokens[idx++]); // Upper index of second sum
    int M1 = iges_parse_int(tokens[idx++]); // Degree of first set of basis functions
    int M2 = iges_parse_int(tokens[idx++]); // Degree of second set of basis functions
    int PROP1 = iges_parse_int(tokens[idx++]); // 0=non-rational, 1=rational
    int PROP2 = iges_parse_int(tokens[idx++]); // 0=non-periodic, 1=periodic in first direction
    int PROP3 = iges_parse_int(tokens[idx++]); // 0=non-periodic, 1=periodic in second direction
    int PROP4 = iges_parse_int(tokens[idx++]); // 0=open, 1=closed in first direction
    int PROP5 = iges_parse_int(tokens[idx++]); // 0=open, 1=closed in second direction
    
    (*surface)->degree_u = M1;
    (*surface)->degree_v = M2;
    (*surface)->num_control_points_u = K1 + 1;
    (*surface)->num_control_points_v = K2 + 1;
    
    // Parse knot vectors
    (*surface)->num_knots_u = K1 + M1 + 2;
    (*surface)->num_knots_v = K2 + M2 + 2;
    
    for (int i = 0; i < (*surface)->num_knots_u && idx < token_count; i++) {
        (*surface)->knots_u[i] = iges_parse_float(tokens[idx++]);
    }
    
    for (int i = 0; i < (*surface)->num_knots_v && idx < token_count; i++) {
        (*surface)->knots_v[i] = iges_parse_float(tokens[idx++]);
    }
    
    // Parse weights (if rational)
    float weights[MAX_CONTROL_POINTS][MAX_CONTROL_POINTS];
    if (PROP1 == 1) { // Rational
        for (int j = 0; j <= K2 && idx < token_count; j++) {
            for (int i = 0; i <= K1 && idx < token_count; i++) {
                weights[i][j] = iges_parse_float(tokens[idx++]);
            }
        }
    } else {
        // Non-rational, set all weights to 1.0
        for (int j = 0; j <= K2; j++) {
            for (int i = 0; i <= K1; i++) {
                weights[i][j] = 1.0f;
            }
        }
    }
    
    // Parse control points
    for (int j = 0; j <= K2 && idx < token_count; j++) {
        for (int i = 0; i <= K1 && idx < token_count; i++) {
            float x = iges_parse_float(tokens[idx++]);
            float y = iges_parse_float(tokens[idx++]);
            float z = iges_parse_float(tokens[idx++]);
            
            (*surface)->control_points[i][j].x = x * weights[i][j];
            (*surface)->control_points[i][j].y = y * weights[i][j];
            (*surface)->control_points[i][j].z = z * weights[i][j];
            (*surface)->control_points[i][j].w = weights[i][j];
        }
    }
    
    // Parse parameter ranges
    float U_min = iges_parse_float(tokens[idx++]);
    float U_max = iges_parse_float(tokens[idx++]);
    float V_min = iges_parse_float(tokens[idx++]);
    float V_max = iges_parse_float(tokens[idx++]);
    
    return true;
}

bool iges_parse_rational_bspline_curve(IgesParameterData *param, NURBSCurve **curve) {
    if (!param || !param->data) {
        iges_last_error = IGES_ERROR_PARSE_ERROR;
        return false;
    }
    
    *curve = calloc(1, sizeof(NURBSCurve));
    if (!*curve) {
        iges_last_error = IGES_ERROR_MEMORY_ALLOCATION;
        return false;
    }
    
    // Parse parameter data for NURBS curve
    char *data = param->data;
    char *tokens[512];
    int token_count = 0;
    
    if (!iges_parse_parameter_line(data, ',', tokens, &token_count)) {
        free(*curve);
        *curve = NULL;
        return false;
    }
    
    if (token_count < 6) {
        iges_last_error = IGES_ERROR_PARSE_ERROR;
        free(*curve);
        *curve = NULL;
        return false;
    }
    
    // Parse NURBS curve parameters
    int idx = 0;
    int K = iges_parse_int(tokens[idx++]); // Upper index of sum
    int M = iges_parse_int(tokens[idx++]); // Degree of basis functions
    int PROP1 = iges_parse_int(tokens[idx++]); // 0=non-rational, 1=rational
    int PROP2 = iges_parse_int(tokens[idx++]); // 0=non-periodic, 1=periodic
    int PROP3 = iges_parse_int(tokens[idx++]); // 0=open, 1=closed
    
    (*curve)->degree = M;
    (*curve)->num_control_points = K + 1;
    (*curve)->num_knots = K + M + 2;
    
    // Parse knot vector
    for (int i = 0; i < (*curve)->num_knots && idx < token_count; i++) {
        (*curve)->knots[i] = iges_parse_float(tokens[idx++]);
    }
    
    // Parse weights (if rational)
    float weights[MAX_CONTROL_POINTS];
    if (PROP1 == 1) { // Rational
        for (int i = 0; i <= K && idx < token_count; i++) {
            weights[i] = iges_parse_float(tokens[idx++]);
        }
    } else {
        // Non-rational, set all weights to 1.0
        for (int i = 0; i <= K; i++) {
            weights[i] = 1.0f;
        }
    }
    
    // Parse control points
    for (int i = 0; i <= K && idx < token_count; i++) {
        float x = iges_parse_float(tokens[idx++]);
        float y = iges_parse_float(tokens[idx++]);
        float z = iges_parse_float(tokens[idx++]);
        
        (*curve)->control_points[i].x = x * weights[i];
        (*curve)->control_points[i].y = y * weights[i];
        (*curve)->control_points[i].z = z * weights[i];
        (*curve)->control_points[i].w = weights[i];
    }
    
    // Parse parameter range
    float T_min = iges_parse_float(tokens[idx++]);
    float T_max = iges_parse_float(tokens[idx++]);
    
    return true;
}

// Helper function implementations
static bool iges_read_record(FILE *fp, char *buffer, size_t buffer_size, char expected_section) {
    if (!fgets(buffer, buffer_size, fp)) {
        return false;
    }
    
    // Check if this is the expected section
    if (strlen(buffer) >= 73 && buffer[72] != expected_section) {
        // Rewind one line
        fseek(fp, -strlen(buffer), SEEK_CUR);
        return false;
    }
    
    return true;
}

static void iges_parse_global_parameter(const char *param, int index, IgesGlobalData *global) {
    iges_trim_string((char*)param);
    
    switch (index) {
        case 0: global->parameter_delimiter = param[0]; break;
        case 1: global->record_delimiter = param[0]; break;
        case 2: strncpy(global->product_id, param, sizeof(global->product_id) - 1); break;
        case 3: strncpy(global->file_name, param, sizeof(global->file_name) - 1); break;
        case 4: strncpy(global->system_id, param, sizeof(global->system_id) - 1); break;
        case 5: strncpy(global->preprocessor_version, param, sizeof(global->preprocessor_version) - 1); break;
        case 6: global->integer_bits = iges_parse_int(param); break;
        case 7: global->single_precision_magnitude = iges_parse_int(param); break;
        case 8: global->single_precision_significance = iges_parse_int(param); break;
        case 9: global->double_precision_magnitude = iges_parse_int(param); break;
        case 10: global->double_precision_significance = iges_parse_int(param); break;
        case 11: strncpy(global->product_id_receiving, param, sizeof(global->product_id_receiving) - 1); break;
        case 12: global->model_space_scale = iges_parse_float(param); break;
        case 13: global->units_flag = iges_parse_int(param); break;
        case 14: strncpy(global->units_name, param, sizeof(global->units_name) - 1); break;
        case 15: global->max_line_weight_gradations = iges_parse_int(param); break;
        case 16: global->max_line_weight = iges_parse_float(param); break;
        case 17: strncpy(global->file_generation_time, param, sizeof(global->file_generation_time) - 1); break;
        case 18: global->min_resolution = iges_parse_float(param); break;
        case 19: global->max_coordinate = iges_parse_float(param); break;
        case 20: strncpy(global->author, param, sizeof(global->author) - 1); break;
        case 21: strncpy(global->organization, param, sizeof(global->organization) - 1); break;
        case 22: global->iges_version = iges_parse_int(param); break;
        case 23: global->drafting_standard = iges_parse_int(param); break;
        case 24: strncpy(global->model_creation_time, param, sizeof(global->model_creation_time) - 1); break;
        case 25: strncpy(global->application_protocol, param, sizeof(global->application_protocol) - 1); break;
    }
}

// Continue with more helper functions...
// (File is getting long, will continue in next part)