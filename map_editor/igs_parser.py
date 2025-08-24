#!/usr/bin/env python3
"""
IGS/IGES File Parser for NURBS Map Editor
Converts IGS entities to the editor's NURBS format
"""

import numpy as np
from typing import List, Dict, Tuple, Optional, Union
from dataclasses import dataclass
import math

# Try to import pyiges, fall back to basic parser if not available
try:
    import pyiges
    IGS_AVAILABLE = True
except ImportError:
    try:
        import iges
        IGS_AVAILABLE = True
    except ImportError:
        print("Warning: pyiges not available. Using basic IGS parser.")
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
        self.debug_info = []
        
    def parse_file(self, filename: str) -> bool:
        """Parse an IGS file and extract NURBS entities"""
        try:
            if IGS_AVAILABLE:
                return self._parse_with_library(filename)
            else:
                return self._parse_basic(filename)
        except Exception as e:
            print(f"Error parsing IGS file: {e}")
            self.debug_info.append(f"Parse error: {str(e)}")
            return False
    
    def _parse_with_library(self, filename: str) -> bool:
        """Parse using pyiges library"""
        try:
            # Try pyiges first, then fallback to iges
            if 'pyiges' in globals():
                iges_file = pyiges.read(filename)
                self.debug_info.append(f"Using pyiges library")
            else:
                iges_file = iges.read(filename)
                self.debug_info.append(f"Using iges library")
                
            # Handle different API structures
            if hasattr(iges_file, 'entities'):
                entities = iges_file.entities
            elif hasattr(iges_file, 'get_entities'):
                entities = iges_file.get_entities()
            elif callable(getattr(iges_file, 'entities', None)):
                entities = iges_file.entities()
            else:
                # Try to access entities as a property
                entities = iges_file.entities
                
            if entities is None:
                self.debug_info.append("No entities found in IGS file")
                return False
                
            # Get entity count safely
            try:
                entity_count = len(entities)
                self.debug_info.append(f"Total entities: {entity_count}")
            except TypeError:
                # If entities is not a list, try to iterate
                entity_count = 0
                temp_entities = []
                for entity in entities:
                    temp_entities.append(entity)
                    entity_count += 1
                entities = temp_entities
                self.debug_info.append(f"Total entities: {entity_count}")
            
            for entity in entities:
                try:
                    # Get entity type safely
                    if hasattr(entity, 'entity_type'):
                        entity_type = entity.entity_type
                    elif hasattr(entity, 'type'):
                        entity_type = entity.type
                    else:
                        entity_type = getattr(entity, 'type', 0)
                    
                    # Get entity number safely
                    if hasattr(entity, 'entity_number'):
                        entity_number = entity.entity_number
                    elif hasattr(entity, 'number'):
                        entity_number = entity.number
                    else:
                        entity_number = getattr(entity, 'number', 0)
                    
                    self.debug_info.append(f"Entity {entity_number}: Type {entity_type} ({entity.__class__.__name__})")
                    
                    if entity_type == 126:  # Rational B-Spline Curve
                        curve = self._parse_rational_bspline_curve(entity)
                        if curve:
                            self.curves.append(curve)
                            self.debug_info.append(f"  -> Parsed NURBS curve: {curve.name}")
                            
                    elif entity_type == 128:  # Rational B-Spline Surface
                        surface = self._parse_rational_bspline_surface(entity)
                        if surface:
                            self.surfaces.append(surface)
                            self.debug_info.append(f"  -> Parsed NURBS surface: {surface.name}")
                            
                    elif entity_type == 110:  # Line
                        # Convert line to simple curve
                        curve = self._parse_line_entity(entity)
                        if curve:
                            self.curves.append(curve)
                            self.debug_info.append(f"  -> Converted line to curve: {curve.name}")
                            
                    elif entity_type == 120:  # Surface of Revolution
                        # Convert to NURBS surface
                        surface = self._parse_surface_of_revolution(entity)
                        if surface:
                            self.surfaces.append(surface)
                            self.debug_info.append(f"  -> Converted revolution to surface: {surface.name}")
                            
                    elif entity_type == 100:  # Circular Arc
                        # Convert circular arc to NURBS curve
                        curve = self._parse_circular_arc(entity)
                        if curve:
                            self.curves.append(curve)
                            self.debug_info.append(f"  -> Converted circular arc to curve: {curve.name}")
                            
                    elif entity_type == 102:  # Composite Curve
                        # Parse composite curve
                        curves = self._parse_composite_curve(entity)
                        for curve in curves:
                            self.curves.append(curve)
                            self.debug_info.append(f"  -> Parsed composite curve component: {curve.name}")
                            
                    elif entity_type == 108:  # Plane
                        # Convert plane to NURBS surface
                        surface = self._parse_plane_entity(entity)
                        if surface:
                            self.surfaces.append(surface)
                            self.debug_info.append(f"  -> Converted plane to surface: {surface.name}")
                            
                    else:
                        self.ignored_entities.append(f"Entity {entity_type}: {entity.__class__.__name__}")
                        self.debug_info.append(f"  -> Ignored entity type {entity_type}")
                        
                except Exception as entity_error:
                    self.debug_info.append(f"  -> Error processing entity: {str(entity_error)}")
                    continue
            
            return True
            
        except Exception as e:
            print(f"Error with IGS library: {e}")
            self.debug_info.append(f"IGS library error: {str(e)}")
            return False
    
    def _parse_basic(self, filename: str) -> bool:
        """Basic IGS parser for when python-iges is not available"""
        try:
            with open(filename, 'r') as f:
                lines = f.readlines()
            
            self.debug_info.append(f"Using basic parser")
            self.debug_info.append(f"Total lines: {len(lines)}")
            
            # Parse IGS file manually
            current_entity = None
            entity_data = []
            line_count = 0
            
            for line in lines:
                line_count += 1
                line = line.strip()
                if not line:
                    continue
                    
                if line.endswith('S'):  # Start section
                    self.debug_info.append(f"Line {line_count}: Start section")
                    continue
                elif line.endswith('G'):  # Global section
                    self.debug_info.append(f"Line {line_count}: Global section")
                    continue
                elif line.endswith('D'):  # Directory section
                    # Parse entity information
                    if len(line) >= 80:
                        try:
                            entity_type = int(line[0:8].strip())
                            entity_number = int(line[8:16].strip())
                            self.debug_info.append(f"Line {line_count}: Entity {entity_number}, Type {entity_type}")
                            
                            if entity_type in [126, 128, 110, 120, 100, 102, 108]:  # NURBS entities
                                current_entity = {
                                    'type': entity_type,
                                    'number': entity_number,
                                    'data': []
                                }
                                self.debug_info.append(f"  -> Found NURBS entity: Type {entity_type}")
                        except ValueError:
                            self.debug_info.append(f"Line {line_count}: Could not parse entity info")
                            
                elif line.endswith('P'):  # Parameter section
                    if current_entity:
                        # Extract parameter data
                        param_data = line[:-1].strip()
                        current_entity['data'].append(param_data)
                        self.debug_info.append(f"Line {line_count}: Parameter data for entity {current_entity['number']}")
                        
                        # If this is the last parameter line, process the entity
                        if len(current_entity['data']) >= 2:
                            self._process_basic_entity(current_entity)
                            current_entity = None
                            
            return True
            
        except Exception as e:
            print(f"Error with basic parser: {e}")
            self.debug_info.append(f"Basic parser error: {str(e)}")
            return False
    
    def _parse_rational_bspline_curve(self, entity) -> Optional[IGSNURBSCurve]:
        """Parse a rational B-spline curve entity (type 126)"""
        try:
            # Extract curve parameters
            degree = getattr(entity, 'degree', 3)
            num_control_points = getattr(entity, 'num_control_points', 0)
            control_points = []
            
            self.debug_info.append(f"    Parsing B-spline curve: degree={degree}, points={num_control_points}")
            
            # Convert control points
            for i in range(num_control_points):
                cp_data = entity.control_points[i] if hasattr(entity, 'control_points') else [0, 0, 0]
                weight = entity.weights[i] if hasattr(entity, 'weights') and i < len(entity.weights) else 1.0
                
                cp = IGSControlPoint(
                    x=float(cp_data[0]) if len(cp_data) > 0 else 0.0,
                    y=float(cp_data[1]) if len(cp_data) > 1 else 0.0,
                    z=float(cp_data[2]) if len(cp_data) > 2 else 0.0,
                    weight=float(weight)
                )
                control_points.append(cp)
            
            # Extract knots
            knots = list(entity.knots) if hasattr(entity, 'knots') else [0.0, 0.0, 1.0, 1.0]
            
            self.debug_info.append(f"    Control points: {len(control_points)}, Knots: {len(knots)}")
            
            return IGSNURBSCurve(
                degree=degree,
                control_points=control_points,
                knots=knots,
                entity_number=entity.entity_number,
                name=f"Curve_{entity.entity_number}"
            )
            
        except Exception as e:
            print(f"Error parsing B-spline curve: {e}")
            self.debug_info.append(f"    Error parsing curve: {str(e)}")
            return None
    
    def _parse_rational_bspline_surface(self, entity) -> Optional[IGSNURBSSurface]:
        """Parse a rational B-spline surface entity (type 128)"""
        try:
            # Extract surface parameters
            degree_u = getattr(entity, 'degree_u', 3)
            degree_v = getattr(entity, 'degree_v', 3)
            num_u = getattr(entity, 'num_control_points_u', 0)
            num_v = getattr(entity, 'num_control_points_v', 0)
            
            self.debug_info.append(f"    Parsing B-spline surface: degree_u={degree_u}, degree_v={degree_v}, points_u={num_u}, points_v={num_v}")
            
            # Convert control point grid
            control_points = []
            for i in range(num_u):
                row = []
                for j in range(num_v):
                    idx = i * num_v + j
                    cp_data = entity.control_points[idx] if hasattr(entity, 'control_points') and idx < len(entity.control_points) else [0, 0, 0]
                    weight = entity.weights[idx] if hasattr(entity, 'weights') and idx < len(entity.weights) else 1.0
                    
                    cp = IGSControlPoint(
                        x=float(cp_data[0]) if len(cp_data) > 0 else 0.0,
                        y=float(cp_data[1]) if len(cp_data) > 1 else 0.0,
                        z=float(cp_data[2]) if len(cp_data) > 2 else 0.0,
                        weight=float(weight)
                    )
                    row.append(cp)
                control_points.append(row)
            
            # Extract knot vectors
            knots_u = list(entity.knots_u) if hasattr(entity, 'knots_u') else [0.0, 0.0, 1.0, 1.0]
            knots_v = list(entity.knots_v) if hasattr(entity, 'knots_v') else [0.0, 0.0, 1.0, 1.0]
            
            self.debug_info.append(f"    Control points grid: {len(control_points)}x{len(control_points[0]) if control_points else 0}")
            
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
            self.debug_info.append(f"    Error parsing surface: {str(e)}")
            return None
    
    def _parse_line_entity(self, entity) -> Optional[IGSNURBSCurve]:
        """Convert a line entity to a simple NURBS curve"""
        try:
            # Create a linear NURBS curve from start to end point
            start = getattr(entity, 'start_point', [0, 0, 0])
            end = getattr(entity, 'end_point', [1, 0, 0])
            
            self.debug_info.append(f"    Converting line: start={start}, end={end}")
            
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
            self.debug_info.append(f"    Error converting line: {str(e)}")
            return None
    
    def _parse_surface_of_revolution(self, entity) -> Optional[IGSNURBSSurface]:
        """Convert a surface of revolution to NURBS surface"""
        try:
            # This is a simplified conversion - in practice would need more complex math
            # For now, create a basic cylindrical surface
            
            self.debug_info.append(f"    Converting surface of revolution")
            
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
            self.debug_info.append(f"    Error converting revolution: {str(e)}")
            return None
    
    def _process_basic_entity(self, entity_info):
        """Process entity data from basic parser"""
        entity_type = entity_info['type']
        entity_number = entity_info['number']
        
        self.debug_info.append(f"    Processing basic entity: Type {entity_type}, Number {entity_number}")
        
        if entity_type == 126:  # Rational B-Spline Curve
            # Parse basic curve data
            # This is simplified - would need proper parameter parsing
            self.debug_info.append(f"    Basic parser: Found B-spline curve entity")
            pass
        elif entity_type == 128:  # Rational B-Spline Surface
            # Parse basic surface data
            self.debug_info.append(f"    Basic parser: Found B-spline surface entity")
            pass
    
    def _parse_circular_arc(self, entity) -> Optional[IGSNURBSCurve]:
        """Convert a circular arc entity to NURBS curve"""
        try:
            # Extract arc parameters
            center = getattr(entity, 'center', [0, 0, 0])
            start_angle = getattr(entity, 'start_angle', 0)
            end_angle = getattr(entity, 'end_angle', 2*math.pi)
            radius = getattr(entity, 'radius', 1.0)
            
            self.debug_info.append(f"    Converting circular arc: center={center}, radius={radius}, angles={start_angle}-{end_angle}")
            
            # Create NURBS curve approximation of the arc
            num_points = max(8, int(abs(end_angle - start_angle) * 4 / math.pi))
            control_points = []
            
            for i in range(num_points + 1):
                angle = start_angle + (end_angle - start_angle) * i / num_points
                x = center[0] + radius * math.cos(angle)
                y = center[1] + radius * math.sin(angle)
                z = center[2]
                control_points.append(IGSControlPoint(x, y, z))
            
            # Create knot vector for quadratic curve
            knots = [0.0] * 3 + [1.0] * 3
            
            return IGSNURBSCurve(
                degree=2,
                control_points=control_points,
                knots=knots,
                entity_number=entity.entity_number,
                name=f"Arc_{entity.entity_number}"
            )
            
        except Exception as e:
            print(f"Error parsing circular arc: {e}")
            self.debug_info.append(f"    Error converting arc: {str(e)}")
            return None
    
    def _parse_composite_curve(self, entity) -> List[IGSNURBSCurve]:
        """Parse a composite curve entity"""
        try:
            curves = []
            # This is a simplified implementation
            # In practice, would need to parse the constituent curves
            self.debug_info.append(f"    Parsing composite curve")
            
            # For now, create a simple placeholder curve
            control_points = [
                IGSControlPoint(0, 0, 0),
                IGSControlPoint(1, 0, 0),
                IGSControlPoint(1, 1, 0)
            ]
            
            curve = IGSNURBSCurve(
                degree=2,
                control_points=control_points,
                knots=[0.0, 0.0, 0.0, 1.0, 1.0, 1.0],
                entity_number=entity.entity_number,
                name=f"Composite_{entity.entity_number}"
            )
            curves.append(curve)
            
            return curves
            
        except Exception as e:
            print(f"Error parsing composite curve: {e}")
            self.debug_info.append(f"    Error parsing composite curve: {str(e)}")
            return []
    
    def _parse_plane_entity(self, entity) -> Optional[IGSNURBSSurface]:
        """Convert a plane entity to NURBS surface"""
        try:
            # Extract plane parameters
            point = getattr(entity, 'point', [0, 0, 0])
            normal = getattr(entity, 'normal', [0, 0, 1])
            
            self.debug_info.append(f"    Converting plane: point={point}, normal={normal}")
            
            # Create a simple rectangular surface
            size = 2.0
            control_points = []
            knots_u = [0.0, 0.0, 1.0, 1.0]
            knots_v = [0.0, 0.0, 1.0, 1.0]
            
            # Create 2x2 control point grid
            for i in range(2):
                row = []
                for j in range(2):
                    x = point[0] + (i - 0.5) * size
                    y = point[1] + (j - 0.5) * size
                    z = point[2]
                    row.append(IGSControlPoint(x, y, z))
                control_points.append(row)
            
            return IGSNURBSSurface(
                degree_u=1,
                degree_v=1,
                control_points=control_points,
                knots_u=knots_u,
                knots_v=knots_v,
                entity_number=entity.entity_number,
                name=f"Plane_{entity.entity_number}"
            )
            
        except Exception as e:
            print(f"Error parsing plane entity: {e}")
            self.debug_info.append(f"    Error converting plane: {str(e)}")
            return None
    
    def get_summary(self) -> Dict:
        """Get summary of parsed entities"""
        return {
            'curves': len(self.curves),
            'surfaces': len(self.surfaces),
            'ignored_entities': len(self.ignored_entities),
            'ignored_types': self.ignored_entities,
            'debug_info': self.debug_info
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