#ifndef SCENE_H
#define SCENE_H

#include <QObject>
#include <QVector>
#include "NodeGraphicsScene.h"
#include "Serializable.h"

class Node;
class Edge;
struct Theme;

class Scene : public Serializable {

public:
    Scene();

    void addNode(Node* node);
    void addEdge(Edge* edge);
    void removeNode(Node* node);
    void removeEdge(Edge* edge);

    NodeGraphicsScene* graphicsScene() const { return _graphicsScene; }
    void clearScene();

    bool loadFromFile(const QString& filename);
    bool saveToFile(const QString& filename);

    QJsonObject serialize() const override;
    void deserialize(
        const QJsonObject& data,
        std::unordered_map<qint64, Serializable*>& hashmap

    ) override;
    

private:
    void initUI();

    QVector<Node*> nodes;
    QVector<Edge*> edges;

    int sceneWidth;
    int sceneHeight;

    NodeGraphicsScene *_graphicsScene;
};

#endif // SCENE_H
