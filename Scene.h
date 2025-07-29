#ifndef SCENE_H
#define SCENE_H

#include <QObject>
#include <QVector>
#include "NodeGraphicsScene.h"

class Node;
class Edge;
struct Theme;

class Scene : public QObject {
    Q_OBJECT

public:
    explicit Scene(QObject *parent = nullptr);

    void addNode(Node* node);
    void addEdge(Edge* edge);
    void removeNode(Node* node);
    void removeEdge(Edge* edge);

    NodeGraphicsScene* graphicsScene() const { return _graphicsScene; }

private:
    void initUI();

    QVector<Node*> nodes;
    QVector<Edge*> edges;

    int sceneWidth;
    int sceneHeight;

    NodeGraphicsScene *_graphicsScene;
};

#endif // SCENE_H
