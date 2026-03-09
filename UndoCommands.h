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

#include "Scene.h"
#include "Socket.h"
#include "Edge.h"
#include "Node.h"
#include "NodeEditorGraphicsView.h"
#include "NodeGraphicsItem.h"
#include "EdgeGraphicsPathItem.h"
#include "Serializable.h"
#include "NodeRegistry.h"

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
    void applySelection(const QSet<qint64>& nodeIds, const QSet<qint64>& edgeIds)
    {
        if (!m_scene) return;
        QGraphicsScene* gscene = m_scene->graphicsScene();
        if (!gscene) return;

        gscene->blockSignals(true);
        gscene->clearSelection();

        for (auto id : nodeIds) {
            Node* node = m_scene->getNodeById(id);
            if (node && node->getNodeGraphicsItem())
                node->getNodeGraphicsItem()->setSelected(true);
        }

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

class CreateNodeCommand : public QUndoCommand {
public:
    CreateNodeCommand(Scene* scene,
                      const QString& nodeType,
                      const QPointF& position,
                      QUndoCommand* parent = nullptr)
        : QUndoCommand("Create Node", parent),
          m_scene(scene),
          m_nodeType(nodeType),
          m_position(position)
    {}

    void redo() override {
        if (!m_scene) return;

        // If this is the first execution, create the node
        if (m_serializedNode.isEmpty()) {
            m_node = m_scene->createNode(m_nodeType);
            if (!m_node) {
                qWarning() << "Failed to create node of type:" << m_nodeType;
                return;
            }

            m_node->setPos(m_position.x(), m_position.y());
            m_serializedNode = m_node->serialize();
            m_nodeId = m_node->getId();
        }
        // Redo after undo → restore from serialized state
        else {
            Node* node = m_scene->createNode(m_nodeType);
            if (!node) return;
            std::unordered_map<qint64, Serializable*> hashmap;
            node->deserialize(m_serializedNode, hashmap, true);
            m_node = node;
        }
    }

    void undo() override {
        if (!m_scene) return;

        if (Node* node = m_scene->getNodeById(m_nodeId)) {
            node->remove();
        }

        m_node = nullptr;
    }

private:
    Scene* m_scene = nullptr;
    QString m_nodeType;
    QPointF m_position;

    Node* m_node = nullptr;
    qint64 m_nodeId = -1;
    QJsonObject m_serializedNode;
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

    void notifySocket(Socket* socket)
    {
        if (!socket) return;

        Node* node = socket->getNode();
        if (!node) return;

        node->onEdgeConnectionChanged(m_edge);

        if (socket->isInput())
            node->onInputChanged(m_edge);
    }

    void redo() override {
        if (m_conflictingEdge) {
            m_conflictingEdge->remove();
            m_conflictingEdge = nullptr;
        }
        if (m_previousEdge) {
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
        notifySocket(m_start);
        notifySocket(m_end);
        m_edge->updatePositions();
        m_serializedEdge = m_edge->serialize();
    }

    void undo() override {
        if (m_edge) {
            m_edge->remove();
            m_edge = nullptr;
            notifySocket(m_start);
            notifySocket(m_end);
        }

        std::unordered_map<qint64, Serializable*> hashmap;
        for (Node* node : m_scene->getNodes()) {
            for (Socket* sock : node->inputs)
                hashmap[sock->getId()] = sock;
            for (Socket* sock : node->outputs)
                hashmap[sock->getId()] = sock;
        }

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
        for (qint64 edgeId : pastedEdgeIds)
            if (Edge* edge = scene->getEdgeById(edgeId))
                edge->remove();
        pastedEdgeIds.clear();

        for (qint64 nodeId : pastedNodeIds)
            if (Node* node = scene->getNodeById(nodeId))
                node->remove();
        pastedNodeIds.clear();
    }

    void redo() override {
        std::unordered_map<qint64, Serializable*> hashmap;
        for (Node* node : scene->getNodes()) {
            for (Socket* sock : node->inputs)  hashmap[sock->getId()] = sock;
            for (Socket* sock : node->outputs) hashmap[sock->getId()] = sock;
        }

        if (firstExecution) {
            NodeEditorGraphicsView* view =
                dynamic_cast<NodeEditorGraphicsView*>(scene->graphicsScene()->views().first());
            pasteCenter = view->getLastSceneMousePosition();
            computeOriginalCenter();
        }

        QJsonArray nodesArray = data["nodes"].toArray();
        size_t index = 0;

        for (int i = 0; i < nodesArray.size(); ++i) {
            QJsonObject nodeData = nodesArray[i].toObject();

            QString type = nodeData["type"].toString();
            Node* newNode = scene->createNode(type);
            if (!newNode) continue;

            newNode->deserialize(nodeData, hashmap, !firstExecution);

            QPointF originalPos = newNode->pos();
            QPointF finalPos;

            if (firstExecution) {
                QPointF offset = pasteCenter - originalCenter;
                finalPos = originalPos + offset;
                relativeOffsets.push_back(finalPos - pasteCenter);
                nodesArray[i] = newNode->serialize();
            } else {
                finalPos = pasteCenter + relativeOffsets[index];
            }

            newNode->setPos(finalPos.x(), finalPos.y());
            pastedNodeIds.push_back(newNode->getId());
            index++;
        }

        data["nodes"] = nodesArray;

        if (data.contains("edges")) {
            QJsonArray edgesArray = data["edges"].toArray();
            for (int i = 0; i < edgesArray.size(); ++i) {
                QJsonObject edgeData = edgesArray[i].toObject();
                Edge* newEdge = new Edge(scene);
                newEdge->deserialize(edgeData, hashmap, false);
                scene->addEdge(newEdge);
                pastedEdgeIds.push_back(newEdge->getId());

                if (firstExecution)
                    edgesArray[i] = newEdge->serialize();
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
    CutCommand(Scene* scene, const QJsonObject& cutData,
               const QList<QGraphicsItem*>& selected,
               QUndoCommand* parent = nullptr)
        : QUndoCommand("Cut elements from scene", parent),
          scene(scene), data(cutData)
    {
        for (QGraphicsItem* item : selected) {
            if (auto edgeItem = dynamic_cast<EdgeGraphicsPathItem*>(item)) {
                if (Edge* edge = edgeItem->getEdge())
                    m_edges.append(edge);
            } else if (auto nodeItem = dynamic_cast<NodeGraphicsItem*>(item)) {
                if (Node* node = nodeItem->getNode()) {
                    m_nodes.append(node);
                    for (Edge* edge : node->getConnectedEdges())
                        if (!m_edges.contains(edge))
                            m_edges.append(edge);
                }
            }
        }
    }

    void undo() override {
        std::unordered_map<qint64, Serializable*> hashmap;

        QJsonArray nodesArray = data["nodes"].toArray();
        for (auto val : nodesArray) {
            QJsonObject nodeData = val.toObject();

            QString type = nodeData["type"].toString();
            Node* newNode = scene->createNode(type);
            if (!newNode) continue;

            m_nodes.append(newNode);
            newNode->deserialize(nodeData, hashmap, true);
        }

        if (data.contains("edges")) {
            QJsonArray edgesArray = data["edges"].toArray();
            for (auto val : edgesArray) {
                QJsonObject edgeData = val.toObject();
                Edge* newEdge = new Edge(scene);
                m_edges.append(newEdge);
                newEdge->deserialize(edgeData, hashmap, true);
            }
        }
    }

    void redo() override {
        for (Node* node : m_nodes)
            if (node) node->remove();
        for (Edge* edge : m_edges)
            if (edge) edge->remove();
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
        if (!m_scene) return;
        for (auto it = m_moveData.constBegin(); it != m_moveData.constEnd(); ++it) {
            if (Node* node = m_scene->getNodeById(it.key())) {
                node->setPos(it.value().first.x(), it.value().first.y());
                node->updateConnectedEdges();
            }
        }
    }

    void redo() override {
        if (!m_scene) return;
        for (auto it = m_moveData.constBegin(); it != m_moveData.constEnd(); ++it) {
            if (Node* node = m_scene->getNodeById(it.key())) {
                node->setPos(it.value().second.x(), it.value().second.y());
                node->updateConnectedEdges();
            }
        }
    }

private:
    Scene* m_scene = nullptr;
    QMap<qint64, QPair<QPointF, QPointF>> m_moveData;
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
        for (QGraphicsItem* item : selected) {
            if (auto edgeItem = dynamic_cast<EdgeGraphicsPathItem*>(item)) {
                if (Edge* edge = edgeItem->getEdge()) {
                    if (!m_edgeIds.contains(edge->getId())) {
                        m_edgeIds.append(edge->getId());
                        m_serializedEdges.append(edge->serialize());
                    }
                }
            } else if (auto nodeItem = dynamic_cast<NodeGraphicsItem*>(item)) {
                if (Node* node = nodeItem->getNode()) {
                    if (!m_nodeIds.contains(node->getId())) {
                        m_nodeIds.append(node->getId());
                        m_serializedNodes.append(node->serialize());
                    }
                    for (Edge* edge : node->getConnectedEdges()) {
                        if (!m_edgeIds.contains(edge->getId())) {
                            m_edgeIds.append(edge->getId());
                            m_serializedEdges.append(edge->serialize());
                        }
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
            QString type = nodeData["type"].toString();
            Node* node = m_scene->createNode(type);
            if (!node) continue;
            node->deserialize(nodeData, socketMap, true);
        }

        for (Node* node : m_scene->getNodes()) {
            for (Socket* sock : node->inputs)
                socketMap[sock->getId()] = sock;
            for (Socket* sock : node->outputs)
                socketMap[sock->getId()] = sock;
        }

        // 3) Restore edges
        for (const QJsonObject& edgeData : m_serializedEdges) {
            Edge* edge = new Edge(m_scene);
            edge->deserialize(edgeData, socketMap, true);
        }
    }

    void redo() override {
        if (!m_scene) return;
        for (qint64 id : m_nodeIds)
            if (Node* node = m_scene->getNodeById(id))
                node->remove();
        for (qint64 id : m_edgeIds)
            if (Edge* edge = m_scene->getEdgeById(id))
                edge->remove();
    }

private:
    Scene* m_scene;
    QList<qint64> m_nodeIds;
    QList<qint64> m_edgeIds;
    QList<QJsonObject> m_serializedNodes;
    QList<QJsonObject> m_serializedEdges;
};

#endif // UNDOCOMMANDS_H
