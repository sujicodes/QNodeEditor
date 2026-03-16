#ifndef UNDOCOMMANDS_H
#define UNDOCOMMANDS_H

#include <QUndoCommand>
#include <QGraphicsItem>
#include <QList>
#include <QJsonObject>
#include <QJsonArray>
#include <QPointF>
#include <unordered_map>
#include <vector>
#include <limits>
#include <QPointer>
#include <QSet>
#include <QMap>

#include "NodeEditorGraphicsScene.h"
#include "SocketItem.h"
#include "Edge.h"
#include "NodeItem.h"
#include "NodeEditorGraphicsView.h"
#include "EdgeGraphicsPathItem.h"
#include "Serializable.h"
#include "NodeRegistry.h"

/**
 * @brief Undo command that restores a previous selection state in the scene.
 *
 * Captures the set of selected node and edge IDs before and after a selection
 * change, allowing undo/redo to cycle between the two states. Signals are
 * blocked during selection restoration to prevent recursive command creation.
 *
 * @see NodeEditorGraphicsScene
 */
class SelectionChangedCommand : public QUndoCommand
{
public:

    /**
     * @brief Constructs the command with the old and new selection states.
     *
     * @param scene      The scene whose selection is being tracked.
     * @param oldNodeIds The set of node IDs that were selected before the change.
     * @param oldEdgeIds The set of edge IDs that were selected before the change.
     * @param newNodeIds The set of node IDs selected after the change.
     * @param newEdgeIds The set of edge IDs selected after the change.
     * @param parent     Optional parent undo command.
     */
    SelectionChangedCommand(NodeEditorGraphicsScene* scene,
                            const QSet<qint64>& oldNodeIds,
                            const QSet<qint64>& oldEdgeIds,
                            const QSet<qint64>& newNodeIds,
                            const QSet<qint64>& newEdgeIds,
                            QUndoCommand* parent = nullptr)
        : QUndoCommand("Selection Changed", parent),
          m_scene(scene),
          m_oldNodeIds(oldNodeIds),
          m_oldEdgeIds(oldEdgeIds),
          m_newNodeIds(newNodeIds),
          m_newEdgeIds(newEdgeIds)
    {}

    /**
     * @brief Restores the selection state that existed before the change.
     */
    void undo() override { applySelection(m_oldNodeIds, m_oldEdgeIds); }

    /**
     * @brief Applies the new selection state.
     */
    void redo() override { applySelection(m_newNodeIds, m_newEdgeIds); }

private:

    /**
     * @brief Applies a given selection state to the scene.
     *
     * Clears the current selection, then selects all nodes and edges
     * whose IDs appear in the provided sets. Scene signals are blocked
     * during the operation to prevent re-entrant command creation.
     *
     * @param nodeIds The set of node IDs to select.
     * @param edgeIds The set of edge IDs to select.
     */
    void applySelection(const QSet<qint64>& nodeIds, const QSet<qint64>& edgeIds)
    {
        if (!m_scene) return;
        QGraphicsScene* grScene = m_scene;
        if (!grScene) return;

        grScene->blockSignals(true);
        grScene->clearSelection();

        for (auto id : nodeIds) {
            NodeItem* node = m_scene->getNodeById(id);
            if (node)
                node->setSelected(true);
        }

        for (auto id : edgeIds) {
            Edge* edge = m_scene->getEdgeById(id);
            if (edge && edge->getEdgeGraphicsItem())
                edge->getEdgeGraphicsItem()->setSelected(true);
        }

        grScene->blockSignals(false);
    }

    /// @brief The scene whose selection is managed by this command.
    NodeEditorGraphicsScene* m_scene = nullptr;

    /// @brief Node IDs selected before the change (restored on undo).
    QSet<qint64> m_oldNodeIds;

    /// @brief Edge IDs selected before the change (restored on undo).
    QSet<qint64> m_oldEdgeIds;

    /// @brief Node IDs selected after the change (applied on redo).
    QSet<qint64> m_newNodeIds;

    /// @brief Edge IDs selected after the change (applied on redo).
    QSet<qint64> m_newEdgeIds;
};


