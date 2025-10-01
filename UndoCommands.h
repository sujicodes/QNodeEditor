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
    SelectionChangedCommand(Scene *scene,
                            const QList<QGraphicsItem*> &oldSel,
                            const QList<QGraphicsItem*> &newSel,
                            QUndoCommand *parent = nullptr)
        : QUndoCommand("Selection Changed", parent),
          m_scene(scene),
          m_oldSelection(oldSel),
          m_newSelection(newSel) {}

    void undo() override {
        setSelection(m_oldSelection);
    }

    void redo() override {
        setSelection(m_newSelection);
    }

private:
    void setSelection(const QList<QGraphicsItem*> &items) {
        // clear existing selection
        for (auto *item : m_scene->graphicsScene()->selectedItems()) {
            item->setSelected(false);
        }
        // reapply
        for (auto *item : items) {
            item->setSelected(true);
        }
    }

    Scene *m_scene;
    QList<QGraphicsItem*> m_oldSelection;
    QList<QGraphicsItem*> m_newSelection;
};

// --------------------------------------
// Create Edge
// --------------------------------------
class CreateEdgeCommand : public QUndoCommand {
public:
    CreateEdgeCommand(Scene* scene,
                      Edge* edge,
                      Socket* start,
                      Socket* end,
                      Edge* previous = nullptr,
                      Edge* conflicting = nullptr,
                      QUndoCommand* parent = nullptr)
        : QUndoCommand("Create Edge", parent),
          m_scene(scene),
          m_edge(edge),
          m_start(start),
          m_end(end),
          m_previousEdge(previous),
          m_conflictingEdge(conflicting)
    {
        if (m_edge)
            m_serializedEdge = m_edge->serialize();
        if (m_previousEdge)
            m_serializedPrev = m_previousEdge->serialize();
        if (m_conflictingEdge)
            m_serializedConflict = m_conflictingEdge->serialize();
    }

