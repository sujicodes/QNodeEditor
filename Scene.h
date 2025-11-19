#ifndef SCENE_H
#define SCENE_H

#include <QObject>
#include <QVector>
#include <QUndoStack>
#include "NodeGraphicsScene.h"
#include "Serializable.h"

class Node;
class Edge;
class Theme;
class History;

class Scene : public Serializable {

public:
    Scene();

    void addNode(Node* node);
    void addEdge(Edge* edge);
    void removeNode(Node* node);
    void removeEdge(Edge* edge);
    const QVector<Edge*>& getEdges() const { return edges; }
    const QVector<Node*>& getNodes() const { return nodes; }

    NodeGraphicsScene* graphicsScene() const { return _graphicsScene; }
    QUndoStack*  getHistory() const { return history; }
    void clearScene();

    bool loadFromFile(const QString& filename,  QString* errorMsg);
    bool saveToFile(const QString& filename);

    QJsonObject serialize() const override;
    void deserialize(
        const QJsonObject& data,
        std::unordered_map<qint64, Serializable*>& hashmap,
        bool restoreId = true
    ) override;

    QJsonObject serializeSelected(bool del = true);
    void deserializeFromClipboard(const QJsonObject& data);

    Node* getNodeById(qint64 id) const;
    Edge* getEdgeById(qint64 id) const;

    bool hasBeenModified() const;
    void setHasBeenModified(bool value);
    void addHasBeenModifiedListener(const std::function<void()>& callback);
    

private:
    void initUI();

    QVector<Node*> nodes;
    QVector<Edge*> edges;
    int sceneWidth;
    int sceneHeight;
    QUndoStack* history;

    NodeGraphicsScene* _graphicsScene;
    bool m_hasBeenModified;
    std::vector<std::function<void()>> m_hasBeenModifiedListeners;
};

#endif // SCENE_H