/**
 * @brief Undo command that creates a node at a given scene position.
 *
 * On first execution (redo), creates the node via NodeRegistry and serializes
 * its state. On subsequent redo calls (after an undo), restores the node from
 * the serialized state so its ID and socket configuration are preserved.
 *
 * @see NodeEditorGraphicsScene
 * @see NodeRegistry
 * @see NodeItem
 */
class CreateNodeCommand : public QUndoCommand
{
public:

    /**
     * @brief Constructs the command with the node type and spawn position.
     *
     * @param scene    The scene in which the node will be created.
     * @param nodeType The registered type name of the node (e.g. "AddNode").
     * @param position The scene position at which the node will be placed.
     * @param parent   Optional parent undo command.
     */
    CreateNodeCommand(NodeEditorGraphicsScene* scene,
                      const QString& nodeType,
                      const QPointF& position,
                      QUndoCommand* parent = nullptr)
        : QUndoCommand("Create Node", parent),
          m_scene(scene),
          m_nodeType(nodeType),
          m_position(position)
    {}

    /**
     * @brief Creates or restores the node in the scene.
     *
     * On the first call, constructs the node via NodeRegistry, positions it,
     * and serializes its state for future redo calls. On subsequent calls,
     * reconstructs the node from the cached serialized state.
     */
    void redo() override
    {
        if (!m_scene) return;

        if (m_serializedNode.isEmpty())
        {
            m_node = m_scene->createNode(m_nodeType);
            if (!m_node)
            {
                qWarning() << "Failed to create node of type:" << m_nodeType;
                return;
            }
            m_node->setPos(m_position.x(), m_position.y());
            m_serializedNode = m_node->serialize();
            m_nodeId = m_node->getId();
        }
        else
        {
            NodeItem* node = m_scene->createNode(m_nodeType);
            if (!node) return;
            std::unordered_map<qint64, Serializable*> hashmap;
            node->deserialize(m_serializedNode, hashmap, true);
            m_node = node;
        }
    }

    /**
     * @brief Removes the created node from the scene.
     */
    void undo() override
    {
        if (!m_scene) return;

        if (NodeItem* node = m_scene->getNodeById(m_nodeId))
            node->remove();

        m_node = nullptr;
    }

private:

    /// @brief The scene the node is created in.
    NodeEditorGraphicsScene* m_scene = nullptr;

    /// @brief The registered type name used to create the node.
    QString m_nodeType;

    /// @brief The scene position at which the node is placed on first creation.
    QPointF m_position;

    /// @brief Pointer to the live node instance. Null after undo.
    NodeItem* m_node = nullptr;

    /// @brief The unique ID of the node, used to locate it for removal on undo.
    qint64 m_nodeId = -1;

    /// @brief Serialized node state captured after first creation, used for redo restoration.
    QJsonObject m_serializedNode;
};


/**
 * @brief Undo command that creates an edge between two sockets.
 *
 * Handles the full lifecycle of a drag-to-connect operation: removing the
 * temporary drag edge, optionally removing a conflicting or previous edge
 * on the target socket, and creating the final connection. Undo restores
 * any removed previous or conflicting edges.
 *
 * @see Edge
 * @see SocketItem
 * @see NodeEditorGraphicsScene
 */
class CreateEdgeCommand : public QUndoCommand
{
public:

    /**
     * @brief Constructs the command from a completed edge drag operation.
     *
     * Immediately removes the temporary drag edge and serializes any previous
     * or conflicting edges so they can be restored on undo.
     *
     * @param scene       The scene in which the edge is created.
     * @param dragEdge    The temporary edge created during dragging. Removed immediately.
     * @param start       The socket at the start (output) end of the new edge.
     * @param end         The socket at the end (input) end of the new edge.
     * @param previous    An existing edge on the start socket to be replaced, or nullptr.
     * @param conflicting An existing edge on the end socket to be replaced, or nullptr.
     * @param parent      Optional parent undo command.
     */
    CreateEdgeCommand(NodeEditorGraphicsScene* scene,
                      Edge* dragEdge,
                      SocketItem* start,
                      SocketItem* end,
                      Edge* previous = nullptr,
                      Edge* conflicting = nullptr,
                      QUndoCommand* parent = nullptr)
        : QUndoCommand("Create Edge", parent),
          m_scene(scene),
          m_start(start),
          m_end(end),
          m_previousEdge(previous),
          m_conflictingEdge(conflicting)
    {
        if (dragEdge)
        {
            dragEdge->remove();
            dragEdge = nullptr;
            m_edge = new Edge(m_scene);
        }

        if (m_previousEdge)
            m_serializedPrev = m_previousEdge->serialize();
        if (m_conflictingEdge)
            m_serializedConflict = m_conflictingEdge->serialize();
    }