    void redo() override {
        // Remove conflicting edges first
        if (m_conflictingEdge) { m_conflictingEdge->remove(); m_conflictingEdge = nullptr; }
        if (m_previousEdge)    { m_previousEdge->remove();    m_previousEdge = nullptr; }

        if (!m_edge) {
            m_edge = new Edge(m_scene);
            std::unordered_map<qint64, Serializable*> hashmap;
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

        // Restore any edges that were removed
        std::unordered_map<qint64, Serializable*> hashmap;
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
          scene(scene), data(data) {}

    void undo() override {
        for (Node* node : pastedNodes) {
            scene->removeNode(node);
        }
        pastedNodes.clear();

        for (Edge* edge : pastedEdges) {
            scene->removeEdge(edge);
        }
        pastedEdges.clear();
    }

    void redo() override {
        std::unordered_map<qint64, Serializable*> hashmap;

        NodeEditorGraphicsView* view =
            dynamic_cast<NodeEditorGraphicsView*>(scene->graphicsScene()->views().first());
        QPointF mouseScenePos = view->getLastSceneMousePosition();

        // bbox
        double minx = std::numeric_limits<double>::max();
        double maxx = std::numeric_limits<double>::lowest();
        double miny = std::numeric_limits<double>::max();
        double maxy = std::numeric_limits<double>::lowest();

        QJsonArray nodesArray = data["nodes"].toArray();
        for (auto val : nodesArray) {
            QJsonObject nodeData = val.toObject();
            double x = nodeData["pos_x"].toDouble();
            double y = nodeData["pos_y"].toDouble();
            minx = std::min(minx, x);
            maxx = std::max(maxx, x);
            miny = std::min(miny, y);
            maxy = std::max(maxy, y);
        }

        double offsetX = mouseScenePos.x() - (minx + maxx) / 2.0;
        double offsetY = mouseScenePos.y() - (miny + maxy) / 2.0;

        // create nodes
        for (auto val : nodesArray) {
            QJsonObject nodeData = val.toObject();
            Node* newNode = new Node(scene);
            newNode->deserialize(nodeData, hashmap, false);
            QPointF pos = newNode->pos();
            newNode->setPos(pos.x() + offsetX, pos.y() + offsetY);
            pastedNodes.push_back(newNode);
            scene->addNode(newNode);
        }

        // create edges
        if (data.contains("edges")) {
            QJsonArray edgesArray = data["edges"].toArray();
            for (auto val : edgesArray) {
                QJsonObject edgeData = val.toObject();
                Edge* newEdge = new Edge(scene);
                newEdge->deserialize(edgeData, hashmap, false);
                pastedEdges.push_back(newEdge);
                scene->addEdge(newEdge);
            }
        }
    }

private:
    Scene* scene;
    QJsonObject data;
    std::vector<Node*> pastedNodes;
    std::vector<Edge*> pastedEdges;
};

// --------------------------------------
// Cut
// --------------------------------------
class CutCommand : public QUndoCommand {
public:
    CutCommand(Scene* scene, const QJsonObject& cutData, QUndoCommand* parent = nullptr)
        : QUndoCommand("Cut elements from scene", parent),
          scene(scene), data(cutData) {}

    void undo() override {
        // same logic as PasteCommand::redo
        std::unordered_map<qint64, Serializable*> hashmap;

        QJsonArray nodesArray = data["nodes"].toArray();
        for (auto val : nodesArray) {
            QJsonObject nodeData = val.toObject();
            Node* newNode = new Node(scene);
            newNode->deserialize(nodeData, hashmap, true);
            scene->addNode(newNode);
        }

        if (data.contains("edges")) {
            QJsonArray edgesArray = data["edges"].toArray();
            for (auto val : edgesArray) {
                QJsonObject edgeData = val.toObject();
                Edge* newEdge = new Edge(scene);
                newEdge->deserialize(edgeData, hashmap, true);
                scene->addEdge(newEdge);
            }
        }
    }

    void redo() override {
        auto* view = dynamic_cast<NodeEditorGraphicsView*>(scene->graphicsScene()->views().first());
        view->deleteSelected();
    }

private:
    Scene* scene;
    QJsonObject data;
};

// --------------------------------------
// Move Node
// --------------------------------------
class MoveNodeCommand : public QUndoCommand {
public:
    MoveNodeCommand(Node* node, const QPointF& oldPos, const QPointF& newPos,
                    QUndoCommand* parent = nullptr)
        : QUndoCommand("Move Node", parent),
          m_node(node),
          m_oldPos(oldPos),
          m_newPos(newPos) {}

    void undo() override {
        if (m_node) {
            m_node->setPos(m_oldPos.x(), m_oldPos.y());
            m_node->updateConnectedEdges();
        }
    }

    void redo() override {
        if (m_node) {
            m_node->setPos(m_newPos.x(), m_newPos.y());
            m_node->updateConnectedEdges();
        }
    }

private:
    Node* m_node;
    QPointF m_oldPos;
    QPointF m_newPos;
};

// --------------------------------------
// Delete Selected
// --------------------------------------
class DeleteSelectedCommand : public QUndoCommand {
public:
    DeleteSelectedCommand(Scene* scene, const QList<QGraphicsItem*>& selected,
                          QUndoCommand* parent = nullptr)
        : QUndoCommand("Delete Selected", parent), m_scene(scene) 
    {
        for (QGraphicsItem* item : selected) {
            if (auto edgeItem = dynamic_cast<EdgeGraphicsPathItem*>(item)) {
                Edge* edge = edgeItem->getEdge();
                if (edge) {
                    m_edges.append(edge);
                    m_serializedEdges.append(edge->serialize());
                }
            } else if (auto nodeItem = dynamic_cast<NodeGraphicsItem*>(item)) {
                Node* node = nodeItem->getNode();
                if (node) {
                    m_nodes.append(node);
                    m_serializedNodes.append(node->serialize());
                }
            }
        }
    }

    void undo() override {
        if (!m_scene) return;
        std::unordered_map<qint64, Serializable*> hashmap;

        for (const QJsonObject& nodeData : m_serializedNodes) {
            Node* node = new Node(m_scene);
            node->deserialize(nodeData, hashmap, true);
        }
        for (const QJsonObject& edgeData : m_serializedEdges) {
            Edge* edge = new Edge(m_scene);
            edge->deserialize(edgeData, hashmap, true);
        }
    }

    void redo() override {
        for (Node* node : m_nodes) {
            if (node) node->remove();
        }
        for (Edge* edge : m_edges) {
            if (edge) edge->remove();
        }
    }

private:
    Scene* m_scene;
    QList<Node*> m_nodes;
    QList<Edge*> m_edges;
    QList<QJsonObject> m_serializedNodes;
    QList<QJsonObject> m_serializedEdges;
};

#endif // UNDOCOMMANDS_H
