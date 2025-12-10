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

#include "Scene.h"
#include "Socket.h"
#include "Edge.h"
#include "Node.h"
#include "NodeEditorGraphicsView.h"
#include "NodeGraphicsItem.h"
#include "EdgeGraphicsPathItem.h"
#include "Serializable.h"

// --------------------------------------
// Selection Changed
// --------------------------------------
class SelectionChangedCommand : public QUndoCommand {
public:
    SelectionChangedCommand(Scene* scene,
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

    void undo() override { applySelection(m_oldNodeIds, m_oldEdgeIds); }
    void redo() override { applySelection(m_newNodeIds, m_newEdgeIds); }

private:
    void applySelection(const QSet<qint64>& nodeIds, const QSet<qint64>& edgeIds) {
        if (!m_scene) return;
        QGraphicsScene* gscene = m_scene->graphicsScene();
        if (!gscene) return;

        gscene->blockSignals(true);
        gscene->clearSelection();

        // Re-select nodes
        for (auto id : nodeIds) {
            Node* node = m_scene->getNodeById(id);
            if (node && node->getNodeGraphicsItem())
                node->getNodeGraphicsItem()->setSelected(true);
        }

        // Re-select edges
        for (auto id : edgeIds) {
            Edge* edge = m_scene->getEdgeById(id);
            if (edge && edge->getEdgeGraphicsItem())
                edge->getEdgeGraphicsItem()->setSelected(true);
        }

        gscene->blockSignals(false);
    }

    Scene* m_scene = nullptr;
    QSet<qint64> m_oldNodeIds;
    QSet<qint64> m_oldEdgeIds;
    QSet<qint64> m_newNodeIds;
    QSet<qint64> m_newEdgeIds;
};


// --------------------------------------
// Create Edge
// --------------------------------------
class CreateEdgeCommand : public QUndoCommand {
public:
    CreateEdgeCommand(Scene* scene,
                      Edge* dragEdge,
                      Socket* start,
                      Socket* end,
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
        if (dragEdge) {
            dragEdge->remove();
            dragEdge = nullptr;
            m_edge = new Edge(m_scene);
        }
        if (m_previousEdge)
            m_serializedPrev = m_previousEdge->serialize();
        if (m_conflictingEdge)
            m_serializedConflict = m_conflictingEdge->serialize();
    }

    void redo() override {
        // Remove conflicting edges first
        if (m_conflictingEdge) {
            m_conflictingEdge->remove();
            m_conflictingEdge = nullptr;
        }
        if (m_previousEdge){

            m_previousEdge->remove();
            m_previousEdge = nullptr;
        }

        if (!m_edge) {
            m_edge = new Edge(m_scene);
            std::unordered_map<qint64, Serializable*> hashmap;
            for (Node* node : m_scene->getNodes()) {
                for (Socket* sock : node->inputs)
                    hashmap[sock->getId()] = sock;
                for (Socket* sock : node->outputs)
                    hashmap[sock->getId()] = sock;
            }
            m_edge->deserialize(m_serializedEdge, hashmap, true);
            return;
        }

        m_edge->setStartSocket(m_start);
        m_edge->setEndSocket(m_end);
        m_edge->updatePositions();
        m_serializedEdge = m_edge->serialize();
    }

    void undo() override {
        if (m_edge) {
            m_edge->remove();
            m_edge = nullptr;
        }

        std::unordered_map<qint64, Serializable*> hashmap;
        for (Node* node : m_scene->getNodes()) {
            for (Socket* sock : node->inputs)
                hashmap[sock->getId()] = sock;
            for (Socket* sock : node->outputs)
                hashmap[sock->getId()] = sock;
        }
        // Restore any edges that were removed

        if (!m_serializedPrev.isEmpty()) {
            m_previousEdge = new Edge(m_scene);
            m_previousEdge->deserialize(m_serializedPrev, hashmap, true);
        }
        if (!m_serializedConflict.isEmpty()) {
            m_conflictingEdge = new Edge(m_scene);
            m_conflictingEdge->deserialize(m_serializedConflict, hashmap, true);
        }
    }

private:
    Scene* m_scene;
    Edge* m_edge = nullptr;
    Socket* m_start = nullptr;
    Socket* m_end = nullptr;

