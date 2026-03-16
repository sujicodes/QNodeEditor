#ifndef EDGE_H
#define EDGE_H

#include "NodeEditorGraphicsScene.h"
#include "Serializable.h"

class Scene;
class SocketItem;
class EdgeGraphicsPathItem;
class QJsonObject;

/**
 * @brief Represents a connection (edge) between two sockets in the node editor.
 *
 * An Edge connects a start socket to an end socket, managing the logical
 * connection between nodes as well as the visual representation via
 * EdgeGraphicsPathItem. It supports serialization for save/load and
 * undo/redo workflows.
 *
 * @see SocketItem
 * @see EdgeGraphicsPathItem
 * @see NodeEditorGraphicsScene
 */
class Edge : public Serializable
{
public:

    /**
     * @brief Constructs an Edge and adds it to the given scene.
     *
     * @param scene       The scene this edge belongs to. Must not be null.
     * @param startSocket The socket at the start (output) end of the edge. Can be null initially.
     * @param endSocket   The socket at the end (input) end of the edge. Can be null initially.
     */
    Edge(NodeEditorGraphicsScene* scene, SocketItem* startSocket = nullptr, SocketItem* endSocket = nullptr);

    /**
     * @brief Destructor. Cleans up the visual graphics item associated with this edge.
     */
    ~Edge();

    /**
     * @brief Returns the scene this edge belongs to.
     * @return Pointer to the owning NodeEditorGraphicsScene.
     */
    NodeEditorGraphicsScene* getScene() const { return m_scene; }

    /**
     * @brief Returns the visual graphics item representing this edge.
     * @return Pointer to the EdgeGraphicsPathItem used for rendering.
     */
    EdgeGraphicsPathItem* getEdgeGraphicsItem() const { return m_grEdge; }

    /**
     * @brief Sets the end socket of this edge.
     *
     * Updates the socket's internal reference to this edge and triggers
     * a position update on the graphics item.
     *
     * @param socket The socket to connect to the end of this edge.
     */
    void setEndSocket(SocketItem* socket);

    /**
     * @brief Returns the end socket of this edge.
     * @return Pointer to the end SocketItem, or nullptr if not set.
     */
    SocketItem* getEndSocket() { return m_endSocket; }

    /**
     * @brief Sets the start socket of this edge.
     *
     * Updates the socket's internal reference to this edge and triggers
     * a position update on the graphics item.
     *
     * @param socket The socket to connect to the start of this edge.
     */
    void setStartSocket(SocketItem* socket);

    /**
     * @brief Returns the start socket of this edge.
     * @return Pointer to the start SocketItem, or nullptr if not set.
     */
    SocketItem* getStartSocket() { return m_startSocket; }

    /**
     * @brief Returns the socket on the opposite end from the given socket.
     *
     * Useful when you have a reference to one socket and need to find
     * the socket on the other end of the edge.
     *
     * @param known_socket The socket you already know.
     * @return The other socket. Returns m_startSocket if known_socket is
     *         m_endSocket, and vice versa.
     */
    SocketItem* getOtherSocket(SocketItem* known_socket) const
    {
        return (known_socket == m_endSocket) ? m_startSocket : m_endSocket;
    }

    /**
     * @brief Updates the visual position of the edge graphics item.
     *
     * Should be called whenever a connected node or socket has moved,
     * to keep the rendered path in sync with the socket positions.
     */
    void updatePositions();

    /**
     * @brief Detaches this edge from both its start and end sockets.
     *
     * Does not delete the edge — only clears the socket references on
     * both ends. Call before removing or reassigning the edge.
     */
    void removeFromSockets();

    /**
     * @brief Fully removes this edge from the scene and its sockets, then deletes it.
     *
     * Calls removeFromSockets() and removes the graphics item from the scene
     * before scheduling this object for deletion.
     */
    void remove();

    /**
     * @brief Serializes this edge to a JSON object.
     *
     * Captures the edge's unique ID and the IDs of the connected start
     * and end sockets, sufficient to reconstruct the connection on deserialization.
     *
     * @return A QJsonObject containing the serialized edge data.
     */
    QJsonObject serialize() const override;

    /**
     * @brief Deserializes this edge from a JSON object.
     *
     * Restores socket connections by looking up socket IDs in the provided
     * hashmap of already-deserialized Serializable objects.
     *
     * @param data       The JSON object containing serialized edge data.
     * @param hashmap    A map of IDs to Serializable pointers, used to resolve socket references.
     * @param restoreId  If true, restores the original unique ID of the edge.
     */
    void deserialize(
        const QJsonObject& data,
        std::unordered_map<qint64, Serializable*>& hashmap,
        bool restoreId = true
    ) override;

private:

    /// @brief The scene this edge belongs to.
    NodeEditorGraphicsScene* m_scene = nullptr;

    /// @brief The socket at the start (typically output) end of the edge.
    SocketItem* m_startSocket = nullptr;

    /// @brief The socket at the end (typically input) end of the edge.
    SocketItem* m_endSocket = nullptr;

    /// @brief The visual graphics item used to render this edge in the scene.
    EdgeGraphicsPathItem* m_grEdge;
};

#endif // EDGE_H