    /**
     * @brief Notifies a socket's parent node that an edge connection has changed.
     *
     * Calls NodeItem::onEdgeConnectionChanged() on the node owning @p socket,
     * and additionally calls NodeItem::onInputChanged() if the socket is an input.
     *
     * @param socket The socket whose node should be notified.
     */
    void notifySocket(SocketItem* socket)
    {
        if (!socket) return;
        NodeItem* node = socket->getNode();
        if (!node) return;

        node->onEdgeConnectionChanged(m_edge);

        if (socket->isInput())
            node->onInputChanged(m_edge);
    }

    /**
     * @brief Creates the edge connection, removing any conflicting or previous edges first.
     *
     * On the first call, connects m_edge between m_start and m_end, notifies both
     * nodes, and serializes the result. On subsequent redo calls, reconstructs the
     * edge from the cached serialized state.
     */
    void redo() override
    {
        if (m_conflictingEdge)
        {
            m_conflictingEdge->remove();
            m_conflictingEdge = nullptr;
        }

        if (m_previousEdge)
        {
            m_previousEdge->remove();
            m_previousEdge = nullptr;
        }

        if (!m_edge)
        {
            m_edge = new Edge(m_scene);
            std::unordered_map<qint64, Serializable*> hashmap;
            for (NodeItem* node : m_scene->getNodes())
            {
                for (SocketItem* sock : node->getInputSockets())
                    hashmap[sock->getId()] = sock;
                for (SocketItem* sock : node->getOutputSockets())
                    hashmap[sock->getId()] = sock;
            }
            m_edge->deserialize(m_serializedEdge, hashmap, true);
            return;
        }

        m_edge->setStartSocket(m_start);
        m_edge->setEndSocket(m_end);
        notifySocket(m_start);
        notifySocket(m_end);
        m_edge->updatePositions();
        m_serializedEdge = m_edge->serialize();
    }

    /**
     * @brief Removes the created edge and restores any previously removed edges.
     *
     * Removes m_edge from the scene, notifies both sockets, then deserializes
     * and restores any previous or conflicting edges that were removed during redo.
     */
    void undo() override
    {
        if (m_edge)
        {
            m_edge->remove();
            m_edge = nullptr;
            notifySocket(m_start);
            notifySocket(m_end);
        }

        std::unordered_map<qint64, Serializable*> hashmap;
        for (NodeItem* node : m_scene->getNodes())
        {
            for (SocketItem* sock : node->getInputSockets())
                hashmap[sock->getId()] = sock;
            for (SocketItem* sock : node->getOutputSockets())
                hashmap[sock->getId()] = sock;
        }

        if (!m_serializedPrev.isEmpty())
        {
            m_previousEdge = new Edge(m_scene);
            m_previousEdge->deserialize(m_serializedPrev, hashmap, true);
        }
        if (!m_serializedConflict.isEmpty())
        {
            m_conflictingEdge = new Edge(m_scene);
            m_conflictingEdge->deserialize(m_serializedConflict, hashmap, true);
        }
    }

private:

    /// @brief The scene in which the edge is created.
    NodeEditorGraphicsScene* m_scene;

    /// @brief The live edge instance. Null after undo.
    Edge* m_edge = nullptr;

    /// @brief The start (output) socket of the edge.
    SocketItem* m_start = nullptr;

    /// @brief The end (input) socket of the edge.
    SocketItem* m_end = nullptr;

    /// @brief The edge previously connected to the start socket, replaced by this command.
    Edge* m_previousEdge = nullptr;

    /// @brief An edge previously connected to the end socket, displaced by this command.
    Edge* m_conflictingEdge = nullptr;

    /// @brief Serialized state of the created edge, used for redo after undo.
    QJsonObject m_serializedEdge;

