#include <QJsonArray>
#include <QFile>
#include <QJsonDocument>
#include <QJsonObject>
#include <QDebug>
#include <QtWidgets/qgraphicsview.h>

#include "Scene.h"
#include "Edge.h"
#include "EdgeGraphicsPathItem.h"
#include "Node.h"
#include "NodeEditorGraphicsView.h"
#include "NodeGraphicsItem.h"
#include "Socket.h"
#include "history.h"
#include "UndoCommands.h"

Scene::Scene()
    : sceneWidth(64000), sceneHeight(64000)
{
    initUI();
    history = new QUndoStack();
    QObject::connect(_graphicsScene, &NodeGraphicsScene::itemSelected,
                     [this]() { this->onItemSelected(); });

    QObject::connect(_graphicsScene, &NodeGraphicsScene::itemSelected,
                     [this]() { this->onItemsDeselected(); });
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
    setHasBeenModified(false);
    
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
    std::unordered_map<qint64, Serializable*>& hashmap,
    bool restoreId
    ) {
    qDebug() << "Deserializing data:" << QJsonDocument(data).toJson(QJsonDocument::Indented);

    clearScene();
    hashmap.clear();

    if (restoreId) {
        // Set ID and add to hashmap
        id = static_cast<qint64>(data["id"].toDouble());
        hashmap[id] = this;
    }

    // create nodes
    if (data.contains("nodes") && data["nodes"].isArray()) {
        QJsonArray nodesArray = data["nodes"].toArray();
        for (auto it = nodesArray.begin(); it != nodesArray.end(); ++it) {
            const QJsonValue &nodeVal = *it;
            if (nodeVal.isObject()) {
                QJsonObject nodeObj = nodeVal.toObject();
                Node* node = new Node(this);       // Node constructor takes Scene*
                node->deserialize(nodeObj, hashmap, restoreId);
            }
        }
    }

    // create edges
    if (data.contains("edges") && data["edges"].isArray()) {
        QJsonArray edgesArray = data["edges"].toArray();
        for (auto it = edgesArray.begin(); it != edgesArray.end(); ++it) {
            const QJsonValue &edgeVal = *it;
            if (edgeVal.isObject()) {
                QJsonObject edgeObj = edgeVal.toObject();
                Edge* edge = new Edge(this);       // Edge constructor takes Scene*
                edge->deserialize(edgeObj, hashmap, restoreId);
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
    setHasBeenModified(false);
    return true;
}

bool Scene::loadFromFile(const QString& filename,  QString* errorMsg)
{
    QFile file(filename);
    if (!file.open(QIODevice::ReadOnly | QIODevice::Text)) {
        if (errorMsg)
            *errorMsg = QString("Failed to open file: %1").arg(filename);
        return false;
    }

    QByteArray rawData = file.readAll();
    file.close();

    QJsonParseError parseError;
    QJsonDocument doc = QJsonDocument::fromJson(rawData, &parseError);
    if (parseError.error != QJsonParseError::NoError) {
        if (errorMsg)
            *errorMsg = QString("Failed to parse JSON: %1").arg(parseError.errorString());
        return false;
    }

    if (!doc.isObject()) {
        if (errorMsg)
            *errorMsg = QString("Invalid JSON format: root is not an object");
        return false;
    }

    std::unordered_map<qint64, Serializable*> hashmap;
    deserialize(doc.object(), hashmap);
    setHasBeenModified(false);
    return true;
}

QJsonObject Scene::serializeSelected(bool del)
{
    if ( !graphicsScene()) return QJsonObject();

    qDebug() << "-- COPY TO CLIPBOARD ---";

    QJsonArray selNodes;
    QList<Edge*> selEdges;
    QMap<int, Socket*> selSockets;

    // --- sort edges and nodes ---
    for (QGraphicsItem* item : graphicsScene()->selectedItems()) {
        // Node?
        if (auto* nodeItem = dynamic_cast<NodeGraphicsItem*>(item)){
            selNodes.append(nodeItem->getNode()->serialize());

            for (Socket* socket : nodeItem->getNode()->inputs) {
                selSockets[socket->getId()] = socket;
            }
            for (Socket* socket : nodeItem->getNode()->outputs) {
                selSockets[socket->getId()] = socket;
            }
        }
        // Edge?
        else if (auto* edgeItem = dynamic_cast<EdgeGraphicsPathItem*>(item)) {
            selEdges.append(edgeItem->getEdge());
        }
    }

        qWarning() << "  NODES:" << selNodes;
        qWarning() << "  EDGES count:" << selEdges.size();
        qWarning() << "  SOCKETS count:" << selSockets.size();

    // --- remove invalid edges ---
    QList<Edge*> edgesToRemove;
    for (Edge* edge : selEdges) {
        if (!(selSockets.contains(edge->getStartSocket()->getId()) &&
              selSockets.contains(edge->getEndSocket()->getId()))) {
            qDebug() << "edge" << edge << "is not connected with both sides";
            edgesToRemove.append(edge);
        }
    }
    for (Edge* e : edgesToRemove) {
        selEdges.removeAll(e);
    }

    // --- finalize edges ---
    QJsonArray edgesFinal;
    for (Edge* edge : selEdges) {
        edgesFinal.append(edge->serialize());
    }

    qDebug() << "our final edge list:" << edgesFinal;

    // --- assemble final data ---
    QJsonObject data;
    data["nodes"] = selNodes;
    data["edges"] = edgesFinal;

    // --- handle CUT ---
    if (del) {
            getHistory()->push(
                new CutCommand(this, data, graphicsScene()->selectedItems())
            );
            setHasBeenModified(true);
        }
    return data;
}

void Scene::deserializeFromClipboard(const QJsonObject &data)
{
    getHistory()->push(
            new PasteCommand(this, data)
        );
    setHasBeenModified(true);
}

Node* Scene::getNodeById(qint64 id) const {
    for (Node* n : nodes) {            // adjust the container name if yours differs
        if (n && n->getId() == id) return n;
    }
    return nullptr;
}

Edge* Scene::getEdgeById(qint64 id) const {
    for (Edge* e : edges) {           // adjust the container name if yours differs
        if (e && e->getId() == id) return e;
    }
    return nullptr;
}

bool Scene::hasBeenModified() const{
    return m_hasBeenModified;
}

void Scene::setHasBeenModified(bool value) {
    // If transitioning from false → true
    if (!m_hasBeenModified && value) {
        m_hasBeenModified = true;

        // Notify all registered listeners
        for (auto& callback : m_hasBeenModifiedListeners) {
            if (callback) callback();
        }

    } else {
        m_hasBeenModified = value;
    }
}

void Scene::addHasBeenModifiedListener(const std::function<void()>& callback) {
    m_hasBeenModifiedListeners.push_back(callback);
}

void Scene::addItemSelectedListener(std::function<void()>& callback)
{
    m_itemSelectedListeners.push_back(callback);
}

void Scene::addItemsDeselectedListener(std::function<void()>& callback)
{
    m_itemsDeselectedListeners.push_back(callback);
}

void Scene::resetLastSelectedStates()
{
    for (auto* node : nodes)
        if (node && node->getNodeGraphicsItem())
            node->getNodeGraphicsItem()->setLastSelectedState(false);

    for (auto* edge : edges)
        if (edge && edge->getEdgeGraphicsItem())
            edge->getEdgeGraphicsItem()->setLastSelectedState(false);
}

void Scene::onItemSelected()
{
    qDebug() << "SCENE:: ~onItemSelected";

    QList<QGraphicsItem*> current = getSelectedItems();

    if (current != _graphicsScene->lastSelectedItems)
    {
        _graphicsScene->lastSelectedItems = current;

        // Fire callbacks
        for (auto& cb : m_itemSelectedListeners)
            cb();
    }
}

void Scene::onItemsDeselected()
{
    qDebug() << "SCENE:: ~onItemsDeselected";

    resetLastSelectedStates();

    if (!_graphicsScene->lastSelectedItems.isEmpty())
    {
        _graphicsScene->lastSelectedItems.clear();

        // Fire callbacks
        for (auto& cb : m_itemsDeselectedListeners)
            cb();
    }
}