    Edge* m_previousEdge = nullptr;
    Edge* m_conflictingEdge = nullptr;

    QJsonObject m_serializedEdge;
    QJsonObject m_serializedPrev;
    QJsonObject m_serializedConflict;
};
// --------------------------------------
// Paste
// --------------------------------------
class PasteCommand : public QUndoCommand {
public:
    PasteCommand(Scene* scene, const QJsonObject& data, QUndoCommand* parent = nullptr)
        : QUndoCommand("Paste elements in scene", parent),
        scene(scene), data(data), firstExecution(true) {}

    void undo() override {
        // Remove pasted edges FIRST
        for (qint64 edgeId : pastedEdgeIds) {
            if (Edge* edge = scene->getEdgeById(edgeId)) {
                edge->remove();
            }
        }
        pastedEdgeIds.clear();

        // Now remove pasted nodes
        for (qint64 nodeId : pastedNodeIds) {
            if (Node* node = scene->getNodeById(nodeId)) {
                node->remove();
            }
        }
        pastedNodeIds.clear();
    }

    void redo() override {
        std::unordered_map<qint64, Serializable*> hashmap;

        // Build socket lookup table
        for (Node* node : scene->getNodes()) {
            for (Socket* sock : node->inputs)  hashmap[sock->getId()] = sock;
            for (Socket* sock : node->outputs) hashmap[sock->getId()] = sock;
        }

        // First-time paste → compute relative offset
        if (firstExecution) {
            NodeEditorGraphicsView* view =
                dynamic_cast<NodeEditorGraphicsView*>(scene->graphicsScene()->views().first());
            pasteCenter = view->getLastSceneMousePosition();

            // Compute center of copied nodes
            computeOriginalCenter();
        }

        QJsonArray nodesArray = data["nodes"].toArray();
        size_t index = 0;

        // --- PASTE NODES -----------------------------------------------------
        for (int i = 0; i < nodesArray.size(); ++i) {
            QJsonObject nodeData = nodesArray[i].toObject();

            // Create node
            Node* newNode = new Node(scene);
            newNode->deserialize(nodeData, hashmap, !firstExecution);

            QPointF originalPos = newNode->pos();
            QPointF finalPos;

            if (firstExecution) {
                // Compute position offset
                QPointF offset = pasteCenter - originalCenter;
                finalPos = originalPos + offset;

                // Save offset for future redo()
                relativeOffsets.push_back(finalPos - pasteCenter);

                // Update serialized nodeData for next undo/redo
                nodesArray[i] = newNode->serialize();
            }
            else {
                // Apply same relative offsets
                if (index < relativeOffsets.size())
                    finalPos = pasteCenter + relativeOffsets[index];
                else
                    finalPos = pasteCenter;
            }

            newNode->setPos(finalPos.x(), finalPos.y());

            // TRACK BY ID, NOT POINTER
            pastedNodeIds.push_back(newNode->getId());
            index++;
        }

        data["nodes"] = nodesArray;

        // --- PASTE EDGES ------------------------------------------------------
        if (data.contains("edges")) {
            QJsonArray edgesArray = data["edges"].toArray();

            for (int i = 0; i < edgesArray.size(); ++i) {
                QJsonObject edgeData = edgesArray[i].toObject();
                Edge* newEdge = new Edge(scene);

                newEdge->deserialize(edgeData, hashmap, false);
                scene->addEdge(newEdge);

                pastedEdgeIds.push_back(newEdge->getId());

                if (firstExecution) {
                    edgesArray[i] = newEdge->serialize();
                }
            }

            data["edges"] = edgesArray;
        }

        firstExecution = false;
    }

private:
    Scene* scene;
    QJsonObject data;

