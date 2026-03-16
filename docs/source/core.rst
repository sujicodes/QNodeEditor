Core
====

The foundational classes that underpin the node editor's data model,
serialization system, theming, and node registration.

Serializable
------------

Base class providing unique ID generation and a JSON serialization interface
for all persistable objects in the editor.

.. doxygenclass:: Serializable
   :members:
   :undoc-members:
   :protected-members:

----

NodeItem
--------

The base class for all nodes in the graph. Manages sockets, connected edges,
embedded widgets, and the dirty/invalid evaluation state system.

.. doxygenclass:: NodeItem
   :members:
   :undoc-members:
   :protected-members:

----

Edge
----

Represents a logical connection between two sockets, managing both the
data relationship and the visual path item.

.. doxygenclass:: Edge
   :members:
   :undoc-members:

----

SocketItem
----------

A connection point on a NodeItem. Can be an input or output, and holds
references to all edges currently connected to it.

.. doxygenclass:: SocketItem
   :members:
   :undoc-members:

----

NodeRegistry
------------

A singleton factory registry that maps type name strings to node constructors,
enabling dynamic node creation by name at runtime.

.. doxygenclass:: NodeRegistry
   :members:
   :undoc-members:

.. doxygendefine:: REGISTER_NODE

----

Theme
-----

A singleton that loads and exposes colour and display settings from a JSON
file, applied consistently across all visual components of the editor.

.. doxygenclass:: Theme
   :members:
   :undoc-members:
