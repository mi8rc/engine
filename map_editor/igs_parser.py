#!/usr/bin/env python3
"""
IGS/IGES File Parser for NURBS Map Editor
Converts IGS entities to the editor's NURBS format
"""

import numpy as np
from typing import List, Dict, Tuple, Optional, Union
from dataclasses import dataclass
import math

# Try to import python-iges, fall back to basic parser if not available
try:
    import iges
    IGS_AVAILABLE = True
except ImportError:
    print("Warning: python-iges not available. Using basic IGS parser.")
    IGS_AVAILABLE = False

@dataclass
class IGSControlPoint:
    """Control point with weight for NURBS"""
    x: float
    y: float
    z: float
    weight: float = 1.0
    
    def to_vector4(self):
        """Convert to homogeneous coordinates"""
        return [self.x, self.y, self.z, self.weight]

@dataclass
class IGSNURBSCurve:
    """NURBS curve from IGS file"""
    degree: int
    control_points: List[IGSControlPoint]
    knots: List[float]
    entity_number: int
    name: str = ""

@dataclass
class IGSNURBSSurface:
    """NURBS surface from IGS file"""
    degree_u: int
    degree_v: int
    control_points: List[List[IGSControlPoint]]  # 2D grid
    knots_u: List[float]
    knots_v: List[float]
    entity_number: int
    name: str = ""