    std::vector<qint64> pastedNodeIds;
    std::vector<qint64> pastedEdgeIds;

    bool firstExecution;
    QPointF pasteCenter;
    QPointF originalCenter;
    std::vector<QPointF> relativeOffsets;

    void computeOriginalCenter() {
        double minx = 1e9, maxx = -1e9;
        double miny = 1e9, maxy = -1e9;

        QJsonArray nodesArray = data["nodes"].toArray();
        for (auto val : nodesArray) {
            QJsonObject obj = val.toObject();
            double x = obj["pos_x"].toDouble();
            double y = obj["pos_y"].toDouble();
            minx = std::min(minx, x);
            maxx = std::max(maxx, x);
            miny = std::min(miny, y);
            maxy = std::max(maxy, y);
        }

        originalCenter = QPointF((minx + maxx) / 2.0, (miny + maxy) / 2.0);
    }
};

// --------------------------------------
// Cut
// --------------------------------------
class CutCommand : public QUndoCommand {
public:
    CutCommand(Scene* scene, const QJsonObject& cutData, const QList<QGraphicsItem*>& selected, QUndoCommand* parent = nullptr)
        : QUndoCommand("Cut elements from scene", parent),
          scene(scene), data(cutData)
    {
        for (QGraphicsItem* item : selected) {
            if (auto edgeItem = dynamic_cast<EdgeGraphicsPathItem*>(item)) {
                Edge* edge = edgeItem->getEdge();
                if (edge) {
                    m_edges.append(edge);
                }
            } else if (auto nodeItem = dynamic_cast<NodeGraphicsItem*>(item)) {
                Node* node = nodeItem->getNode();
                if (node) {
                    m_nodes.append(node);
                    // if edges arnt selected
                    for( Edge* edge :node->getConnectedEdges()){
                        if(m_edges.contains(edge)){
                            continue;
                        }
                        m_edges.append(edge);
                    }
                }
            }
        }
    }

    void undo() override {
        // same logic as PasteCommand::redo
        std::unordered_map<qint64, Serializable*> hashmap;

        QJsonArray nodesArray = data["nodes"].toArray();
        for (auto val : nodesArray) {
            QJsonObject nodeData = val.toObject();
            Node* newNode = new Node(scene);
            m_nodes.append(newNode);
            newNode->deserialize(nodeData, hashmap, true);
            scene->addNode(newNode);
        }

        if (data.contains("edges")) {
            QJsonArray edgesArray = data["edges"].toArray();
            for (auto val : edgesArray) {
                QJsonObject edgeData = val.toObject();
                Edge* newEdge = new Edge(scene);
                m_edges.append(newEdge);
                newEdge->deserialize(edgeData, hashmap, true);
                scene->addEdge(newEdge);
            }
        }
    }

    void redo() override {
        for (Node* node : m_nodes) {
            if (node) node->remove();
        }
        for (Edge* edge : m_edges) {
            if (edge) edge->remove();
        }
        m_nodes.clear();
        m_edges.clear();
    }

private:
    Scene* scene;
    QJsonObject data;
    QList<Node*> m_nodes;
    QList<Edge*> m_edges;
};

// --------------------------------------
// Move Node
// --------------------------------------
class MoveNodeCommand : public QUndoCommand {
public:
    MoveNodeCommand(Scene* scene,
                    const QMap<qint64, QPair<QPointF, QPointF>> moveData,
                    QUndoCommand* parent = nullptr)
        : QUndoCommand("Move Nodes", parent),
          m_scene(scene),
          m_moveData(moveData) {}

    void undo() override {
        qDebug() << "Im aasd runningggg";
        if (!m_scene) return;

        for (auto it = m_moveData.constBegin(); it != m_moveData.constEnd(); ++it) {
            qint64 nodeId = it.key();
            const QPointF& oldPos = it.value().first;
            if (Node* node = m_scene->getNodeById(nodeId)) {
                node->setPos(oldPos.x(), oldPos.y());
                node->updateConnectedEdges();
            }
        }
    }

