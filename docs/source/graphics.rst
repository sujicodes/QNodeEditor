Graphics
========

The visual layer of the node editor — scene, view, widget, and window classes
that handle rendering, user interaction, and application window management.

NodeEditorGraphicsScene
-----------------------

The core scene class managing all nodes, edges, undo history, serialization,
selection tracking, and background grid rendering.

.. doxygenclass:: NodeEditorGraphicsScene
   :members:
   :undoc-members:
   :protected-members:

----

NodeEditorGraphicsView
----------------------

The main viewport providing zoom, pan, edge dragging, node drag-and-drop,
multi-node move tracking, and selection handling.

.. doxygenclass:: NodeEditorGraphicsView
   :members:
   :undoc-members:
   :protected-members:

----

EdgeGraphicsPathItem
--------------------

Abstract base class for the visual representation of an edge. Subclass and
implement ``calcPath()`` to define bezier, direct, or custom curve styles.

.. doxygenclass:: EdgeGraphicsPathItem
   :members:
   :undoc-members:
   :protected-members:

----

NodeEditorWidget
----------------

A self-contained QWidget combining the scene and view, with a high-level
API for file operations, selection queries, and undo/redo state.

.. doxygenclass:: NodeEditorWidget
   :members:
   :undoc-members:

----

NodeEditorWindow
----------------

The main application window providing a full menu bar with File and Edit
actions, status bar mouse position display, and unsaved-changes prompting.

.. doxygenclass:: NodeEditorWindow
   :members:
   :undoc-members:
   :protected-members:
