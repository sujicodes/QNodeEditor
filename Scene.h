#ifndef SCENE_H
#define SCENE_H

#include <QObject>
#include <QVector>
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
    History*  getHistory() const { return history; }
    void clearScene();

    bool loadFromFile(const QString& filename);
    bool saveToFile(const QString& filename);

    QJsonObject serialize() const override;
    void deserialize(
        const QJsonObject& data,
        std::unordered_map<qint64, Serializable*>& hashmap,
        bool restoreId = true
    ) override;

    QJsonObject serializeSelected(bool del = true);
    void deserializeFromClipboard(const QJsonObject& data);
    

private:
    void initUI();

    QVector<Node*> nodes;
    QVector<Edge*> edges;

    int sceneWidth;
    int sceneHeight;
    History* history;

    NodeGraphicsScene* _graphicsScene;
};

#endif // SCENE_H