    void redo() override {
        qDebug() << "Im runningggg";
        if (!m_scene) return;

        for (auto it = m_moveData.constBegin(); it != m_moveData.constEnd(); ++it) {
            qint64 nodeId = it.key();
            const QPointF& newPos = it.value().second;
            if (Node* node = m_scene->getNodeById(nodeId)) {
                node->setPos(newPos.x(), newPos.y());
                node->updateConnectedEdges();
            }
        }
    }

private:
    Scene* m_scene = nullptr;
    QMap<qint64, QPair<QPointF, QPointF>> m_moveData; // { nodeId: (oldPos, newPos) }
};
// --------------------------------------
// Delete Selected
// --------------------------------------
class DeleteSelectedCommand : public QUndoCommand {
public:
    DeleteSelectedCommand(Scene* scene,
                          const QList<QGraphicsItem*>& selected,
                          QUndoCommand* parent = nullptr)
        : QUndoCommand("Delete Selected", parent),
        m_scene(scene)
    {
        // Collect all node IDs and edge IDs + serialize them
        for (QGraphicsItem* item : selected) {

            if (auto edgeItem = dynamic_cast<EdgeGraphicsPathItem*>(item)) {
                Edge* edge = edgeItem->getEdge();
                if (!edge) continue;

                qint64 id = edge->getId();
                if (!m_edgeIds.contains(id)) {
                    m_edgeIds.append(id);
                    m_serializedEdges.append(edge->serialize());
                }
            }

            else if (auto nodeItem = dynamic_cast<NodeGraphicsItem*>(item)) {
                Node* node = nodeItem->getNode();
                if (!node) continue;

                qint64 nodeId = node->getId();
                if (!m_nodeIds.contains(nodeId)) {
                    m_nodeIds.append(nodeId);
                    m_serializedNodes.append(node->serialize());
                }

                // Make sure connected edges are also saved,
                // EVEN if they were not selected.
                for (Edge* edge : node->getConnectedEdges()) {
                    qint64 eid = edge->getId();
                    if (!m_edgeIds.contains(eid)) {
                        m_edgeIds.append(eid);
                        m_serializedEdges.append(edge->serialize());
                    }
                }
            }
        }
    }

    void undo() override {
        if (!m_scene) return;

        std::unordered_map<qint64, Serializable*> socketMap;

        // 1) Restore nodes
        for (const QJsonObject& nodeData : m_serializedNodes) {
            Node* node = new Node(m_scene);
            node->deserialize(nodeData, socketMap, true);
        }


        // Add sockets to lookup table for edge restoration
        for (Node* node : m_scene->getNodes()) {
            for (Socket* sock : node->inputs)  socketMap[sock->getId()] = sock;
            for (Socket* sock : node->outputs) socketMap[sock->getId()] = sock;
        }

        // 2) Restore edges now that socketMap is complete
        for (const QJsonObject& edgeData : m_serializedEdges) {
            Edge* edge = new Edge(m_scene);
            edge->deserialize(edgeData, socketMap, true);
        }
    }

    void redo() override {
        if (!m_scene) return;

        // Remove nodes
        for (qint64 id : m_nodeIds) {
            if (Node* node = m_scene->getNodeById(id))
                node->remove();
        }

        // Remove edges
        for (qint64 id : m_edgeIds) {
            if (Edge* edge = m_scene->getEdgeById(id))
                edge->remove();
        }
    }

private:
    Scene* m_scene;

    // IDs of nodes/edges that were deleted
    QList<qint64> m_nodeIds;
    QList<qint64> m_edgeIds;

    // Serialized JSON data (complete state needed to restore)
    QList<QJsonObject> m_serializedNodes;
    QList<QJsonObject> m_serializedEdges;
};

#endif // UNDOCOMMANDS_H
