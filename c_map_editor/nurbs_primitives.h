#ifndef NURBS_PRIMITIVES_H
#define NURBS_PRIMITIVES_H

#include "../src/nurbs.h"

// NURBS primitive creation functions
// These functions create the basic NURBS shapes used in the editor

/**
 * Create a NURBS sphere with the specified radius
 * @param radius The radius of the sphere
 * @return Pointer to the created NURBSSurface, or NULL on failure
 */
NURBSSurface* nurbs_create_sphere(float radius);

/**
 * Create a NURBS plane with the specified width and height
 * @param width The width of the plane
 * @param height The height of the plane
 * @return Pointer to the created NURBSSurface, or NULL on failure
 */
NURBSSurface* nurbs_create_plane(float width, float height);

/**
 * Create a NURBS cylinder with the specified radius and height
 * @param radius The radius of the cylinder
 * @param height The height of the cylinder
 * @return Pointer to the created NURBSSurface, or NULL on failure
 */
NURBSSurface* nurbs_create_cylinder(float radius, float height);

/**
 * Create a NURBS torus with the specified major and minor radii
 * @param major_radius The major radius (distance from center to tube center)
 * @param minor_radius The minor radius (tube radius)
 * @return Pointer to the created NURBSSurface, or NULL on failure
 */
NURBSSurface* nurbs_create_torus(float major_radius, float minor_radius);

/**
 * Create a NURBS cone with the specified radius and height
 * @param bottom_radius The radius at the bottom of the cone
 * @param top_radius The radius at the top of the cone (0 for a point)
 * @param height The height of the cone
 * @return Pointer to the created NURBSSurface, or NULL on failure
 */
NURBSSurface* nurbs_create_cone(float bottom_radius, float top_radius, float height);

/**
 * Create a NURBS cube with the specified dimensions
 * @param width The width of the cube
 * @param height The height of the cube
 * @param depth The depth of the cube
 * @return Array of 6 NURBSSurface pointers (one for each face), or NULL on failure
 */
NURBSSurface** nurbs_create_cube(float width, float height, float depth);

// Advanced primitive functions

/**
 * Create a NURBS surface of revolution by rotating a curve around an axis
 * @param profile_curve The curve to revolve
 * @param axis_start Starting point of the revolution axis
 * @param axis_end Ending point of the revolution axis
 * @param start_angle Starting angle in radians
 * @param end_angle Ending angle in radians
 * @return Pointer to the created NURBSSurface, or NULL on failure
 */
NURBSSurface* nurbs_create_surface_of_revolution(NURBSCurve *profile_curve, 
                                                Vector3 axis_start, Vector3 axis_end,
                                                float start_angle, float end_angle);

/**
 * Create a NURBS ruled surface between two curves
 * @param curve1 First boundary curve
 * @param curve2 Second boundary curve
 * @return Pointer to the created NURBSSurface, or NULL on failure
 */
NURBSSurface* nurbs_create_ruled_surface(NURBSCurve *curve1, NURBSCurve *curve2);

/**
 * Create a NURBS swept surface by sweeping a profile along a path
 * @param profile_curve The profile curve to sweep
 * @param path_curve The path curve to sweep along
 * @param scale_factor Scaling factor along the path (1.0 = no scaling)
 * @return Pointer to the created NURBSSurface, or NULL on failure
 */
NURBSSurface* nurbs_create_swept_surface(NURBSCurve *profile_curve, NURBSCurve *path_curve, float scale_factor);

// Curve primitives

/**
 * Create a NURBS circle curve
 * @param center Center point of the circle
 * @param radius Radius of the circle
 * @param normal Normal vector to the circle plane
 * @return Pointer to the created NURBSCurve, or NULL on failure
 */
NURBSCurve* nurbs_create_circle_curve(Vector3 center, float radius, Vector3 normal);

/**
 * Create a NURBS line curve
 * @param start Starting point of the line
 * @param end Ending point of the line
 * @return Pointer to the created NURBSCurve, or NULL on failure
 */
NURBSCurve* nurbs_create_line_curve(Vector3 start, Vector3 end);

/**
 * Create a NURBS arc curve
 * @param center Center point of the arc
 * @param radius Radius of the arc
 * @param start_angle Starting angle in radians
 * @param end_angle Ending angle in radians
 * @param normal Normal vector to the arc plane
 * @return Pointer to the created NURBSCurve, or NULL on failure
 */
NURBSCurve* nurbs_create_arc_curve(Vector3 center, float radius, 
                                  float start_angle, float end_angle, Vector3 normal);

/**
 * Create a NURBS ellipse curve
 * @param center Center point of the ellipse
 * @param major_radius Major axis radius
 * @param minor_radius Minor axis radius
 * @param normal Normal vector to the ellipse plane
 * @return Pointer to the created NURBSCurve, or NULL on failure
 */
NURBSCurve* nurbs_create_ellipse_curve(Vector3 center, float major_radius, float minor_radius, Vector3 normal);

// Utility functions for primitive creation

/**
 * Generate uniform knot vector for the given parameters
 * @param degree Degree of the curve/surface
 * @param num_control_points Number of control points
 * @param knots Output array for knot values (must be pre-allocated)
 * @return Number of knots generated
 */
int nurbs_generate_uniform_knots(int degree, int num_control_points, float *knots);

/**
 * Generate open uniform knot vector (clamped at ends)
 * @param degree Degree of the curve/surface
 * @param num_control_points Number of control points
 * @param knots Output array for knot values (must be pre-allocated)
 * @return Number of knots generated
 */
int nurbs_generate_open_uniform_knots(int degree, int num_control_points, float *knots);

/**
 * Calculate control points for a circular arc in NURBS form
 * @param center Center of the arc
 * @param radius Radius of the arc
 * @param start_angle Starting angle
 * @param end_angle Ending angle
 * @param normal Normal vector to the arc plane
 * @param control_points Output array for control points (must be pre-allocated)
 * @param weights Output array for weights (must be pre-allocated)
 * @return Number of control points generated
 */
int nurbs_calculate_arc_control_points(Vector3 center, float radius,
                                      float start_angle, float end_angle, Vector3 normal,
                                      Vector4 *control_points, float *weights);

/**
 * Transform a NURBS surface by applying translation, rotation, and scaling
 * @param surface The surface to transform
 * @param translation Translation vector
 * @param rotation Rotation angles (in radians) around X, Y, Z axes
 * @param scale Scaling factors for X, Y, Z axes
 */
void nurbs_transform_surface(NURBSSurface *surface, Vector3 translation, Vector3 rotation, Vector3 scale);

/**
 * Transform a NURBS curve by applying translation, rotation, and scaling
 * @param curve The curve to transform
 * @param translation Translation vector
 * @param rotation Rotation angles (in radians) around X, Y, Z axes
 * @param scale Scaling factors for X, Y, Z axes
 */
void nurbs_transform_curve(NURBSCurve *curve, Vector3 translation, Vector3 rotation, Vector3 scale);

// Validation functions

/**
 * Validate a NURBS surface for correctness
 * @param surface The surface to validate
 * @return true if the surface is valid, false otherwise
 */
bool nurbs_validate_surface(NURBSSurface *surface);

/**
 * Validate a NURBS curve for correctness
 * @param curve The curve to validate
 * @return true if the curve is valid, false otherwise
 */
bool nurbs_validate_curve(NURBSCurve *curve);

#endif // NURBS_PRIMITIVES_H