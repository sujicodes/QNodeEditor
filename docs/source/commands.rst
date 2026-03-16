Undo Commands
=============

All undoable operations in QNodeEditor are implemented as ``QUndoCommand``
subclasses. Each command captures enough state to fully undo and redo
the operation, including serialized node and edge data where needed.

SelectionChangedCommand
-----------------------

Records the before and after selection state, allowing undo/redo to cycle
between the two sets of selected nodes and edges.

.. doxygenclass:: SelectionChangedCommand
   :members:
   :undoc-members:

----

CreateNodeCommand
-----------------

Creates a node at a given scene position. On first execution constructs
the node via ``NodeRegistry``; subsequent redo calls restore from serialized state.

.. doxygenclass:: CreateNodeCommand
   :members:
   :undoc-members:

----

CreateEdgeCommand
-----------------

Finalises a drag-to-connect operation, removing the temporary drag edge and
any conflicting edges, and creating the permanent connection between two sockets.

.. doxygenclass:: CreateEdgeCommand
   :members:
   :undoc-members:

----

MoveNodeCommand
---------------

Records the start and end positions of one or more moved nodes, restoring
or replaying positions and updating connected edges on undo/redo.

.. doxygenclass:: MoveNodeCommand
   :members:
   :undoc-members:

----

DeleteSelectedCommand
---------------------

Serializes and removes all selected nodes and their connected edges.
Undo fully reconstructs nodes first, then restores edges via the socket hashmap.

.. doxygenclass:: DeleteSelectedCommand
   :members:
   :undoc-members:

----

PasteCommand
------------

Pastes nodes and edges from clipboard JSON data, centring the pasted content
on the current mouse position. Supports accurate re-pasting on subsequent redo calls.

.. doxygenclass:: PasteCommand
   :members:
   :undoc-members:

----

CutCommand
----------

Removes selected nodes and edges from the scene after serializing their state.
Undo reconstructs nodes first, then restores edges in the correct dependency order.

.. doxygenclass:: CutCommand
   :members:
   :undoc-members:
