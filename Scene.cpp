#include <QJsonArray>
#include <QFile>
#include <QJsonDocument>
#include <QJsonObject>
#include <QDebug>

#include "Scene.h"
#include "Edge.h"
#include "Node.h"
#include "history.h"

Scene::Scene()
    : sceneWidth(64000), sceneHeight(64000)
{
    initUI();
    history = new History(this);
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

void Scene::clearScene() {
    while (!nodes.empty()) {
        if (nodes.front()) {
            nodes.front()->remove();
        }
    }
}

QJsonObject Scene::serialize() const {
    QJsonObject obj;
    obj["id"] = id;
    obj["scene_width"] = sceneWidth;
    obj["scene_height"] = sceneHeight;

    // Serialize nodes
    QJsonArray nodesArray;
    for (const Node* node : nodes) {
        if (node) {
            nodesArray.append(node->serialize());
        }
    }
    obj["nodes"] = nodesArray;

    // Serialize edges
    QJsonArray edgesArray;
    for (const Edge* edge : edges) {
        if (edge) {
            edgesArray.append(edge->serialize());
        }
    }
    obj["edges"] = edgesArray;

    return obj;
}
void Scene::deserialize(
    const QJsonObject& data,
    std::unordered_map<qint64, Serializable*>& hashmap
    ) {
    qDebug() << "Deserializing data:" << QJsonDocument(data).toJson(QJsonDocument::Indented);

    clearScene();
    hashmap.clear();

    id = data["id"].toDouble();

    // create nodes
    if (data.contains("nodes") && data["nodes"].isArray()) {
        QJsonArray nodesArray = data["nodes"].toArray();
        for (const QJsonValue& nodeVal : nodesArray) {
            if (nodeVal.isObject()) {
                QJsonObject nodeObj = nodeVal.toObject();
                Node* node = new Node(this);       // Node constructor takes Scene*
                node->deserialize(nodeObj, hashmap);
                nodes.push_back(node);             // keep track of it in Scene
            }
        }
    }

    // create edges
    if (data.contains("edges") && data["edges"].isArray()) {
        QJsonArray edgesArray = data["edges"].toArray();
        for (const QJsonValue& edgeVal : edgesArray) {
            if (edgeVal.isObject()) {
                QJsonObject edgeObj = edgeVal.toObject();
                Edge* edge = new Edge(this);       // Edge constructor takes Scene*
                edge->deserialize(edgeObj, hashmap);
                edges.push_back(edge);             // keep track of it in Scene
            }
        }
    }
}

bool Scene::saveToFile(const QString& filename)
{
    QFile file(filename);
    if (!file.open(QIODevice::WriteOnly | QIODevice::Text)) {
        qWarning() << "Failed to open file for writing:" << filename;
        return false;
    }

    // serialize() must return QJsonObject containing "nodes" and "edges"
    QJsonDocument doc(serialize());
    file.write(doc.toJson(QJsonDocument::Indented));
    file.close();

    qDebug() << "Saving to" << filename << "was successful.";
    return true;
}

bool Scene::loadFromFile(const QString& filename)
{
    QFile file(filename);
    if (!file.open(QIODevice::ReadOnly | QIODevice::Text)) {
        qWarning() << "Failed to open file for reading:" << filename;
        return false;
    }

    QByteArray rawData = file.readAll();
    file.close();

    QJsonParseError parseError;
    QJsonDocument doc = QJsonDocument::fromJson(rawData, &parseError);
    if (parseError.error != QJsonParseError::NoError) {
        qWarning() << "Failed to parse JSON:" << parseError.errorString();
        return false;
    }

    if (!doc.isObject()) {
        qWarning() << "Invalid JSON format: root is not an object";
        return false;
    }
    std::unordered_map<qint64, Serializable*> hashmap = {};

    // deserialize() must accept a QJsonObject to restore nodes/edges
    deserialize(doc.object(), hashmap);
    return true;
}
