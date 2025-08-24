#!/usr/bin/env python3
"""
NURBS Map Editor - A Roblox Studio-like editor for NURBS-based FPS games
"""

import tkinter as tk
from tkinter import ttk, filedialog, messagebox, colorchooser
import numpy as np
import json
import math
from typing import List, Dict, Tuple, Optional
from dataclasses import dataclass, asdict
from enum import Enum

# OpenGL imports for 3D viewport
try:
    from OpenGL.GL import *
    from OpenGL.GLU import *
    import pygame
    from pygame.locals import *
    OPENGL_AVAILABLE = True
except ImportError:
    print("Warning: OpenGL/Pygame not available. 3D viewport will be disabled.")
    OPENGL_AVAILABLE = False

class ObjectType(Enum):
    SPHERE = "sphere"
    PLANE = "plane" 
    CYLINDER = "cylinder"
    TORUS = "torus"
    CUSTOM = "custom"

class LightType(Enum):
    POINT = 0
    DIRECTIONAL = 1
    SPOT = 2

@dataclass
class Vector3:
    x: float = 0.0
    y: float = 0.0
    z: float = 0.0
    
    def __iter__(self):
        return iter([self.x, self.y, self.z])
    
    def __add__(self, other):
        return Vector3(self.x + other.x, self.y + other.y, self.z + other.z)
    
    def __sub__(self, other):
        return Vector3(self.x - other.x, self.y - other.y, self.z - other.z)
    
    def __mul__(self, scalar):
        return Vector3(self.x * scalar, self.y * scalar, self.z * scalar)

@dataclass
class Material:
    ambient: Vector3 = None
    diffuse: Vector3 = None
    specular: Vector3 = None
    shininess: float = 32.0
    
    def __post_init__(self):
        if self.ambient is None:
            self.ambient = Vector3(0.2, 0.2, 0.2)
        if self.diffuse is None:
            self.diffuse = Vector3(0.8, 0.8, 0.8)
        if self.specular is None:
            self.specular = Vector3(1.0, 1.0, 1.0)

@dataclass
class Light:
    name: str = "Light"
    position: Vector3 = None
    color: Vector3 = None
    intensity: float = 1.0
    light_type: LightType = LightType.POINT
    direction: Vector3 = None
    spot_angle: float = 45.0
    
    def __post_init__(self):
        if self.position is None:
            self.position = Vector3(0.0, 5.0, 0.0)
        if self.color is None:
            self.color = Vector3(1.0, 1.0, 1.0)
        if self.direction is None:
            self.direction = Vector3(0.0, -1.0, 0.0)

@dataclass
class NURBSObject:
    name: str = "Object"
    object_type: ObjectType = ObjectType.SPHERE
    position: Vector3 = None
    rotation: Vector3 = None
    scale: Vector3 = None
    material: Material = None
    is_collidable: bool = True
    parameters: Dict = None  # Type-specific parameters
    
    def __post_init__(self):
        if self.position is None:
            self.position = Vector3(0.0, 0.0, 0.0)
        if self.rotation is None:
            self.rotation = Vector3(0.0, 0.0, 0.0)
        if self.scale is None:
            self.scale = Vector3(1.0, 1.0, 1.0)
        if self.material is None:
            self.material = Material()
        if self.parameters is None:
            self.parameters = {}

