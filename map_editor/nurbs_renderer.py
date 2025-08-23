#!/usr/bin/env python3
"""
NURBS Renderer for the Map Editor
Handles rendering of NURBS curves and surfaces from IGS imports
"""

import numpy as np
from typing import List, Dict, Tuple, Optional
import math

# OpenGL imports
try:
    from OpenGL.GL import *
    from OpenGL.GLU import *
    OPENGL_AVAILABLE = True
except ImportError:
    OPENGL_AVAILABLE = False
    print("Warning: OpenGL not available for NURBS rendering")

class NURBSRenderer:
    """Renderer for NURBS curves and surfaces"""
    
    def __init__(self):
        self.display_lists = {}
        self.tessellation_resolution = 32
        
    def render_nurbs_curve(self, control_points: List[List[float]], knots: List[float], 
                          degree: int, color: Tuple[float, float, float] = (1.0, 1.0, 1.0)):
        """Render a NURBS curve"""
        if not OPENGL_AVAILABLE:
            return
            
        glColor3f(*color)
        glLineWidth(2.0)
        
        # Generate curve points
        curve_points = self._evaluate_nurbs_curve(control_points, knots, degree)
        
        # Render curve
        glBegin(GL_LINE_STRIP)
        for point in curve_points:
            glVertex3f(point[0], point[1], point[2])
        glEnd()
        
        # Render control points
        glColor3f(1.0, 0.0, 0.0)
        glPointSize(5.0)
        glBegin(GL_POINTS)
        for cp in control_points:
            glVertex3f(cp[0], cp[1], cp[2])
        glEnd()
        
        # Render control polygon
        glColor3f(0.5, 0.5, 0.5)
        glLineWidth(1.0)
        glBegin(GL_LINE_STRIP)
        for cp in control_points:
            glVertex3f(cp[0], cp[1], cp[2])
        glEnd()
        
    def render_nurbs_surface(self, control_points: List[List[List[float]]], 
                           knots_u: List[float], knots_v: List[float],
                           degree_u: int, degree_v: int,
                           color: Tuple[float, float, float] = (0.8, 0.8, 0.8)):
        """Render a NURBS surface"""
        if not OPENGL_AVAILABLE:
            return
            
        glColor3f(*color)
        
        # Generate surface mesh
        mesh_points, mesh_normals = self._evaluate_nurbs_surface(
            control_points, knots_u, knots_v, degree_u, degree_v
        )
        
        # Render surface as triangles
        glBegin(GL_TRIANGLES)
        for i in range(len(mesh_points) - 1):
            for j in range(len(mesh_points[0]) - 1):
                # First triangle
                p1 = mesh_points[i][j]
                p2 = mesh_points[i+1][j]
                p3 = mesh_points[i][j+1]
                n1 = mesh_normals[i][j]
                n2 = mesh_normals[i+1][j]
                n3 = mesh_normals[i][j+1]
                
                glNormal3f(*n1)
                glVertex3f(*p1)
                glNormal3f(*n2)
                glVertex3f(*p2)
                glNormal3f(*n3)
                glVertex3f(*p3)
                
                # Second triangle
                p4 = mesh_points[i+1][j+1]
                n4 = mesh_normals[i+1][j+1]
                
                glNormal3f(*n2)
                glVertex3f(*p2)
                glNormal3f(*n4)
                glVertex3f(*p4)
                glNormal3f(*n3)
                glVertex3f(*p3)
        glEnd()
        
        # Render control net (optional)
        if self.show_control_net:
            self._render_control_net(control_points, color)
    
    def _evaluate_nurbs_curve(self, control_points: List[List[float]], 
                             knots: List[float], degree: int) -> List[List[float]]:
        """Evaluate NURBS curve at multiple parameter values"""
        points = []
        num_samples = self.tessellation_resolution
        
        for i in range(num_samples + 1):
            t = knots[degree] + (knots[-degree-1] - knots[degree]) * i / num_samples
            point = self._evaluate_curve_at_t(control_points, knots, degree, t)
            points.append(point)
            
        return points
    
    def _evaluate_curve_at_t(self, control_points: List[List[float]], 
                            knots: List[float], degree: int, t: float) -> List[float]:
        """Evaluate NURBS curve at specific parameter t"""
        # Cox-de Boor algorithm for NURBS evaluation
        n = len(control_points) - 1
        k = degree
        
        # Find knot span
        span = self._find_span(n, k, t, knots)
        
        # Compute basis functions
        basis_functions = self._compute_basis_functions(span, t, k, knots)
        
        # Compute point
        point = [0.0, 0.0, 0.0]
        for i in range(k + 1):
            idx = span - k + i
            if idx < len(control_points):
                weight = control_points[idx][3] if len(control_points[idx]) > 3 else 1.0
                for j in range(3):
                    point[j] += basis_functions[i] * control_points[idx][j] * weight
                    
        # Normalize by weight
        total_weight = 0.0
        for i in range(k + 1):
            idx = span - k + i
            if idx < len(control_points):
                weight = control_points[idx][3] if len(control_points[idx]) > 3 else 1.0
                total_weight += basis_functions[i] * weight
                
        if total_weight > 0:
            for j in range(3):
                point[j] /= total_weight
                
        return point
    
    def _evaluate_nurbs_surface(self, control_points: List[List[List[float]]],
                               knots_u: List[float], knots_v: List[float],
                               degree_u: int, degree_v: int) -> Tuple[List[List[List[float]]], List[List[List[float]]]]:
        """Evaluate NURBS surface at multiple parameter values"""
        points = []
        normals = []
        num_samples_u = self.tessellation_resolution
        num_samples_v = self.tessellation_resolution
        
        for i in range(num_samples_u + 1):
            u_row = []
            n_row = []
            u = knots_u[degree_u] + (knots_u[-degree_u-1] - knots_u[degree_u]) * i / num_samples_u
            
            for j in range(num_samples_v + 1):
                v = knots_v[degree_v] + (knots_v[-degree_v-1] - knots_v[degree_v]) * j / num_samples_v
                
                point = self._evaluate_surface_at_uv(control_points, knots_u, knots_v, degree_u, degree_v, u, v)
                normal = self._compute_surface_normal(control_points, knots_u, knots_v, degree_u, degree_v, u, v)
                
                u_row.append(point)
                n_row.append(normal)
                
            points.append(u_row)
            normals.append(n_row)
            
        return points, normals
    
    def _evaluate_surface_at_uv(self, control_points: List[List[List[float]]],
                                knots_u: List[float], knots_v: List[float],
                                degree_u: int, degree_v: int, u: float, v: float) -> List[float]:
        """Evaluate NURBS surface at specific parameters (u, v)"""
        # Find knot spans
        span_u = self._find_span(len(control_points) - 1, degree_u, u, knots_u)
        span_v = self._find_span(len(control_points[0]) - 1, degree_v, v, knots_v)
        
        # Compute basis functions
        basis_u = self._compute_basis_functions(span_u, u, degree_u, knots_u)
        basis_v = self._compute_basis_functions(span_v, v, degree_v, knots_v)
        
        # Compute point
        point = [0.0, 0.0, 0.0]
        total_weight = 0.0
        
        for i in range(degree_u + 1):
            for j in range(degree_v + 1):
                idx_u = span_u - degree_u + i
                idx_v = span_v - degree_v + j
                
                if idx_u < len(control_points) and idx_v < len(control_points[0]):
                    cp = control_points[idx_u][idx_v]
                    weight = cp[3] if len(cp) > 3 else 1.0
                    basis_product = basis_u[i] * basis_v[j] * weight
                    
                    for k in range(3):
                        point[k] += basis_product * cp[k]
                    total_weight += basis_product
        
        # Normalize
        if total_weight > 0:
            for k in range(3):
                point[k] /= total_weight
                
        return point
    
    def _compute_surface_normal(self, control_points: List[List[List[float]]],
                               knots_u: List[float], knots_v: List[float],
                               degree_u: int, degree_v: int, u: float, v: float) -> List[float]:
        """Compute surface normal at parameters (u, v)"""
        # Compute tangent vectors
        tangent_u = self._compute_surface_tangent(control_points, knots_u, knots_v, degree_u, degree_v, u, v, 'u')
        tangent_v = self._compute_surface_tangent(control_points, knots_u, knots_v, degree_u, degree_v, u, v, 'v')
        
        # Cross product for normal
        normal = self._cross_product(tangent_u, tangent_v)
        
        # Normalize
        length = math.sqrt(sum(x*x for x in normal))
        if length > 0:
            normal = [x/length for x in normal]
            
        return normal
    
    def _compute_surface_tangent(self, control_points: List[List[List[float]]],
                                knots_u: List[float], knots_v: List[float],
                                degree_u: int, degree_v: int, u: float, v: float, 
                                direction: str) -> List[float]:
        """Compute surface tangent in specified direction"""
        # Simplified tangent computation
        # In a full implementation, this would compute the derivative of the surface
        delta = 0.01
        
        if direction == 'u':
            p1 = self._evaluate_surface_at_uv(control_points, knots_u, knots_v, degree_u, degree_v, u - delta, v)
            p2 = self._evaluate_surface_at_uv(control_points, knots_u, knots_v, degree_u, degree_v, u + delta, v)
        else:
            p1 = self._evaluate_surface_at_uv(control_points, knots_u, knots_v, degree_u, degree_v, u, v - delta)
            p2 = self._evaluate_surface_at_uv(control_points, knots_u, knots_v, degree_u, degree_v, u, v + delta)
            
        tangent = [p2[i] - p1[i] for i in range(3)]
        
        # Normalize
        length = math.sqrt(sum(x*x for x in tangent))
        if length > 0:
            tangent = [x/length for x in tangent]
            
        return tangent
    
    def _find_span(self, n: int, k: int, t: float, knots: List[float]) -> int:
        """Find the knot span index"""
        if t >= knots[n + 1]:
            return n
        if t <= knots[k]:
            return k
            
        low = k
        high = n + 1
        mid = (low + high) // 2
        
        while t < knots[mid] or t >= knots[mid + 1]:
            if t < knots[mid]:
                high = mid
            else:
                low = mid
            mid = (low + high) // 2
            
        return mid
    
    def _compute_basis_functions(self, span: int, t: float, k: int, knots: List[float]) -> List[float]:
        """Compute basis functions for NURBS evaluation"""
        basis = [0.0] * (k + 1)
        basis[0] = 1.0
        
        for j in range(1, k + 1):
            for r in range(j, 0, -1):
                left = span - j + r
                right = span + r
                
                if knots[right] - knots[left] != 0:
                    temp = basis[r - 1] / (knots[right] - knots[left])
                    basis[r] = basis[r] + (knots[right] - t) * temp
                    basis[r - 1] = (t - knots[left]) * temp
                else:
                    basis[r] = basis[r - 1]
                    
        return basis
    
    def _cross_product(self, a: List[float], b: List[float]) -> List[float]:
        """Compute cross product of two 3D vectors"""
        return [
            a[1] * b[2] - a[2] * b[1],
            a[2] * b[0] - a[0] * b[2],
            a[0] * b[1] - a[1] * b[0]
        ]
    
    def _render_control_net(self, control_points: List[List[List[float]]], color: Tuple[float, float, float]):
        """Render the control net of a NURBS surface"""
        if not OPENGL_AVAILABLE:
            return
            
        glColor3f(0.3, 0.3, 0.3)
        glLineWidth(1.0)
        
        # Render u-direction lines
        for i in range(len(control_points)):
            glBegin(GL_LINE_STRIP)
            for j in range(len(control_points[0])):
                cp = control_points[i][j]
                glVertex3f(cp[0], cp[1], cp[2])
            glEnd()
            
        # Render v-direction lines
        for j in range(len(control_points[0])):
            glBegin(GL_LINE_STRIP)
            for i in range(len(control_points)):
                cp = control_points[i][j]
                glVertex3f(cp[0], cp[1], cp[2])
            glEnd()
            
        # Render control points
        glColor3f(1.0, 0.0, 0.0)
        glPointSize(4.0)
        glBegin(GL_POINTS)
        for i in range(len(control_points)):
            for j in range(len(control_points[0])):
                cp = control_points[i][j]
                glVertex3f(cp[0], cp[1], cp[2])
        glEnd()
    
    def set_tessellation_resolution(self, resolution: int):
        """Set the tessellation resolution for NURBS evaluation"""
        self.tessellation_resolution = max(8, min(64, resolution))
    
    def set_show_control_net(self, show: bool):
        """Set whether to show control nets"""
        self.show_control_net = show