#ifndef SOCKETITEM_H
#define SOCKETITEM_H

#include <QGraphicsItem>
#include <QList>
#include <QPen>
#include <QBrush>
#include "Serializable.h"

class NodeItem;
class Edge;

/**
 * @brief Represents a connection point (socket) on a NodeItem.
 *
 * SocketItem is the visual and logical endpoint for edges in the node editor.
 * Each socket belongs to a single NodeItem and is either an input or an output.
 * Sockets can hold one or more connected edges depending on the allowMultiEdges flag.
 *
 * Position constants (LEFT_TOP, RIGHT_CENTER, etc.) define where on the node
 * boundary the socket is rendered, and are used by NodeItem::getSocketPosition()
 * to calculate the socket's scene coordinates for edge rendering.
 *
 * SocketItem implements Serializable so its state (type, index, position, edges)
 * can be saved and restored as part of full scene serialization.
 *
 * @see NodeItem
 * @see Edge
 */
class SocketItem : public QGraphicsItem, public Serializable
{
public:

    /**
     * @brief Socket is positioned at the top of the left side of the node.
     */
    static const int LEFT_TOP = 1;

    /**
     * @brief Socket is positioned at the centre of the left side of the node.
     */
    static const int LEFT_CENTER = 2;

    /**
     * @brief Socket is positioned at the bottom of the left side of the node.
     */
    static const int LEFT_BOTTOM = 3;

    /**
     * @brief Socket is positioned at the top of the right side of the node.
     */
    static const int RIGHT_TOP = 4;

    /**
     * @brief Socket is positioned at the centre of the right side of the node.
     */
    static const int RIGHT_CENTER = 5;

    /**
     * @brief Socket is positioned at the bottom of the right side of the node.
     */
    static const int RIGHT_BOTTOM = 6;

    /**
     * @brief Socket type constant indicating this socket accepts incoming edges (input).
     */
    static const int INPUT = 6;

    /**
     * @brief Socket type constant indicating this socket emits outgoing edges (output).
     */
    static const int OUTPUT = 8;

    /**
     * @brief Constructs a SocketItem and attaches it to the given node.
     *
     * @param node            The NodeItem this socket belongs to. Must not be null.
     * @param type            The socket type: INPUT or OUTPUT.
     * @param index           The zero-based index of this socket within its type list
     *                        on the parent node. Used for vertical positioning.
     * @param position        The positional alignment constant (e.g. LEFT_CENTER, RIGHT_CENTER)
     *                        defining which side of the node the socket appears on.
     * @param allowMultiEdges If true, this socket can hold multiple simultaneous edge
     *                        connections. If false, connecting a new edge replaces the existing one.
     */
    SocketItem(NodeItem* node, int type, int index = 0, int position = LEFT_TOP,
               bool allowMultiEdges = false);

    /**
     * @brief Returns this socket as a non-const pointer.
     *
     * Convenience accessor used in contexts that require a mutable SocketItem*
     * from a const reference.
     *
     * @return A non-const pointer to this SocketItem.
     */
    SocketItem* getGraphicsSocket() const { return const_cast<SocketItem*>(this); }

    /**
     * @brief Whether this socket permits multiple simultaneous edge connections.
     *
     * When true, addEdge() appends without removing existing edges.
     * When false, adding a new edge first removes any existing one.
     */
    bool allowedMultiEdges = false;

    /**
     * @brief Sets the socket type to INPUT or OUTPUT.
     * @param type The new socket type constant.
     */
    void setSocketType(int type) { m_socketType = type; }

    /**
     * @brief Returns whether this socket is an input socket.
     * @return True if m_socketType == INPUT, false otherwise.
     */
    bool isInput() const { return m_socketType == INPUT; }

    /**
     * @brief Adds an edge to this socket's connection list.
     *
     * If allowedMultiEdges is false and an edge is already connected,
     * the existing edge is removed before the new one is added.
     *
     * @param edge The edge to connect. Must not be null.
     */
    void addEdge(Edge* edge);

    /**
     * @brief Removes a specific edge from this socket's connection list.
     *
     * Does nothing if the edge is not currently connected to this socket.
     *
     * @param edge The edge to disconnect.
     */
    void removeEdge(Edge* edge);