    /// @brief Serialized state of m_previousEdge, used to restore it on undo.
    QJsonObject m_serializedPrev;

    /// @brief Serialized state of m_conflictingEdge, used to restore it on undo.
    QJsonObject m_serializedConflict;
};


/**
 * @brief Undo command that pastes nodes and edges from clipboard JSON data.
 *
 * On first execution, spawns nodes and edges from the clipboard data, offsetting
 * their positions so the pasted content is centred on the current mouse position.
 * Subsequent redo calls re-paste at the same relative positions. Undo removes
 * all pasted items by their tracked IDs.
 *
 * @see NodeEditorGraphicsScene
 * @see NodeEditorGraphicsView
 */
class PasteCommand : public QUndoCommand
{
public:

    /**
     * @brief Constructs the paste command with the clipboard data.
     *
     * @param scene  The scene into which items will be pasted.
     * @param data   The JSON object produced by NodeEditorGraphicsScene::serializeSelected().
     * @param parent Optional parent undo command.
     */
    PasteCommand(NodeEditorGraphicsScene* scene, const QJsonObject& data,
                 QUndoCommand* parent = nullptr)
        : QUndoCommand("Paste elements in scene", parent),
          m_scene(scene),
          m_data(data),
          m_firstExecution(true)
    {}

    /**
     * @brief Removes all nodes and edges that were pasted by this command.
     */
    void undo() override
    {
        for (qint64 edgeId : m_pastedEdgeIds)
            if (Edge* edge = m_scene->getEdgeById(edgeId))
                edge->remove();
        m_pastedEdgeIds.clear();

        for (qint64 nodeId : m_pastedNodeIds)
            if (NodeItem* node = m_scene->getNodeById(nodeId))
                node->remove();
        m_pastedNodeIds.clear();
    }

    /**
     * @brief Pastes nodes and edges into the scene.
     *
     * On the first call, reads the current mouse scene position and computes
     * the offset needed to centre the pasted content on the cursor. Stores
     * per-node relative offsets for accurate re-pasting on subsequent redo calls.
     * Edges are deserialized with fresh IDs on first execution to avoid conflicts.
     */
    void redo() override
    {
        std::unordered_map<qint64, Serializable*> hashmap;
        for (NodeItem* node : m_scene->getNodes())
        {
            for (SocketItem* sock : node->getInputSockets())  hashmap[sock->getId()] = sock;
            for (SocketItem* sock : node->getOutputSockets()) hashmap[sock->getId()] = sock;
        }

        if (m_firstExecution)
        {
            NodeEditorGraphicsView* view =
                dynamic_cast<NodeEditorGraphicsView*>(m_scene->views().first());
            m_pasteCenter = view->getLastSceneMousePosition();
            computeOriginalCenter();
        }

        QJsonArray nodesArray = m_data["nodes"].toArray();
        size_t index = 0;

        for (int i = 0; i < nodesArray.size(); ++i)
        {
            QJsonObject nodeData = nodesArray[i].toObject();
            QString type = nodeData["type"].toString();
            NodeItem* newNode = m_scene->createNode(type);
            if (!newNode) continue;

            newNode->deserialize(nodeData, hashmap, !m_firstExecution);

            QPointF originalPos = newNode->pos();
            QPointF finalPos;

            if (m_firstExecution)
            {
                QPointF offset = m_pasteCenter - m_originalCenter;
                finalPos = originalPos + offset;
                m_relativeOffsets.push_back(finalPos - m_pasteCenter);
                nodesArray[i] = newNode->serialize();
            }
            else
            {
                finalPos = m_pasteCenter + m_relativeOffsets[index];
            }

            newNode->setPos(finalPos.x(), finalPos.y());
            m_pastedNodeIds.push_back(newNode->getId());
            index++;
        }

        m_data["nodes"] = nodesArray;

        if (m_data.contains("edges"))
        {
            QJsonArray edgesArray = m_data["edges"].toArray();
            for (int i = 0; i < edgesArray.size(); ++i)
            {
                QJsonObject edgeData = edgesArray[i].toObject();
                Edge* newEdge = new Edge(m_scene);
                newEdge->deserialize(edgeData, hashmap, false);
                m_scene->addEdge(newEdge);
                m_pastedEdgeIds.push_back(newEdge->getId());

                if (m_firstExecution)
                    edgesArray[i] = newEdge->serialize();
            }

            m_data["edges"] = edgesArray;
        }

        m_firstExecution = false;
    }

private:

