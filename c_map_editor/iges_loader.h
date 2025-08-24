#ifndef IGES_LOADER_H
#define IGES_LOADER_H

#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include "../src/nurbs.h"

// IGES file sections
typedef enum {
    IGES_SECTION_START = 'S',
    IGES_SECTION_GLOBAL = 'G',
    IGES_SECTION_DIRECTORY = 'D',
    IGES_SECTION_PARAMETER = 'P',
    IGES_SECTION_TERMINATE = 'T'
} IgesSection;

// IGES entity types (common ones)
typedef enum {
    IGES_ENTITY_POINT = 116,
    IGES_ENTITY_LINE = 110,
    IGES_ENTITY_CIRCULAR_ARC = 100,
    IGES_ENTITY_CONIC_ARC = 104,
    IGES_ENTITY_PARAMETRIC_SPLINE_CURVE = 112,
    IGES_ENTITY_PARAMETRIC_SPLINE_SURFACE = 114,
    IGES_ENTITY_RATIONAL_BSPLINE_CURVE = 126,
    IGES_ENTITY_RATIONAL_BSPLINE_SURFACE = 128,
    IGES_ENTITY_PLANE_SURFACE = 190,
    IGES_ENTITY_CYLINDRICAL_SURFACE = 192,
    IGES_ENTITY_CONICAL_SURFACE = 194,
    IGES_ENTITY_SPHERICAL_SURFACE = 196,
    IGES_ENTITY_TOROIDAL_SURFACE = 198,
    IGES_ENTITY_RULED_SURFACE = 118,
    IGES_ENTITY_SURFACE_OF_REVOLUTION = 120,
    IGES_ENTITY_TABULATED_CYLINDER = 122,
    IGES_ENTITY_OFFSET_SURFACE = 140,
    IGES_ENTITY_TRIMMED_SURFACE = 144,
    IGES_ENTITY_MANIFOLD_SOLID_BREP = 186
} IgesEntityType;

// IGES global parameters
typedef struct {
    char parameter_delimiter;
    char record_delimiter;
    char product_id[256];
    char file_name[256];
    char system_id[256];
    char preprocessor_version[256];
    int integer_bits;
    int single_precision_magnitude;
    int single_precision_significance;
    int double_precision_magnitude;
    int double_precision_significance;
    char product_id_receiving[256];
    float model_space_scale;
    int units_flag;
    char units_name[256];
    int max_line_weight_gradations;
    float max_line_weight;
    char file_generation_time[256];
    float min_resolution;
    float max_coordinate;
    char author[256];
    char organization[256];
    int iges_version;
    int drafting_standard;
    char model_creation_time[256];
    char application_protocol[256];
} IgesGlobalData;

// IGES directory entry
typedef struct IgesDirectoryEntry {
    int entity_type;
    int parameter_data_pointer;
    int structure;
    int line_font_pattern;
    int level;
    int view;
    int transformation_matrix;
    int label_display_associativity;
    int status_number;
    int sequence_number;
    int entity_type_number;
    int line_weight_number;
    int color_number;
    int parameter_line_count;
    int form_number;
    char reserved[8];
    char entity_label[8];
    int entity_subscript;
    struct IgesDirectoryEntry *next;
} IgesDirectoryEntry;

// IGES parameter data
typedef struct IgesParameterData {
    int directory_pointer;
    char *data;
    size_t data_length;
    struct IgesParameterData *next;
} IgesParameterData;

// IGES model structure
typedef struct IgesModel {
    IgesGlobalData global_data;
    IgesDirectoryEntry *directory_entries;
    IgesParameterData *parameter_data;
    
    // Parsed geometry
    NURBSSurface **surfaces;
    int surface_count;
    NURBSCurve **curves;
    int curve_count;
    
    // Additional metadata
    int total_entities;
    char filename[1024];
} IgesModel;

// IGES parsing functions
IgesModel* iges_load_file(const char *filename);
void iges_free_model(IgesModel *model);

// Section parsers
bool iges_parse_start_section(FILE *fp, IgesModel *model);
bool iges_parse_global_section(FILE *fp, IgesModel *model);
bool iges_parse_directory_section(FILE *fp, IgesModel *model);
bool iges_parse_parameter_section(FILE *fp, IgesModel *model);
bool iges_parse_terminate_section(FILE *fp, IgesModel *model);

// Entity parsers
bool iges_parse_rational_bspline_surface(IgesParameterData *param, NURBSSurface **surface);
bool iges_parse_rational_bspline_curve(IgesParameterData *param, NURBSCurve **curve);
bool iges_parse_plane_surface(IgesParameterData *param, NURBSSurface **surface);
bool iges_parse_cylindrical_surface(IgesParameterData *param, NURBSSurface **surface);
bool iges_parse_spherical_surface(IgesParameterData *param, NURBSSurface **surface);
bool iges_parse_toroidal_surface(IgesParameterData *param, NURBSSurface **surface);

// IGES writing functions
bool iges_save_file(const char *filename, IgesModel *model);
bool iges_write_start_section(FILE *fp, IgesModel *model);
bool iges_write_global_section(FILE *fp, IgesModel *model);
bool iges_write_directory_section(FILE *fp, IgesModel *model);
bool iges_write_parameter_section(FILE *fp, IgesModel *model);
bool iges_write_terminate_section(FILE *fp, IgesModel *model);

// Entity writers
bool iges_write_rational_bspline_surface(FILE *fp, NURBSSurface *surface, int *entity_count);
bool iges_write_rational_bspline_curve(FILE *fp, NURBSCurve *curve, int *entity_count);

// Utility functions
char* iges_read_line(FILE *fp, char section);
bool iges_parse_parameter_line(const char *line, char delimiter, char **tokens, int *token_count);
float iges_parse_float(const char *str);
int iges_parse_int(const char *str);
void iges_trim_string(char *str);

// IGES to NURBS conversion
NURBSSurface* iges_create_nurbs_surface_from_entity(IgesDirectoryEntry *entry, IgesParameterData *param);
NURBSCurve* iges_create_nurbs_curve_from_entity(IgesDirectoryEntry *entry, IgesParameterData *param);

// Error handling
typedef enum {
    IGES_ERROR_NONE = 0,
    IGES_ERROR_FILE_NOT_FOUND,
    IGES_ERROR_INVALID_FORMAT,
    IGES_ERROR_PARSE_ERROR,
    IGES_ERROR_UNSUPPORTED_ENTITY,
    IGES_ERROR_MEMORY_ALLOCATION,
    IGES_ERROR_WRITE_ERROR
} IgesError;

extern IgesError iges_last_error;
const char* iges_get_error_string(IgesError error);

// Debug functions
void iges_print_model_info(IgesModel *model);
void iges_print_directory_entries(IgesModel *model);
void iges_print_global_data(IgesGlobalData *global);

#endif // IGES_LOADER_H