class IGSParser:
    """Parser for IGS/IGES files"""
    
    def __init__(self):
        self.entities = {}
        self.curves = []
        self.surfaces = []
        self.ignored_entities = []
        
    def parse_file(self, filename: str) -> bool:
        """Parse an IGS file and extract NURBS entities"""
        try:
            if IGS_AVAILABLE:
                return self._parse_with_library(filename)
            else:
                return self._parse_basic(filename)
        except Exception as e:
            print(f"Error parsing IGS file: {e}")
            return False
    
    def _parse_with_library(self, filename: str) -> bool:
        """Parse using python-iges library"""
        try:
            iges_file = iges.read(filename)
            
            for entity in iges_file.entities:
                entity_type = entity.entity_type
                
                if entity_type == 126:  # Rational B-Spline Curve
                    curve = self._parse_rational_bspline_curve(entity)
                    if curve:
                        self.curves.append(curve)
                        
                elif entity_type == 128:  # Rational B-Spline Surface
                    surface = self._parse_rational_bspline_surface(entity)
                    if surface:
                        self.surfaces.append(surface)
                        
                elif entity_type == 110:  # Line
                    # Convert line to simple curve
                    curve = self._parse_line_entity(entity)
                    if curve:
                        self.curves.append(curve)
                        
                elif entity_type == 120:  # Surface of Revolution
                    # Convert to NURBS surface
                    surface = self._parse_surface_of_revolution(entity)
                    if surface:
                        self.surfaces.append(surface)
                        
                else:
                    self.ignored_entities.append(f"Entity {entity.entity_type}: {entity.__class__.__name__}")
            
            return True
            
        except Exception as e:
            print(f"Error with python-iges: {e}")
            return False
    
    def _parse_basic(self, filename: str) -> bool:
        """Basic IGS parser for when python-iges is not available"""
        try:
            with open(filename, 'r') as f:
                lines = f.readlines()
            
            # Parse IGS file manually
            current_entity = None
            entity_data = []
            
            for line in lines:
                line = line.strip()
                if not line:
                    continue
                    
                if line.endswith('S'):  # Start section
                    continue
                elif line.endswith('G'):  # Global section
                    continue
                elif line.endswith('D'):  # Directory section
                    # Parse entity information
                    if len(line) >= 80:
                        entity_type = int(line[0:8].strip())
                        if entity_type in [126, 128, 110, 120]:  # NURBS entities
                            current_entity = {
                                'type': entity_type,
                                'number': int(line[8:16].strip()),
                                'data': []
                            }
                elif line.endswith('P'):  # Parameter section
                    if current_entity:
                        # Extract parameter data
                        param_data = line[:-1].strip()
                        current_entity['data'].append(param_data)
                        
                        # If this is the last parameter line, process the entity
                        if len(current_entity['data']) >= 2:
                            self._process_basic_entity(current_entity)
                            current_entity = None
                            
            return True
            
        except Exception as e:
            print(f"Error with basic parser: {e}")
            return False
    
    def _parse_rational_bspline_curve(self, entity) -> Optional[IGSNURBSCurve]:
        """Parse a rational B-spline curve entity (type 126)"""
        try:
            # Extract curve parameters
            degree = entity.degree
            num_control_points = entity.num_control_points
            control_points = []
            
            # Convert control points
            for i in range(num_control_points):
                cp = IGSControlPoint(
                    x=entity.control_points[i][0],
                    y=entity.control_points[i][1],
                    z=entity.control_points[i][2],
                    weight=entity.weights[i] if hasattr(entity, 'weights') else 1.0
                )
                control_points.append(cp)
            
            # Extract knots
            knots = list(entity.knots)
            
            return IGSNURBSCurve(
                degree=degree,
                control_points=control_points,
                knots=knots,
                entity_number=entity.entity_number,
                name=f"Curve_{entity.entity_number}"
            )
            
        except Exception as e:
            print(f"Error parsing B-spline curve: {e}")
            return None
    
    def _parse_rational_bspline_surface(self, entity) -> Optional[IGSNURBSSurface]:
        """Parse a rational B-spline surface entity (type 128)"""
        try:
            # Extract surface parameters
            degree_u = entity.degree_u
            degree_v = entity.degree_v
            num_u = entity.num_control_points_u
            num_v = entity.num_control_points_v
            
            # Convert control point grid
            control_points = []
            for i in range(num_u):
                row = []
                for j in range(num_v):
                    idx = i * num_v + j
                    cp = IGSControlPoint(
                        x=entity.control_points[idx][0],
                        y=entity.control_points[idx][1],
                        z=entity.control_points[idx][2],
                        weight=entity.weights[idx] if hasattr(entity, 'weights') else 1.0
                    )
                    row.append(cp)
                control_points.append(row)
            
            # Extract knot vectors
            knots_u = list(entity.knots_u)
            knots_v = list(entity.knots_v)
            
            return IGSNURBSSurface(
                degree_u=degree_u,
                degree_v=degree_v,
                control_points=control_points,
                knots_u=knots_u,
                knots_v=knots_v,
                entity_number=entity.entity_number,
                name=f"Surface_{entity.entity_number}"
            )
            
        except Exception as e:
            print(f"Error parsing B-spline surface: {e}")
            return None
    
    def _parse_line_entity(self, entity) -> Optional[IGSNURBSCurve]:
        """Convert a line entity to a simple NURBS curve"""
        try:
            # Create a linear NURBS curve from start to end point
            start = entity.start_point
            end = entity.end_point
            
            # Create control points
            control_points = [
                IGSControlPoint(start[0], start[1], start[2]),
                IGSControlPoint(end[0], end[1], end[2])
            ]
            
            # Create knot vector for linear curve
            knots = [0.0, 0.0, 1.0, 1.0]
            
            return IGSNURBSCurve(
                degree=1,
                control_points=control_points,
                knots=knots,
                entity_number=entity.entity_number,
                name=f"Line_{entity.entity_number}"
            )
            
        except Exception as e:
            print(f"Error parsing line entity: {e}")
            return None
    
    def _parse_surface_of_revolution(self, entity) -> Optional[IGSNURBSSurface]:
        """Convert a surface of revolution to NURBS surface"""
        try:
            # This is a simplified conversion - in practice would need more complex math
            # For now, create a basic cylindrical surface
            
            # Extract axis and generatrix
            axis_point = entity.axis_point
            axis_direction = entity.axis_direction
            generatrix = entity.generatrix
            
            # Create a simple cylindrical surface approximation
            # This is a placeholder - would need proper mathematical conversion
            control_points = []
            knots_u = [0.0, 0.0, 0.0, 1.0, 1.0, 1.0]
            knots_v = [0.0, 0.0, 1.0, 1.0]
            
            # Create basic control point grid (simplified)
            for i in range(3):
                row = []
                for j in range(2):
                    angle = i * math.pi / 2
                    radius = 1.0
                    cp = IGSControlPoint(
                        x=radius * math.cos(angle),
                        y=radius * math.sin(angle),
                        z=j
                    )
                    row.append(cp)
                control_points.append(row)
            
            return IGSNURBSSurface(
                degree_u=2,
                degree_v=1,
                control_points=control_points,
                knots_u=knots_u,
                knots_v=knots_v,
                entity_number=entity.entity_number,
                name=f"Revolution_{entity.entity_number}"
            )
            
        except Exception as e:
            print(f"Error parsing surface of revolution: {e}")
            return None
    
    def _process_basic_entity(self, entity_info):
        """Process entity data from basic parser"""
        entity_type = entity_info['type']
        entity_number = entity_info['number']
        
        if entity_type == 126:  # Rational B-Spline Curve
            # Parse basic curve data
            # This is simplified - would need proper parameter parsing
            pass
        elif entity_type == 128:  # Rational B-Spline Surface
            # Parse basic surface data
            pass
    
    def get_summary(self) -> Dict:
        """Get summary of parsed entities"""
        return {
            'curves': len(self.curves),
            'surfaces': len(self.surfaces),
            'ignored_entities': len(self.ignored_entities),
            'ignored_types': self.ignored_entities
        }
    
    def convert_to_editor_format(self) -> Dict:
        """Convert parsed IGS entities to editor-compatible format"""
        result = {
            'objects': [],
            'metadata': {
                'source': 'IGS',
                'curves': len(self.curves),
                'surfaces': len(self.surfaces)
            }
        }
        
        # Convert curves
        for curve in self.curves:
            obj = {
                'name': curve.name,
                'type': 'nurbs_curve',
                'degree': curve.degree,
                'control_points': [cp.to_vector4() for cp in curve.control_points],
                'knots': curve.knots,
                'position': [0.0, 0.0, 0.0],
                'rotation': [0.0, 0.0, 0.0],
                'scale': [1.0, 1.0, 1.0]
            }
            result['objects'].append(obj)
        
        # Convert surfaces
        for surface in self.surfaces:
            obj = {
                'name': surface.name,
                'type': 'nurbs_surface',
                'degree_u': surface.degree_u,
                'degree_v': surface.degree_v,
                'control_points': [[cp.to_vector4() for cp in row] for row in surface.control_points],
                'knots_u': surface.knots_u,
                'knots_v': surface.knots_v,
                'position': [0.0, 0.0, 0.0],
                'rotation': [0.0, 0.0, 0.0],
                'scale': [1.0, 1.0, 1.0]
            }
            result['objects'].append(obj)
        
        return result