    /// @brief The scene into which items are pasted.
    NodeEditorGraphicsScene* m_scene;

    /// @brief The clipboard JSON data containing nodes and edges to paste.
    QJsonObject m_data;

    /// @brief IDs of all nodes created by this paste, used for removal on undo.
    std::vector<qint64> m_pastedNodeIds;

    /// @brief IDs of all edges created by this paste, used for removal on undo.
    std::vector<qint64> m_pastedEdgeIds;

    /// @brief True only during the first redo call; drives position offset calculation.
    bool m_firstExecution;

    /// @brief The scene position of the mouse cursor at the time of the first paste.
    QPointF m_pasteCenter;

    /// @brief The geometric centre of the pasted nodes in their original (clipboard) positions.
    QPointF m_originalCenter;

    /// @brief Per-node offsets relative to m_pasteCenter, used for accurate re-pasting on redo.
    std::vector<QPointF> m_relativeOffsets;

    /**
     * @brief Computes the bounding-box centre of all nodes in the clipboard data.
     *
     * Iterates over the nodes array, reads each node's pos_x/pos_y fields,
     * and stores the midpoint of the bounding box in m_originalCenter.
     * Used to calculate the translation offset that centres pasted content
     * on the mouse cursor.
     */
    void computeOriginalCenter()
    {
        double minx = 1e9, maxx = -1e9;
        double miny = 1e9, maxy = -1e9;

        QJsonArray nodesArray = m_data["nodes"].toArray();
        for (auto val : nodesArray)
        {
            QJsonObject obj = val.toObject();
            double x = obj["pos_x"].toDouble();
            double y = obj["pos_y"].toDouble();
            minx = std::min(minx, x);
            maxx = std::max(maxx, x);
            miny = std::min(miny, y);
            maxy = std::max(maxy, y);
        }

        m_originalCenter = QPointF((minx + maxx) / 2.0, (miny + maxy) / 2.0);
    }
};


/**
 * @brief Undo command that cuts (removes) selected nodes and edges, with restoration on undo.
 *
 * On construction, collects all selected nodes and their connected edges and
 * captures the scene's serialized state. Redo removes the items; undo
 * reconstructs them from the serialized data.
 *
 * @see NodeEditorGraphicsScene
 * @see NodeItem
 * @see Edge
 */
class CutCommand : public QUndoCommand
{
public:

    /**
     * @brief Constructs the cut command from the current selection.
     *
     * Walks the selected items, collects all NodeItem and EdgeGraphicsPathItem
     * instances, and also includes any edges connected to selected nodes that
     * are not already in the selection. The clipboard JSON data is stored for
     * undo restoration.
     *
     * @param scene    The scene from which items will be cut.
     * @param cutData  The serialized JSON of the cut selection, produced by
     *                 NodeEditorGraphicsScene::serializeSelected().
     * @param selected The list of currently selected QGraphicsItems.
     * @param parent   Optional parent undo command.
     */
    CutCommand(NodeEditorGraphicsScene* scene, const QJsonObject& cutData,
               const QList<QGraphicsItem*>& selected,
               QUndoCommand* parent = nullptr)
        : QUndoCommand("Cut elements from scene", parent),
          m_scene(scene),
          m_data(cutData)
    {
        for (QGraphicsItem* item : selected) {
            if (auto edgeItem = dynamic_cast<EdgeGraphicsPathItem*>(item))
            {
                if (Edge* edge = edgeItem->getEdge())
                    m_edges.append(edge);
            }
            else if (auto nodeItem = dynamic_cast<NodeItem*>(item))
            {
                m_nodes.append(nodeItem);
                for (Edge* edge : nodeItem->getConnectedEdges())
                    if (!m_edges.contains(edge))
                        m_edges.append(edge);
            }
        }
    }