class NURBSMapEditor:
    def __init__(self):
        self.root = tk.Tk()
        self.root.title("NURBS Map Editor")
        self.root.geometry("1400x900")
        
        # Editor state
        self.objects: List[NURBSObject] = []
        self.lights: List[Light] = []
        self.selected_object: Optional[NURBSObject] = None
        self.selected_light: Optional[Light] = None
        
        # Initialize NURBS renderer
        try:
            from nurbs_renderer import NURBSRenderer
            self.nurbs_renderer = NURBSRenderer()
        except ImportError:
            self.nurbs_renderer = None
            print("Warning: NURBS renderer not available")
        
        self.current_tool = "select"
        self.camera_pos = Vector3(0, 5, 10)
        self.camera_rotation = Vector3(0, 0, 0)
        
        # Initialize UI
        self.create_menu()
        self.create_main_layout()
        self.create_toolbars()
        self.create_property_panel()
        self.create_scene_hierarchy()
        self.create_viewport()
        
        # Initialize 3D viewport if available
        if OPENGL_AVAILABLE:
            self.init_3d_viewport()
        
        self.update_ui()
    
    def create_menu(self):
        menubar = tk.Menu(self.root)
        self.root.config(menu=menubar)
        
        # File menu
        file_menu = tk.Menu(menubar, tearoff=0)
        menubar.add_cascade(label="File", menu=file_menu)
        file_menu.add_command(label="New", command=self.new_scene)
        file_menu.add_command(label="Open...", command=self.open_scene)
        file_menu.add_command(label="Save", command=self.save_scene)
        file_menu.add_command(label="Save As...", command=self.save_scene_as)
        file_menu.add_separator()
        file_menu.add_command(label="Import IGS...", command=self.import_igs)
        file_menu.add_separator()
        file_menu.add_command(label="Export for Game...", command=self.export_for_game)
        file_menu.add_separator()
        file_menu.add_command(label="Exit", command=self.root.quit)
        
        # Edit menu
        edit_menu = tk.Menu(menubar, tearoff=0)
        menubar.add_cascade(label="Edit", menu=edit_menu)
        edit_menu.add_command(label="Undo", command=self.undo)
        edit_menu.add_command(label="Redo", command=self.redo)
        edit_menu.add_separator()
        edit_menu.add_command(label="Delete Selected", command=self.delete_selected)
        edit_menu.add_command(label="Duplicate Selected", command=self.duplicate_selected)
        
        # Insert menu
        insert_menu = tk.Menu(menubar, tearoff=0)
        menubar.add_cascade(label="Insert", menu=insert_menu)
        insert_menu.add_command(label="NURBS Sphere", 
                               command=lambda: self.add_object(ObjectType.SPHERE))
        insert_menu.add_command(label="NURBS Plane", 
                               command=lambda: self.add_object(ObjectType.PLANE))
        insert_menu.add_command(label="NURBS Cylinder", 
                               command=lambda: self.add_object(ObjectType.CYLINDER))
        insert_menu.add_command(label="NURBS Torus", 
                               command=lambda: self.add_object(ObjectType.TORUS))
        insert_menu.add_separator()
        insert_menu.add_command(label="Point Light", 
                               command=lambda: self.add_light(LightType.POINT))
        insert_menu.add_command(label="Directional Light", 
                               command=lambda: self.add_light(LightType.DIRECTIONAL))
        insert_menu.add_command(label="Spot Light", 
                               command=lambda: self.add_light(LightType.SPOT))
        
        # View menu
        view_menu = tk.Menu(menubar, tearoff=0)
        menubar.add_cascade(label="View", menu=view_menu)
        view_menu.add_command(label="Reset Camera", command=self.reset_camera)
        view_menu.add_command(label="Focus Selected", command=self.focus_selected)
        view_menu.add_separator()
        view_menu.add_command(label="Wireframe", command=self.toggle_wireframe)
        view_menu.add_command(label="Show Grid", command=self.toggle_grid)
        view_menu.add_command(label="Show Lighting", command=self.toggle_lighting)
    
    def create_main_layout(self):
        # Main paned window
        self.main_paned = ttk.PanedWindow(self.root, orient=tk.HORIZONTAL)
        self.main_paned.pack(fill=tk.BOTH, expand=True)
        
        # Left panel for scene hierarchy and properties
        self.left_panel = ttk.Frame(self.main_paned, width=300)
        self.main_paned.add(self.left_panel, weight=0)
        
        # Center panel for viewport
        self.center_panel = ttk.Frame(self.main_paned)
        self.main_paned.add(self.center_panel, weight=1)
        
        # Right panel for properties
        self.right_panel = ttk.Frame(self.main_paned, width=300)
        self.main_paned.add(self.right_panel, weight=0)
    
    def create_toolbars(self):
        # Main toolbar
        toolbar_frame = ttk.Frame(self.center_panel)
        toolbar_frame.pack(side=tk.TOP, fill=tk.X, padx=5, pady=5)
        
        # Tool buttons
        tools = [
            ("Select", "select", self.set_select_tool),
            ("Move", "move", self.set_move_tool),
            ("Rotate", "rotate", self.set_rotate_tool),
            ("Scale", "scale", self.set_scale_tool),
        ]
        
        self.tool_buttons = {}
        for name, tool_id, command in tools:
            btn = ttk.Button(toolbar_frame, text=name, command=command)
            btn.pack(side=tk.LEFT, padx=2)
            self.tool_buttons[tool_id] = btn
        
        # Separator
        ttk.Separator(toolbar_frame, orient=tk.VERTICAL).pack(side=tk.LEFT, padx=10, fill=tk.Y)
        
        # View controls
        ttk.Button(toolbar_frame, text="Top View", 
                  command=lambda: self.set_view("top")).pack(side=tk.LEFT, padx=2)
        ttk.Button(toolbar_frame, text="Front View", 
                  command=lambda: self.set_view("front")).pack(side=tk.LEFT, padx=2)
        ttk.Button(toolbar_frame, text="Side View", 
                  command=lambda: self.set_view("side")).pack(side=tk.LEFT, padx=2)
        ttk.Button(toolbar_frame, text="Perspective", 
                  command=lambda: self.set_view("perspective")).pack(side=tk.LEFT, padx=2)
    
    def create_scene_hierarchy(self):
        # Scene hierarchy panel
        hierarchy_frame = ttk.LabelFrame(self.left_panel, text="Scene Hierarchy")
        hierarchy_frame.pack(fill=tk.BOTH, expand=True, padx=5, pady=5)
        
        # Tree view for scene objects
        self.scene_tree = ttk.Treeview(hierarchy_frame, selectmode="browse")
        self.scene_tree.pack(fill=tk.BOTH, expand=True, padx=5, pady=5)
        
        # Tree columns
        self.scene_tree["columns"] = ("type", "visible")
        self.scene_tree.column("#0", width=150, minwidth=100)
        self.scene_tree.column("type", width=80, minwidth=60)
        self.scene_tree.column("visible", width=50, minwidth=40)
        
        self.scene_tree.heading("#0", text="Name", anchor=tk.W)
        self.scene_tree.heading("type", text="Type", anchor=tk.W)
        self.scene_tree.heading("visible", text="Visible", anchor=tk.W)
        
        # Bind selection
        self.scene_tree.bind("<<TreeviewSelect>>", self.on_tree_select)
        
        # Context menu
        self.tree_context_menu = tk.Menu(self.scene_tree, tearoff=0)
        self.tree_context_menu.add_command(label="Rename", command=self.rename_selected)
        self.tree_context_menu.add_command(label="Duplicate", command=self.duplicate_selected)
        self.tree_context_menu.add_command(label="Delete", command=self.delete_selected)
        
        self.scene_tree.bind("<Button-3>", self.show_tree_context_menu)
    
    def create_property_panel(self):
        # Properties panel
        prop_frame = ttk.LabelFrame(self.right_panel, text="Properties")
        prop_frame.pack(fill=tk.BOTH, expand=True, padx=5, pady=5)
        
        # Create notebook for different property categories
        self.prop_notebook = ttk.Notebook(prop_frame)
        self.prop_notebook.pack(fill=tk.BOTH, expand=True, padx=5, pady=5)
        
        # Transform properties
        self.transform_frame = ttk.Frame(self.prop_notebook)
        self.prop_notebook.add(self.transform_frame, text="Transform")
        
        # Material properties
        self.material_frame = ttk.Frame(self.prop_notebook)
        self.prop_notebook.add(self.material_frame, text="Material")
        
        # Light properties
        self.light_frame = ttk.Frame(self.prop_notebook)
        self.prop_notebook.add(self.light_frame, text="Lighting")
        
        self.create_transform_properties()
        self.create_material_properties()
        self.create_light_properties()
    
    def create_transform_properties(self):
        # Position
        pos_frame = ttk.LabelFrame(self.transform_frame, text="Position")
        pos_frame.pack(fill=tk.X, padx=5, pady=5)
        
        self.pos_vars = {}
        for i, axis in enumerate(['X', 'Y', 'Z']):
            ttk.Label(pos_frame, text=f"{axis}:").grid(row=0, column=i*2, padx=2, sticky=tk.W)
            var = tk.DoubleVar()
            entry = ttk.Entry(pos_frame, textvariable=var, width=8)
            entry.grid(row=0, column=i*2+1, padx=2)
            entry.bind("<Return>", self.update_transform)
            self.pos_vars[axis.lower()] = var
        
        # Rotation
        rot_frame = ttk.LabelFrame(self.transform_frame, text="Rotation")
        rot_frame.pack(fill=tk.X, padx=5, pady=5)
        
        self.rot_vars = {}
        for i, axis in enumerate(['X', 'Y', 'Z']):
            ttk.Label(rot_frame, text=f"{axis}:").grid(row=0, column=i*2, padx=2, sticky=tk.W)
            var = tk.DoubleVar()
            entry = ttk.Entry(rot_frame, textvariable=var, width=8)
            entry.grid(row=0, column=i*2+1, padx=2)
            entry.bind("<Return>", self.update_transform)
            self.rot_vars[axis.lower()] = var
        
        # Scale
        scale_frame = ttk.LabelFrame(self.transform_frame, text="Scale")
        scale_frame.pack(fill=tk.X, padx=5, pady=5)
        
        self.scale_vars = {}
        for i, axis in enumerate(['X', 'Y', 'Z']):
            ttk.Label(scale_frame, text=f"{axis}:").grid(row=0, column=i*2, padx=2, sticky=tk.W)
            var = tk.DoubleVar(value=1.0)
            entry = ttk.Entry(scale_frame, textvariable=var, width=8)
            entry.grid(row=0, column=i*2+1, padx=2)
            entry.bind("<Return>", self.update_transform)
            self.scale_vars[axis.lower()] = var
    
    def create_material_properties(self):
        # Ambient color
        ambient_frame = ttk.LabelFrame(self.material_frame, text="Ambient Color")
        ambient_frame.pack(fill=tk.X, padx=5, pady=5)
        
        self.ambient_color_btn = tk.Button(ambient_frame, text="Choose Color", 
                                          command=lambda: self.choose_color("ambient"))
        self.ambient_color_btn.pack(pady=5)
        
        # Diffuse color
        diffuse_frame = ttk.LabelFrame(self.material_frame, text="Diffuse Color")
        diffuse_frame.pack(fill=tk.X, padx=5, pady=5)
        
        self.diffuse_color_btn = tk.Button(diffuse_frame, text="Choose Color",
                                          command=lambda: self.choose_color("diffuse"))
        self.diffuse_color_btn.pack(pady=5)
        
        # Specular color
        specular_frame = ttk.LabelFrame(self.material_frame, text="Specular Color")
        specular_frame.pack(fill=tk.X, padx=5, pady=5)
        
        self.specular_color_btn = tk.Button(specular_frame, text="Choose Color",
                                           command=lambda: self.choose_color("specular"))
        self.specular_color_btn.pack(pady=5)
        
        # Shininess
        shininess_frame = ttk.LabelFrame(self.material_frame, text="Shininess")
        shininess_frame.pack(fill=tk.X, padx=5, pady=5)
        
        self.shininess_var = tk.DoubleVar(value=32.0)
        shininess_scale = ttk.Scale(shininess_frame, from_=1.0, to=128.0, 
                                   variable=self.shininess_var, orient=tk.HORIZONTAL)
        shininess_scale.pack(fill=tk.X, padx=5, pady=5)
        shininess_scale.bind("<Motion>", self.update_material)
    
    def create_light_properties(self):
        # Light type
        type_frame = ttk.LabelFrame(self.light_frame, text="Light Type")
        type_frame.pack(fill=tk.X, padx=5, pady=5)
        
        self.light_type_var = tk.StringVar(value="Point")
        light_types = ["Point", "Directional", "Spot"]
        for light_type in light_types:
            ttk.Radiobutton(type_frame, text=light_type, variable=self.light_type_var,
                           value=light_type, command=self.update_light).pack(anchor=tk.W)
        
        # Light color
        color_frame = ttk.LabelFrame(self.light_frame, text="Light Color")
        color_frame.pack(fill=tk.X, padx=5, pady=5)
        
        self.light_color_btn = tk.Button(color_frame, text="Choose Color",
                                        command=lambda: self.choose_light_color())
        self.light_color_btn.pack(pady=5)
        
        # Intensity
        intensity_frame = ttk.LabelFrame(self.light_frame, text="Intensity")
        intensity_frame.pack(fill=tk.X, padx=5, pady=5)
        
        self.intensity_var = tk.DoubleVar(value=1.0)
        intensity_scale = ttk.Scale(intensity_frame, from_=0.0, to=5.0,
                                   variable=self.intensity_var, orient=tk.HORIZONTAL)
        intensity_scale.pack(fill=tk.X, padx=5, pady=5)
        intensity_scale.bind("<Motion>", self.update_light)
    
    def create_viewport(self):
        # Viewport frame
        viewport_frame = ttk.LabelFrame(self.center_panel, text="3D Viewport")
        viewport_frame.pack(fill=tk.BOTH, expand=True, padx=5, pady=5)
        
        if OPENGL_AVAILABLE:
            # Create pygame/OpenGL viewport
            self.create_opengl_viewport(viewport_frame)
        else:
            # Create simple canvas viewport
            self.viewport_canvas = tk.Canvas(viewport_frame, bg="gray20")
            self.viewport_canvas.pack(fill=tk.BOTH, expand=True, padx=5, pady=5)
            
            # Bind mouse events
            self.viewport_canvas.bind("<Button-1>", self.on_viewport_click)
            self.viewport_canvas.bind("<B1-Motion>", self.on_viewport_drag)
            self.viewport_canvas.bind("<MouseWheel>", self.on_viewport_scroll)
    
    def create_opengl_viewport(self, parent):
        # Embed pygame/OpenGL in tkinter (simplified approach)
        self.viewport_frame = tk.Frame(parent)
        self.viewport_frame.pack(fill=tk.BOTH, expand=True, padx=5, pady=5)
        
        # Status label for now (full OpenGL integration would be complex)
        status_label = tk.Label(self.viewport_frame, 
                               text="3D OpenGL Viewport\n(Use mouse to navigate)\n" +
                                    "Left: Rotate | Middle: Pan | Wheel: Zoom",
                               bg="gray20", fg="white", font=("Arial", 12))
        status_label.pack(expand=True)
    
    def init_3d_viewport(self):
        # Initialize 3D viewport rendering (simplified)
        self.viewport_initialized = True
        self.wireframe_mode = False
        self.show_grid = True
        self.show_lighting = True
    
    # Tool methods
    def set_select_tool(self):
        self.current_tool = "select"
        self.update_tool_buttons()
    
    def set_move_tool(self):
        self.current_tool = "move"
        self.update_tool_buttons()
    
    def set_rotate_tool(self):
        self.current_tool = "rotate"
        self.update_tool_buttons()
    
    def set_scale_tool(self):
        self.current_tool = "scale"
        self.update_tool_buttons()
    
    def update_tool_buttons(self):
        # Update tool button states
        for tool_id, button in self.tool_buttons.items():
            if tool_id == self.current_tool:
                button.state(['pressed'])
            else:
                button.state(['!pressed'])
    
    # Object management
    def add_object(self, object_type: ObjectType):
        name = f"{object_type.value.title()}_{len(self.objects) + 1}"
        
        # Set default parameters based on type
        parameters = {}
        if object_type == ObjectType.SPHERE:
            parameters = {"radius": 1.0}
        elif object_type == ObjectType.PLANE:
            parameters = {"width": 2.0, "height": 2.0}
        elif object_type == ObjectType.CYLINDER:
            parameters = {"radius": 1.0, "height": 2.0}
        elif object_type == ObjectType.TORUS:
            parameters = {"major_radius": 1.0, "minor_radius": 0.3}
        
        obj = NURBSObject(
            name=name,
            object_type=object_type,
            position=Vector3(0, 0, 0),
            parameters=parameters
        )
        
        self.objects.append(obj)
        self.selected_object = obj
        self.update_scene_tree()
        self.update_properties()
    
    def add_light(self, light_type: LightType):
        name = f"{light_type.name.title()}_Light_{len(self.lights) + 1}"
        
        light = Light(
            name=name,
            light_type=light_type,
            position=Vector3(0, 5, 0)
        )
        
        self.lights.append(light)
        self.selected_light = light
        self.selected_object = None
        self.update_scene_tree()
        self.update_properties()
    
    def delete_selected(self):
        if self.selected_object:
            self.objects.remove(self.selected_object)
            self.selected_object = None
        elif self.selected_light:
            self.lights.remove(self.selected_light)
            self.selected_light = None
        
        self.update_scene_tree()
        self.update_properties()
    
    def duplicate_selected(self):
        if self.selected_object:
            # Create a copy
            import copy
            new_obj = copy.deepcopy(self.selected_object)
            new_obj.name = f"{new_obj.name}_copy"
            new_obj.position = new_obj.position + Vector3(2, 0, 0)
            self.objects.append(new_obj)
            self.selected_object = new_obj
        elif self.selected_light:
            import copy
            new_light = copy.deepcopy(self.selected_light)
            new_light.name = f"{new_light.name}_copy"
            new_light.position = new_light.position + Vector3(2, 0, 0)
            self.lights.append(new_light)
            self.selected_light = new_light
        
        self.update_scene_tree()
        self.update_properties()
    
    # UI update methods
    def update_scene_tree(self):
        # Clear tree
        for item in self.scene_tree.get_children():
            self.scene_tree.delete(item)
        
        # Add objects
        for obj in self.objects:
            item_id = self.scene_tree.insert("", tk.END, text=obj.name,
                                            values=(obj.object_type.value, "✓"))
            # Store object reference
            self.scene_tree.set(item_id, "object", obj)
        
        # Add lights
        for light in self.lights:
            item_id = self.scene_tree.insert("", tk.END, text=light.name,
                                            values=(light.light_type.name.lower(), "✓"))
            self.scene_tree.set(item_id, "light", light)
    
    def update_properties(self):
        if self.selected_object:
            self.update_transform_ui()
            self.update_material_ui()
        elif self.selected_light:
            self.update_light_ui()
    
    def update_transform_ui(self):
        if not self.selected_object:
            return
        
        obj = self.selected_object
        self.pos_vars['x'].set(obj.position.x)
        self.pos_vars['y'].set(obj.position.y)
        self.pos_vars['z'].set(obj.position.z)
        
        self.rot_vars['x'].set(obj.rotation.x)
        self.rot_vars['y'].set(obj.rotation.y)
        self.rot_vars['z'].set(obj.rotation.z)
        
        self.scale_vars['x'].set(obj.scale.x)
        self.scale_vars['y'].set(obj.scale.y)
        self.scale_vars['z'].set(obj.scale.z)
    
    def update_material_ui(self):
        if not self.selected_object:
            return
        
        mat = self.selected_object.material
        self.shininess_var.set(mat.shininess)
        
        # Update color button backgrounds
        self.ambient_color_btn.configure(
            bg=f"#{int(mat.ambient.x*255):02x}{int(mat.ambient.y*255):02x}{int(mat.ambient.z*255):02x}")
        self.diffuse_color_btn.configure(
            bg=f"#{int(mat.diffuse.x*255):02x}{int(mat.diffuse.y*255):02x}{int(mat.diffuse.z*255):02x}")
        self.specular_color_btn.configure(
            bg=f"#{int(mat.specular.x*255):02x}{int(mat.specular.y*255):02x}{int(mat.specular.z*255):02x}")
    
    def update_light_ui(self):
        if not self.selected_light:
            return
        
        light = self.selected_light
        self.light_type_var.set(light.light_type.name.title())
        self.intensity_var.set(light.intensity)
        
        # Update light color button
        self.light_color_btn.configure(
            bg=f"#{int(light.color.x*255):02x}{int(light.color.y*255):02x}{int(light.color.z*255):02x}")
    
    # Event handlers
    def on_tree_select(self, event):
        selection = self.scene_tree.selection()
        if not selection:
            return
        
        item = selection[0]
        
        # Check if it's an object or light
        if self.scene_tree.set(item, "object"):
            self.selected_object = self.scene_tree.set(item, "object")
            self.selected_light = None
        elif self.scene_tree.set(item, "light"):
            self.selected_light = self.scene_tree.set(item, "light")
            self.selected_object = None
        
        self.update_properties()
    
    def update_transform(self, event=None):
        if not self.selected_object:
            return
        
        obj = self.selected_object
        obj.position.x = self.pos_vars['x'].get()
        obj.position.y = self.pos_vars['y'].get()
        obj.position.z = self.pos_vars['z'].get()
        
        obj.rotation.x = self.rot_vars['x'].get()
        obj.rotation.y = self.rot_vars['y'].get()
        obj.rotation.z = self.rot_vars['z'].get()
        
        obj.scale.x = self.scale_vars['x'].get()
        obj.scale.y = self.scale_vars['y'].get()
        obj.scale.z = self.scale_vars['z'].get()
    
    def update_material(self, event=None):
        if not self.selected_object:
            return
        
        self.selected_object.material.shininess = self.shininess_var.get()
    
    def update_light(self, event=None):
        if not self.selected_light:
            return
        
        light = self.selected_light
        light_type_map = {
            "Point": LightType.POINT,
            "Directional": LightType.DIRECTIONAL,
            "Spot": LightType.SPOT
        }
        light.light_type = light_type_map[self.light_type_var.get()]
        light.intensity = self.intensity_var.get()
    
    def choose_color(self, color_type):
        if not self.selected_object:
            return
        
        color = colorchooser.askcolor(title=f"Choose {color_type} color")
        if color[0]:  # If user didn't cancel
            rgb = [c/255.0 for c in color[0]]
            
            if color_type == "ambient":
                self.selected_object.material.ambient = Vector3(*rgb)
            elif color_type == "diffuse":
                self.selected_object.material.diffuse = Vector3(*rgb)
            elif color_type == "specular":
                self.selected_object.material.specular = Vector3(*rgb)
            
            self.update_material_ui()
    
    def choose_light_color(self):
        if not self.selected_light:
            return
        
        color = colorchooser.askcolor(title="Choose light color")
        if color[0]:
            rgb = [c/255.0 for c in color[0]]
            self.selected_light.color = Vector3(*rgb)
            self.update_light_ui()
    
    # File operations
    def new_scene(self):
        self.objects.clear()
        self.lights.clear()
        self.selected_object = None
        self.selected_light = None
        self.update_scene_tree()
        self.update_properties()
    
    def save_scene(self):
        # Convert scene to JSON format
        scene_data = {
            "objects": [asdict(obj) for obj in self.objects],
            "lights": [asdict(light) for light in self.lights],
            "camera": {
                "position": asdict(self.camera_pos),
                "rotation": asdict(self.camera_rotation)
            }
        }
        
        filename = filedialog.asksaveasfilename(
            defaultextension=".json",
            filetypes=[("JSON files", "*.json"), ("All files", "*.*")],
            title="Save Scene"
        )
        
        if filename:
            try:
                with open(filename, 'w') as f:
                    json.dump(scene_data, f, indent=2, default=str)
                messagebox.showinfo("Success", "Scene saved successfully!")
            except Exception as e:
                messagebox.showerror("Error", f"Failed to save scene: {str(e)}")
    
    def save_scene_as(self):
        self.save_scene()
    
    def open_scene(self):
        filename = filedialog.askopenfilename(
            filetypes=[("JSON files", "*.json"), ("All files", "*.*")],
            title="Open Scene"
        )
        
        if filename:
            try:
                with open(filename, 'r') as f:
                    scene_data = json.load(f)
                
                # Clear current scene
                self.new_scene()
                
                # Load objects
                for obj_data in scene_data.get("objects", []):
                    obj = NURBSObject(**obj_data)
                    self.objects.append(obj)
                
                # Load lights
                for light_data in scene_data.get("lights", []):
                    light = Light(**light_data)
                    self.lights.append(light)
                
                self.update_scene_tree()
                messagebox.showinfo("Success", "Scene loaded successfully!")
                
            except Exception as e:
                messagebox.showerror("Error", f"Failed to load scene: {str(e)}")
    
    def export_for_game(self):
        """Export scene in format compatible with C game engine"""
        if not self.objects and not self.lights:
            messagebox.showwarning("Warning", "No objects or lights to export!")
            return
        
        filename = filedialog.asksaveasfilename(
            defaultextension=".map",
            filetypes=[("Map files", "*.map"), ("All files", "*.*")],
            title="Export for Game"
        )
        
        if filename:
            try:
                # Create C-compatible export format
                export_data = {
                    "version": "1.0",
                    "objects": [],
                    "lights": []
                }
                
                # Export objects
                for obj in self.objects:
                    obj_export = {
                        "name": obj.name,
                        "type": obj.object_type.value,
                        "position": [obj.position.x, obj.position.y, obj.position.z],
                        "rotation": [obj.rotation.x, obj.rotation.y, obj.rotation.z],
                        "scale": [obj.scale.x, obj.scale.y, obj.scale.z],
                        "material": {
                            "ambient": [obj.material.ambient.x, obj.material.ambient.y, obj.material.ambient.z],
                            "diffuse": [obj.material.diffuse.x, obj.material.diffuse.y, obj.material.diffuse.z],
                            "specular": [obj.material.specular.x, obj.material.specular.y, obj.material.specular.z],
                            "shininess": obj.material.shininess
                        },
                        "collidable": obj.is_collidable,
                        "parameters": obj.parameters
                    }
                    export_data["objects"].append(obj_export)
                
                # Export lights
                for light in self.lights:
                    light_export = {
                        "name": light.name,
                        "type": light.light_type.value,
                        "position": [light.position.x, light.position.y, light.position.z],
                        "color": [light.color.x, light.color.y, light.color.z],
                        "intensity": light.intensity,
                        "direction": [light.direction.x, light.direction.y, light.direction.z],
                        "spot_angle": light.spot_angle
                    }
                    export_data["lights"].append(light_export)
                
                with open(filename, 'w') as f:
                    json.dump(export_data, f, indent=2)
                
                messagebox.showinfo("Success", f"Scene exported for game: {filename}")
                
            except Exception as e:
                messagebox.showerror("Error", f"Failed to export scene: {str(e)}")
    
    def import_igs(self):
        """Import IGS file and convert to NURBS objects"""
        try:
            from igs_import_dialog import IGSImportDialog
            dialog = IGSImportDialog(self.root, self.on_igs_import)
            dialog.show()
        except ImportError as e:
            messagebox.showerror("Error", f"Failed to import IGS module: {str(e)}")
        except Exception as e:
            messagebox.showerror("Error", f"Failed to open IGS import dialog: {str(e)}")
    
    def on_igs_import(self, imported_data: Dict):
        """Callback for when IGS objects are imported"""
        try:
            imported_count = 0
            
            for obj_data in imported_data.get('objects', []):
                if obj_data['type'] == 'nurbs_curve':
                    # Create NURBS curve object
                    obj = NURBSObject(
                        name=obj_data['name'],
                        object_type=ObjectType.CUSTOM,
                        position=Vector3(*obj_data['position']),
                        rotation=Vector3(*obj_data['rotation']),
                        scale=Vector3(*obj_data['scale'])
                    )
                    obj.parameters = {
                        'nurbs_type': 'curve',
                        'degree': obj_data['degree'],
                        'control_points': obj_data['control_points'],
                        'knots': obj_data['knots']
                    }
                    self.objects.append(obj)
                    imported_count += 1
                    
                elif obj_data['type'] == 'nurbs_surface':
                    # Create NURBS surface object
                    obj = NURBSObject(
                        name=obj_data['name'],
                        object_type=ObjectType.CUSTOM,
                        position=Vector3(*obj_data['position']),
                        rotation=Vector3(*obj_data['rotation']),
                        scale=Vector3(*obj_data['scale'])
                    )
                    obj.parameters = {
                        'nurbs_type': 'surface',
                        'degree_u': obj_data['degree_u'],
                        'degree_v': obj_data['degree_v'],
                        'control_points': obj_data['control_points'],
                        'knots_u': obj_data['knots_u'],
                        'knots_v': obj_data['knots_v']
                    }
                    self.objects.append(obj)
                    imported_count += 1
            
            if imported_count > 0:
                self.update_scene_tree()
                messagebox.showinfo("Success", f"Successfully imported {imported_count} IGS objects!")
            else:
                messagebox.showwarning("Warning", "No valid objects found in IGS file.")
                
        except Exception as e:
            messagebox.showerror("Error", f"Failed to import IGS objects: {str(e)}")
    
    def render_igs_objects(self):
        """Render imported IGS NURBS objects using the NURBS renderer"""
        if not self.nurbs_renderer or not OPENGL_AVAILABLE:
            return
            
        for obj in self.objects:
            if obj.object_type == ObjectType.CUSTOM and 'nurbs_type' in obj.parameters:
                nurbs_type = obj.parameters['nurbs_type']
                
                if nurbs_type == 'curve':
                    # Render NURBS curve
                    control_points = obj.parameters['control_points']
                    knots = obj.parameters['knots']
                    degree = obj.parameters['degree']
                    
                    # Apply object transformation
                    glPushMatrix()
                    glTranslatef(obj.position.x, obj.position.y, obj.position.z)
                    glRotatef(obj.rotation.x, 1, 0, 0)
                    glRotatef(obj.rotation.y, 0, 1, 0)
                    glRotatef(obj.rotation.z, 0, 0, 1)
                    glScalef(obj.scale.x, obj.scale.y, obj.scale.z)
                    
                    # Render the curve
                    self.nurbs_renderer.render_nurbs_curve(control_points, knots, degree)
                    
                    glPopMatrix()
                    
                elif nurbs_type == 'surface':
                    # Render NURBS surface
                    control_points = obj.parameters['control_points']
                    knots_u = obj.parameters['knots_u']
                    knots_v = obj.parameters['knots_v']
                    degree_u = obj.parameters['degree_u']
                    degree_v = obj.parameters['degree_v']
                    
                    # Apply object transformation
                    glPushMatrix()
                    glTranslatef(obj.position.x, obj.position.y, obj.position.z)
                    glRotatef(obj.rotation.x, 1, 0, 0)
                    glRotatef(obj.rotation.y, 0, 1, 0)
                    glRotatef(obj.rotation.z, 0, 0, 1)
                    glScalef(obj.scale.x, obj.scale.y, obj.scale.z)
                    
                    # Render the surface
                    self.nurbs_renderer.render_nurbs_surface(
                        control_points, knots_u, knots_v, degree_u, degree_v
                    )
                    
                    glPopMatrix()
    
    # Placeholder methods for menu items
    def undo(self): pass
    def redo(self): pass
    def rename_selected(self): pass
    def reset_camera(self): pass
    def focus_selected(self): pass
    def toggle_wireframe(self): pass
    def toggle_grid(self): pass
    def toggle_lighting(self): pass
    def set_view(self, view_type): pass
    def show_tree_context_menu(self, event): pass
    def on_viewport_click(self, event): pass
    def on_viewport_drag(self, event): pass
    def on_viewport_scroll(self, event): pass
    
    def update_ui(self):
        self.update_tool_buttons()
        self.update_scene_tree()
        self.update_properties()
    
    def run(self):
        self.root.mainloop()

if __name__ == "__main__":
    editor = NURBSMapEditor()
    editor.run()