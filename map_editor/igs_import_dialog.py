#!/usr/bin/env python3
"""
IGS Import Dialog for NURBS Map Editor
Provides a user interface for importing IGS files
"""

import tkinter as tk
from tkinter import ttk, filedialog, messagebox
from typing import List, Dict, Optional, Callable
import os
from igs_parser import IGSParser

class IGSImportDialog:
    """Dialog for importing IGS files"""
    
    def __init__(self, parent, on_import_callback: Optional[Callable] = None):
        self.parent = parent
        self.on_import_callback = on_import_callback
        self.parser = IGSParser()
        self.imported_objects = []
        
        # Create dialog window
        self.dialog = tk.Toplevel(parent)
        self.dialog.title("Import IGS File")
        self.dialog.geometry("600x500")
        self.dialog.resizable(True, True)
        self.dialog.transient(parent)
        self.dialog.grab_set()
        
        # Center dialog on parent
        self.dialog.geometry("+%d+%d" % (
            parent.winfo_rootx() + 50,
            parent.winfo_rooty() + 50
        ))
        
        self._create_widgets()
        self._layout_widgets()
        
    def _create_widgets(self):
        """Create all dialog widgets"""
        # File selection frame
        self.file_frame = ttk.LabelFrame(self.dialog, text="File Selection", padding="10")
        
        self.file_path_var = tk.StringVar()
        self.file_entry = ttk.Entry(self.file_frame, textvariable=self.file_path_var, width=50)
        self.browse_button = ttk.Button(self.file_frame, text="Browse...", command=self._browse_file)
        
        # Import options frame
        self.options_frame = ttk.LabelFrame(self.dialog, text="Import Options", padding="10")
        
        self.import_curves_var = tk.BooleanVar(value=True)
        self.import_surfaces_var = tk.BooleanVar(value=True)
        self.import_lines_var = tk.BooleanVar(value=True)
        self.import_revolutions_var = tk.BooleanVar(value=True)
        
        self.curves_check = ttk.Checkbutton(
            self.options_frame, 
            text="Import NURBS Curves", 
            variable=self.import_curves_var
        )
        self.surfaces_check = ttk.Checkbutton(
            self.options_frame, 
            text="Import NURBS Surfaces", 
            variable=self.import_surfaces_var
        )
        self.lines_check = ttk.Checkbutton(
            self.options_frame, 
            text="Import Lines (as curves)", 
            variable=self.import_lines_var
        )
        self.revolutions_check = ttk.Checkbutton(
            self.options_frame, 
            text="Import Surfaces of Revolution", 
            variable=self.import_revolutions_var
        )
        
        # Preview frame
        self.preview_frame = ttk.LabelFrame(self.dialog, text="File Preview", padding="10")
        
        # Create treeview for preview
        self.preview_tree = ttk.Treeview(self.preview_frame, columns=("Type", "Details"), show="tree headings")
        self.preview_tree.heading("#0", text="Entity")
        self.preview_tree.heading("Type", text="Type")
        self.preview_tree.heading("Details", text="Details")
        
        # Scrollbar for preview
        self.preview_scrollbar = ttk.Scrollbar(self.preview_frame, orient="vertical", command=self.preview_tree.yview)
        self.preview_tree.configure(yscrollcommand=self.preview_scrollbar.set)
        
        # Status frame
        self.status_frame = ttk.Frame(self.dialog)
        self.status_label = ttk.Label(self.status_frame, text="Ready to import")
        
        # Buttons frame
        self.button_frame = ttk.Frame(self.dialog)
        self.parse_button = ttk.Button(self.button_frame, text="Parse File", command=self._parse_file)
        self.import_button = ttk.Button(self.button_frame, text="Import Objects", command=self._import_objects, state="disabled")
        self.cancel_button = ttk.Button(self.button_frame, text="Cancel", command=self._cancel)
        
    def _layout_widgets(self):
        """Layout all widgets in the dialog"""
        # File selection
        self.file_frame.pack(fill="x", padx=10, pady=5)
        self.file_entry.pack(side="left", fill="x", expand=True, padx=(0, 5))
        self.browse_button.pack(side="right")
        
        # Import options
        self.options_frame.pack(fill="x", padx=10, pady=5)
        self.curves_check.pack(anchor="w")
        self.surfaces_check.pack(anchor="w")
        self.lines_check.pack(anchor="w")
        self.revolutions_check.pack(anchor="w")
        
        # Preview
        self.preview_frame.pack(fill="both", expand=True, padx=10, pady=5)
        self.preview_tree.pack(side="left", fill="both", expand=True)
        self.preview_scrollbar.pack(side="right", fill="y")
        
        # Status
        self.status_frame.pack(fill="x", padx=10, pady=5)
        self.status_label.pack(side="left")
        
        # Buttons
        self.button_frame.pack(fill="x", padx=10, pady=10)
        self.parse_button.pack(side="left")
        self.import_button.pack(side="left", padx=(5, 0))
        self.cancel_button.pack(side="right")
        
    def _browse_file(self):
        """Browse for IGS file"""
        filename = filedialog.askopenfilename(
            title="Select IGS File",
            filetypes=[
                ("IGS Files", "*.igs"),
                ("IGES Files", "*.iges"),
                ("All Files", "*.*")
            ]
        )
        if filename:
            self.file_path_var.set(filename)
            self.status_label.config(text=f"Selected: {os.path.basename(filename)}")
            
    def _parse_file(self):
        """Parse the selected IGS file"""
        filename = self.file_path_var.get()
        if not filename:
            messagebox.showwarning("Warning", "Please select an IGS file first.")
            return
            
        if not os.path.exists(filename):
            messagebox.showerror("Error", "Selected file does not exist.")
            return
            
        try:
            self.status_label.config(text="Parsing IGS file...")
            self.dialog.update()
            
            # Parse the file
            success = self.parser.parse_file(filename)
            
            if success:
                # Get summary and update preview
                summary = self.parser.get_summary()
                self._update_preview(summary)
                
                # Convert to editor format
                self.imported_objects = self.parser.convert_to_editor_format()
                
                self.status_label.config(
                    text=f"Parsed successfully: {summary['curves']} curves, {summary['surfaces']} surfaces"
                )
                self.import_button.config(state="normal")
                
            else:
                messagebox.showerror("Error", "Failed to parse IGS file. Check file format.")
                self.status_label.config(text="Parse failed")
                
        except Exception as e:
            messagebox.showerror("Error", f"Error parsing file: {str(e)}")
            self.status_label.config(text="Parse error")
            
    def _update_preview(self, summary: Dict):
        """Update the preview tree with parsed entities"""
        # Clear existing items
        for item in self.preview_tree.get_children():
            self.preview_tree.delete(item)
            
        # Add curves
        if summary['curves'] > 0:
            curves_item = self.preview_tree.insert("", "end", text="NURBS Curves", values=(f"{summary['curves']} curves", ""))
            for i, curve in enumerate(self.parser.curves):
                self.preview_tree.insert(curves_item, "end", 
                    text=curve.name,
                    values=("Curve", f"Degree: {curve.degree}, Points: {len(curve.control_points)}")
                )
                
        # Add surfaces
        if summary['surfaces'] > 0:
            surfaces_item = self.preview_tree.insert("", "end", text="NURBS Surfaces", values=(f"{summary['surfaces']} surfaces", ""))
            for i, surface in enumerate(self.parser.surfaces):
                self.preview_tree.insert(surfaces_item, "end",
                    text=surface.name,
                    values=("Surface", f"Degree: ({surface.degree_u}, {surface.degree_v})")
                )
                
        # Add ignored entities
        if summary['ignored_entities'] > 0:
            ignored_item = self.preview_tree.insert("", "end", text="Ignored Entities", values=(f"{summary['ignored_entities']} entities", ""))
            for ignored in summary['ignored_types']:
                self.preview_tree.insert(ignored_item, "end", text=ignored, values=("Ignored", ""))
                
    def _import_objects(self):
        """Import the parsed objects into the editor"""
        if not self.imported_objects or not self.imported_objects['objects']:
            messagebox.showwarning("Warning", "No objects to import.")
            return
            
        try:
            # Call the callback with imported objects
            if self.on_import_callback:
                self.on_import_callback(self.imported_objects)
                
            messagebox.showinfo("Success", f"Successfully imported {len(self.imported_objects['objects'])} objects.")
            self.dialog.destroy()
            
        except Exception as e:
            messagebox.showerror("Error", f"Error importing objects: {str(e)}")
            
    def _cancel(self):
        """Cancel the import operation"""
        self.dialog.destroy()
        
    def show(self):
        """Show the dialog and wait for user interaction"""
        self.dialog.wait_window()
        return self.imported_objects if hasattr(self, 'imported_objects') else None