    /**
     * @brief Restores all cut nodes and edges from the serialized clipboard data.
     *
     * Reconstructs nodes first so their sockets are available in the hashmap
     * for edge deserialization.
     */
    void undo() override
    {
        std::unordered_map<qint64, Serializable*> hashmap;

        QJsonArray nodesArray = m_data["nodes"].toArray();
        for (auto val : nodesArray)
        {
            QJsonObject nodeData = val.toObject();
            QString type = nodeData["type"].toString();
            NodeItem* newNode = m_scene->createNode(type);
            if (!newNode) continue;
            m_nodes.append(newNode);
            newNode->deserialize(nodeData, hashmap, true);
        }

        for (NodeItem* node : m_scene->getNodes())
        {
            for (SocketItem* sock : node->getInputSockets())
                hashmap[sock->getId()] = sock;
            for (SocketItem* sock : node->getOutputSockets())
                hashmap[sock->getId()] = sock;
        }

        if (m_data.contains("edges"))
        {
            QJsonArray edgesArray = m_data["edges"].toArray();
            for (auto val : edgesArray)
            {
                QJsonObject edgeData = val.toObject();
                Edge* newEdge = new Edge(m_scene);
                m_edges.append(newEdge);
                newEdge->deserialize(edgeData, hashmap, true);
            }
        }
    }

    /**
     * @brief Removes all collected nodes and edges from the scene.
     */
    void redo() override
    {
        for (NodeItem* node : m_nodes)
            if (node) node->remove();
        for (Edge* edge : m_edges)
            if (edge) edge->remove();
        m_nodes.clear();
        m_edges.clear();
    }

private:

    /// @brief The scene from which items are cut.
    NodeEditorGraphicsScene* m_scene;

    /// @brief Serialized JSON of the cut items, used for restoration on undo.
    QJsonObject m_data;

    /// @brief The nodes collected from the selection at construction time.
    QList<NodeItem*> m_nodes;

    /// @brief The edges collected from the selection and their connected nodes.
    QList<Edge*> m_edges;
};


/**
 * @brief Undo command that records and replays the movement of one or more nodes.
 *
 * Stores the start and end positions of every moved node, keyed by node ID.
 * Undo restores all nodes to their pre-move positions; redo moves them back
 * to their post-move positions. Connected edges are updated after each position change.
 *
 * @see NodeEditorGraphicsScene
 * @see NodeItem
 */
class MoveNodeCommand : public QUndoCommand
{
public:

    /**
     * @brief Constructs the command with per-node before/after position data.
     *
     * @param scene    The scene containing the moved nodes.
     * @param moveData A map of node ID to (startPosition, endPosition) pairs.
     * @param parent   Optional parent undo command.
     */
    MoveNodeCommand(NodeEditorGraphicsScene* scene,
                    const QMap<qint64, QPair<QPointF, QPointF>> moveData,
                    QUndoCommand* parent = nullptr)
        : QUndoCommand("Move Nodes", parent),
          m_scene(scene),
          m_moveData(moveData)
    {}

    /**
     * @brief Restores all nodes to their positions before the move.
     */
    void undo() override
    {
        if (!m_scene) return;
        for (auto it = m_moveData.constBegin(); it != m_moveData.constEnd(); ++it)
        {
            if (NodeItem* node = m_scene->getNodeById(it.key()))
            {
                node->setPos(it.value().first.x(), it.value().first.y());
                node->updateConnectedEdges();
            }
        }
    }

    /**
     * @brief Moves all nodes to their positions after the move.
     */
    void redo() override
    {
        if (!m_scene) return;
        for (auto it = m_moveData.constBegin(); it != m_moveData.constEnd(); ++it)
        {
            if (NodeItem* node = m_scene->getNodeById(it.key()))
            {
                node->setPos(it.value().second.x(), it.value().second.y());
                node->updateConnectedEdges();
            }
        }
    }

private:

    /// @brief The scene containing the nodes being moved.
    NodeEditorGraphicsScene* m_scene = nullptr;

    /**
     * @brief Per-node movement data.
     *
     * Maps each node's unique ID to a pair of (startPosition, endPosition).
     * The first element is the position before the move (used by undo),
     * the second is the position after (used by redo).
     */
    QMap<qint64, QPair<QPointF, QPointF>> m_moveData;
};


