#!/usr/bin/env python3
"""
NURBS Map Editor Launcher
Convenience script to run the map editor with proper path setup
"""

import sys
import os
sys.path.insert(0, os.path.join(os.path.dirname(__file__), 'map_editor'))

from map_editor.nurbs_editor import NURBSMapEditor

if __name__ == "__main__":
    print("Starting NURBS Map Editor...")
    print("A Roblox Studio-like editor for NURBS-based FPS games")
    print("=" * 50)
    
    try:
        editor = NURBSMapEditor()
        editor.run()
    except KeyboardInterrupt:
        print("\nEditor closed by user.")
    except Exception as e:
        print(f"Error starting editor: {e}")
        print("Make sure you have installed the required dependencies:")
        print("  cd map_editor && pip install -r requirements.txt")
        sys.exit(1)