    /**
     * @brief Removes and disconnects all edges from this socket.
     *
     * Calls Edge::removeFromSockets() on each connected edge before
     * clearing the internal list.
     */
    void removeAllEdges();

    /**
     * @brief Returns whether this socket has at least one connected edge.
     * @return True if m_edges is non-empty, false otherwise.
     */
    bool hasConnectedEdge() const;

    /**
     * @brief Returns all edges currently connected to this socket.
     * @return A QList of Edge pointers.
     */
    QList<Edge*> getConnectedEdges() const;

    /**
     * @brief Returns the parent node this socket belongs to.
     * @return Pointer to the owning NodeItem.
     */
    NodeItem* getNode() const { return m_node; }

    /**
     * @brief Returns the zero-based index of this socket within its type list.
     *
     * Used by NodeItem::getSocketPosition() to calculate the vertical
     * offset of this socket relative to its neighbours.
     *
     * @return The socket index.
     */
    int getIndex() const { return m_index; }

    /**
     * @brief Returns the positional alignment constant for this socket.
     * @return One of the position constants (LEFT_TOP, RIGHT_CENTER, etc.).
     */
    int getPosition() const { return m_position; }

    /**
     * @brief Sets the positional alignment of this socket on the node boundary.
     * @param pos One of the position constants (LEFT_TOP, RIGHT_CENTER, etc.).
     */
    void setPosition(int pos) { m_position = pos; }

    /**
     * @brief Returns the current scene position of this socket's centre point.
     *
     * Delegates to NodeItem::getSocketPosition() using this socket's index,
     * position, and type, then maps the result to scene coordinates.
     *
     * @return The socket centre position in scene coordinates as a QPointF.
     */
    QPointF getSocketPosition() const;

    /**
     * @brief Updates this socket's local position within the parent node.
     *
     * Should be called by NodeItem::updateSockets() after the node is
     * resized or the socket list changes.
     */
    void updateSocketPosition();

    /**
     * @brief Serializes this socket's state to a JSON object.
     *
     * Captures the socket's ID, type, index, position, and the IDs of
     * all connected edges for save/load and clipboard operations.
     *
     * @return A QJsonObject representing the full socket state.
     */
    QJsonObject serialize() const override;

    /**
     * @brief Deserializes this socket's state from a JSON object.
     *
     * Restores type, index, position, and resolves connected edge
     * references via the provided hashmap.
     *
     * @param data       The JSON object containing serialized socket data.
     * @param hashmap    A map of original IDs to live Serializable pointers,
     *                   used to resolve edge references by ID.
     * @param restoreId  If true, restores the original unique ID of this socket.
     */
    void deserialize(const QJsonObject& data,
                     std::unordered_map<qint64, Serializable*>& hashmap,
                     bool restoreId = true) override;

    /**
     * @brief Returns the bounding rectangle of the socket in local coordinates.
     *
     * Sized to encompass the circular socket graphic including outline width.
     *
     * @return A QRectF centred on the socket's local origin.
     */
    QRectF boundingRect() const override;

    /**
     * @brief Paints the socket as a filled circle with an outline.
     *
     * Uses m_brush for the fill and m_pen for the outline. Colour may
     * vary based on socket type or connection state.
     *
     * @param painter The QPainter to draw with.
     * @param option  Style options (selection state, etc.).
     * @param widget  The widget being painted on (may be null).
     */
    void paint(QPainter* painter, const QStyleOptionGraphicsItem* option,
               QWidget* widget = nullptr) override;

private:

    /// @brief The NodeItem this socket is attached to.
    NodeItem* m_node = nullptr;

    /// @brief Zero-based index of this socket within its input or output list on the parent node.
    int m_index = 0;

    /// @brief Positional alignment constant defining which side and vertical alignment on the node.
    int m_position;

    /// @brief Socket type: INPUT or OUTPUT.
    int m_socketType;

    /// @brief All edges currently connected to this socket.
    QList<Edge*> m_edges;

    /// @brief Radius of the circular socket graphic in scene units.
    qreal m_radius = 6.0;

    /// @brief Width of the outline drawn around the socket circle.
    qreal m_outlineWidth = 1.0;

    /// @brief Pen used to draw the socket outline.
    QPen m_pen;

    /// @brief Brush used to fill the socket circle.
    QBrush m_brush;
};

#endif // SOCKETITEM_H