/**
 * @brief Undo command that deletes selected nodes and edges, with full restoration on undo.
 *
 * On construction, serializes all selected nodes and their connected edges so
 * they can be reconstructed exactly on undo. Redo removes items by their stored
 * IDs; undo recreates nodes first, rebuilds the socket hashmap, then restores edges.
 *
 * @see NodeEditorGraphicsScene
 * @see NodeItem
 * @see Edge
 */
class DeleteSelectedCommand : public QUndoCommand
{
public:

    /**
     * @brief Constructs the command from the current selection, serializing all items.
     *
     * Walks selected items to collect unique NodeItem and EdgeGraphicsPathItem instances.
     * For each selected node, connected edges are also collected even if not directly
     * selected, ensuring the scene remains consistent on undo.
     *
     * @param scene    The scene from which items will be deleted.
     * @param selected The list of currently selected QGraphicsItems.
     * @param parent   Optional parent undo command.
     */
    DeleteSelectedCommand(NodeEditorGraphicsScene* scene,
                          const QList<QGraphicsItem*>& selected,
                          QUndoCommand* parent = nullptr)
        : QUndoCommand("Delete Selected", parent),
          m_scene(scene)
    {
        for (QGraphicsItem* item : selected) {
            if (auto edgeItem = dynamic_cast<EdgeGraphicsPathItem*>(item))
            {
                if (Edge* edge = edgeItem->getEdge())
                {
                    if (!m_edgeIds.contains(edge->getId()))
                    {
                        m_edgeIds.append(edge->getId());
                        m_serializedEdges.append(edge->serialize());
                    }
                }
            }
            else if (auto nodeItem = dynamic_cast<NodeItem*>(item))
            {
                if (!m_nodeIds.contains(nodeItem->getId()))
                {
                    m_nodeIds.append(nodeItem->getId());
                    m_serializedNodes.append(nodeItem->serialize());
                }
                for (Edge* edge : nodeItem->getConnectedEdges())
                {
                    if (!m_edgeIds.contains(edge->getId()))
                    {
                        m_edgeIds.append(edge->getId());
                        m_serializedEdges.append(edge->serialize());
                    }
                }
            }
        }
    }

    /**
     * @brief Restores all deleted nodes and edges from their serialized state.
     *
     * Nodes are recreated first so their sockets are available when edges
     * are deserialized and socket references are resolved via the hashmap.
     */
    void undo() override
    {
        if (!m_scene) return;

        std::unordered_map<qint64, Serializable*> socketMap;

        for (const QJsonObject& nodeData : m_serializedNodes)
        {
            QString type = nodeData["type"].toString();
            NodeItem* node = m_scene->createNode(type);
            if (!node) continue;
            node->deserialize(nodeData, socketMap, true);
        }

        for (NodeItem* node : m_scene->getNodes())
        {
            for (SocketItem* sock : node->getInputSockets())
                socketMap[sock->getId()] = sock;
            for (SocketItem* sock : node->getOutputSockets())
                socketMap[sock->getId()] = sock;
        }

        for (const QJsonObject& edgeData : m_serializedEdges)
        {
            Edge* edge = new Edge(m_scene);
            edge->deserialize(edgeData, socketMap, true);
        }
    }

    /**
     * @brief Removes all collected nodes and edges from the scene by their stored IDs.
     */
    void redo() override
    {
        if (!m_scene) return;
        for (qint64 id : m_nodeIds)
            if (NodeItem* node = m_scene->getNodeById(id))
                node->remove();
        for (qint64 id : m_edgeIds)
            if (Edge* edge = m_scene->getEdgeById(id))
                edge->remove();
    }

private:

    /// @brief The scene from which items are deleted.
    NodeEditorGraphicsScene* m_scene;

    /// @brief IDs of deleted nodes, used to locate and remove them on redo.
    QList<qint64> m_nodeIds;

    /// @brief IDs of deleted edges, used to locate and remove them on redo.
    QList<qint64> m_edgeIds;

    /// @brief Serialized state of each deleted node, used for restoration on undo.
    QList<QJsonObject> m_serializedNodes;

    /// @brief Serialized state of each deleted edge, used for restoration on undo.
    QList<QJsonObject> m_serializedEdges;
};

#endif // UNDOCOMMANDS_H