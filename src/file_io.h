#ifndef FILE_IO_H
#define FILE_IO_H

#include "main.h"
#include "nurbs.h"

// File format support
typedef enum {
    FILE_FORMAT_UNKNOWN,
    FILE_FORMAT_NATIVE,  // Our custom format
    FILE_FORMAT_IGES,    // IGES (.igs/.iges)
    FILE_FORMAT_STEP,    // STEP (.stp/.step)
    FILE_FORMAT_OBJ      // Wavefront OBJ (export only)
} FileFormat;

// IGES entity types
typedef enum {
    IGES_ENTITY_LINE = 110,
    IGES_ENTITY_CIRCULAR_ARC = 100,
    IGES_ENTITY_CONIC_ARC = 104,
    IGES_ENTITY_SPLINE_CURVE = 112,
    IGES_ENTITY_SPLINE_SURFACE = 128,
    IGES_ENTITY_NURBS_CURVE = 126,
    IGES_ENTITY_NURBS_SURFACE = 128,
    IGES_ENTITY_POINT = 116,
    IGES_ENTITY_TRANSFORMATION_MATRIX = 124
} IgesEntityType;

// IGES file structure
typedef struct {
    char start_section[81];
    GList *global_section;
    GList *directory_entries;
    GList *parameter_data;
    char terminate_section[81];
} IgesFile;

typedef struct {
    int entity_type;
    int parameter_data_pointer;
    int structure;
    int line_font_pattern;
    int level;
    int view;
    int transformation_matrix;
    int label_display;
    int status_number;
    int sequence_number;
    char entity_label[9];
    int entity_subscript_number;
} IgesDirectoryEntry;

// Function declarations
FileFormat detect_file_format(const char *filename);
gboolean save_file(const char *filename, GList *objects);
GList *load_file(const char *filename);

// Native format
gboolean save_native_format(const char *filename, GList *objects);
GList *load_native_format(const char *filename);

// IGES format
gboolean save_iges_file(const char *filename, GList *objects);
GList *load_iges_file(const char *filename);
IgesFile *parse_iges_file(const char *filename);
void free_iges_file(IgesFile *iges);
MapObject *convert_iges_entity_to_object(IgesDirectoryEntry *entry, const char *param_data);
char *convert_object_to_iges_entity(MapObject *object, int *entity_count);

// OBJ format (export only)
gboolean export_obj_file(const char *filename, GList *objects);

// Utility functions
char *read_file_contents(const char *filename);
gboolean write_file_contents(const char *filename, const char *contents);
void parse_iges_parameter_data(const char *data, float *values, int *count);
char *format_iges_parameter_data(float *values, int count);

// Error handling
typedef enum {
    FILE_ERROR_NONE,
    FILE_ERROR_NOT_FOUND,
    FILE_ERROR_PERMISSION_DENIED,
    FILE_ERROR_INVALID_FORMAT,
    FILE_ERROR_CORRUPTED_DATA,
    FILE_ERROR_UNSUPPORTED_VERSION,
    FILE_ERROR_MEMORY_ERROR
} FileError;

extern FileError g_last_file_error;
const char *get_file_error_string(FileError error);

#endif // FILE_IO_H