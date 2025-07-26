#include "Scene.h"

Scene::Scene(QObject *parent)
    : QObject(parent), sceneWidth(64000), sceneHeight(64000)
{
    initUI();
}

void Scene::initUI()
{
    _graphicsScene = new NodeGraphicsScene(this);
    _graphicsScene->setGraphicsScene(sceneWidth, sceneHeight);  // You’ll need to add this method in your scene class
}

void Scene::addNode(Node* node)
{
    nodes.append(node);
}

void Scene::addEdge(Edge* edge)
{
    edges.append(edge);
}

void Scene::removeNode(Node* node)
{
    nodes.removeAll(node);
}

void Scene::removeEdge(Edge* edge)
{
    edges.removeAll(edge);
}
