#include <QJsonArray>
#include <QFile>
#include <QJsonDocument>
#include <QJsonObject>
#include <QDebug>
#include <QtWidgets/qgraphicsview.h>
#include <cmath>
#include <qgraphicssceneevent.h>

#include "NodeEditorGraphicsScene.h"
#include "Edge.h"
#include "EdgeGraphicsPathItem.h"
#include "NodeItem.h"
#include "NodeEditorGraphicsView.h"
#include "SocketItem.h"
#include "UndoCommands.h"
#include "Theme.h"

NodeEditorGraphicsScene::NodeEditorGraphicsScene()
    : QGraphicsScene(),
    sceneWidth(64000),
    sceneHeight(64000),
    gridSize(20),
    gridSquares(5),
    colorBackground("#393939"),
    colorLight("#2f2f2f"),
    colorDark("#292929"),
    penLight(colorLight),
    penDark(colorDark)
{
    penLight.setWidth(1);
    penDark.setWidth(2);
    setGraphicsScene(sceneWidth, sceneHeight);
    setBackgroundBrush(colorBackground);

    history = new QUndoStack();

    QObject::connect(this, &NodeEditorGraphicsScene::itemSelected,    this, &NodeEditorGraphicsScene::onItemSelected);
    QObject::connect(this, &NodeEditorGraphicsScene::itemsDeselected, this, &NodeEditorGraphicsScene::onItemsDeselected);
}

void NodeEditorGraphicsScene::setGraphicsScene(int width, int height) {
    sceneWidth  = width;
    sceneHeight = height;
    setSceneRect(-width / 2, -height / 2, width, height);
}

void NodeEditorGraphicsScene::drawBackground(QPainter *painter, const QRectF &rect) {
    QGraphicsScene::drawBackground(painter, rect);

    if (!Theme::instance().gridDisplayOn) return;

    int left   = std::floor(rect.left());
    int right  = std::ceil(rect.right());
    int top    = std::floor(rect.top());
    int bottom = std::ceil(rect.bottom());

    int firstLeft = left - (left % gridSize);
    int firstTop  = top  - (top  % gridSize);

    QVector<QLine> linesLight, linesDark;

    for (int x = firstLeft; x < right; x += gridSize) {
        if (x % (gridSize * gridSquares) != 0)
            linesLight.append(QLine(x, top, x, bottom));
        else
            linesDark.append(QLine(x, top, x, bottom));
    }

    for (int y = firstTop; y < bottom; y += gridSize) {
        if (y % (gridSize * gridSquares) != 0)
            linesLight.append(QLine(left, y, right, y));
        else
            linesDark.append(QLine(left, y, right, y));
    }

    painter->setPen(penLight);
    painter->drawLines(linesLight);
    painter->setPen(penDark);
    painter->drawLines(linesDark);
}

void NodeEditorGraphicsScene::dragMoveEvent(QGraphicsSceneDragDropEvent* event) {
    event->accept();
}

NodeItem* NodeEditorGraphicsScene::createNode(QString type) {
    return NodeRegistry::instance().createNode(type, this);
}

void NodeEditorGraphicsScene::addNode(NodeItem* node)    { nodes.append(node); }
void NodeEditorGraphicsScene::addEdge(Edge* edge)        { edges.append(edge); }
void NodeEditorGraphicsScene::removeNode(NodeItem* node) { nodes.removeAll(node); }
void NodeEditorGraphicsScene::removeEdge(Edge* edge)     { edges.removeAll(edge); }

void NodeEditorGraphicsScene::clearScene() {
    while (!nodes.empty()) {
        if (nodes.front()) nodes.front()->remove();
    }
    setHasBeenModified(false);
}

QJsonObject NodeEditorGraphicsScene::serialize() const {
    QJsonObject obj;
    obj["id"]           = id;
    obj["scene_width"]  = sceneWidth;
    obj["scene_height"] = sceneHeight;

    QJsonArray nodesArray;
    for (const NodeItem* node : nodes)
        if (node) nodesArray.append(node->serialize());
    obj["nodes"] = nodesArray;

    QJsonArray edgesArray;
    for (const Edge* edge : edges)
        if (edge) edgesArray.append(edge->serialize());
    obj["edges"] = edgesArray;

    return obj;
}

void NodeEditorGraphicsScene::deserialize(
    const QJsonObject& data,
    std::unordered_map<qint64, Serializable*>& hashmap,
    bool restoreId)
{
    qDebug() << "Deserializing data:" << QJsonDocument(data).toJson(QJsonDocument::Indented);
    clearScene();
    hashmap.clear();

    if (restoreId) {
        id = static_cast<qint64>(data["id"].toDouble());
        hashmap[id] = this;
    }

    if (data.contains("nodes") && data["nodes"].isArray()) {
        for (const QJsonValue& nodeVal : data["nodes"].toArray()) {
            if (nodeVal.isObject()) {
                QJsonObject nodeObj = nodeVal.toObject();
                NodeItem* node = createNode(nodeObj["type"].toString());
                node->deserialize(nodeObj, hashmap, restoreId);
            }
        }
    }

    if (data.contains("edges") && data["edges"].isArray()) {
        for (const QJsonValue& edgeVal : data["edges"].toArray()) {
            if (edgeVal.isObject()) {
                QJsonObject edgeObj = edgeVal.toObject();
                Edge* edge = new Edge(this);
                edge->deserialize(edgeObj, hashmap, restoreId);
            }
        }
    }
}

bool NodeEditorGraphicsScene::saveToFile(const QString& filename) {
    QFile file(filename);
    if (!file.open(QIODevice::WriteOnly | QIODevice::Text)) {
        qWarning() << "Failed to open file for writing:" << filename;
        return false;
    }
    file.write(QJsonDocument(serialize()).toJson(QJsonDocument::Indented));
    file.close();
    qDebug() << "Saving to" << filename << "was successful.";
    setHasBeenModified(false);
    return true;
}

bool NodeEditorGraphicsScene::loadFromFile(const QString& filename, QString* errorMsg) {
    QFile file(filename);
    if (!file.open(QIODevice::ReadOnly | QIODevice::Text)) {
        if (errorMsg) *errorMsg = QString("Failed to open file: %1").arg(filename);
        return false;
    }

    QByteArray rawData = file.readAll();
    file.close();

    QJsonParseError parseError;
    QJsonDocument doc = QJsonDocument::fromJson(rawData, &parseError);
    if (parseError.error != QJsonParseError::NoError) {
        if (errorMsg) *errorMsg = QString("Failed to parse JSON: %1").arg(parseError.errorString());
        return false;
    }
    if (!doc.isObject()) {
        if (errorMsg) *errorMsg = "Invalid JSON format: root is not an object";
        return false;
    }

    std::unordered_map<qint64, Serializable*> hashmap;
    deserialize(doc.object(), hashmap);
    setHasBeenModified(false);
    return true;
}

QJsonObject NodeEditorGraphicsScene::serializeSelected(bool del) {
    qDebug() << "-- COPY TO CLIPBOARD ---";
    QJsonArray selNodes;
    QList<Edge*> selEdges;
    QMap<int, SocketItem*> selSockets;

    for (QGraphicsItem* item : selectedItems()) {
        if (auto* nodeItem = dynamic_cast<NodeItem*>(item)) {
            selNodes.append(nodeItem->serialize());
            for (SocketItem* socket : nodeItem->getInputSockets())
                selSockets[socket->getId()] = socket;
            for (SocketItem* socket : nodeItem->getOutputSockets())
                selSockets[socket->getId()] = socket;
        } else if (auto* edgeItem = dynamic_cast<EdgeGraphicsPathItem*>(item)) {
            selEdges.append(edgeItem->getEdge());
        }
    }

    QList<Edge*> edgesToRemove;
    for (Edge* edge : selEdges) {
        if (!(selSockets.contains(edge->getStartSocket()->getId()) &&
              selSockets.contains(edge->getEndSocket()->getId()))) {
            edgesToRemove.append(edge);
        }
    }
    for (Edge* e : edgesToRemove) selEdges.removeAll(e);

    QJsonArray edgesFinal;
    for (Edge* edge : selEdges) edgesFinal.append(edge->serialize());

    QJsonObject data;
    data["nodes"] = selNodes;
    data["edges"] = edgesFinal;

    if (del) {
        history->push(new CutCommand(this, data, selectedItems()));
        setHasBeenModified(true);
    }
    return data;
}

void NodeEditorGraphicsScene::deserializeFromClipboard(const QJsonObject &data) {
    history->push(new PasteCommand(this, data));
    setHasBeenModified(true);
}

NodeItem* NodeEditorGraphicsScene::getNodeById(qint64 id) const {
    for (NodeItem* n : nodes)
        if (n && n->getId() == id) return n;
    return nullptr;
}

Edge* NodeEditorGraphicsScene::getEdgeById(qint64 id) const {
    for (Edge* e : edges)
        if (e && e->getId() == id) return e;
    return nullptr;
}

bool NodeEditorGraphicsScene::hasBeenModified() const { return m_hasBeenModified; }

void NodeEditorGraphicsScene::setHasBeenModified(bool value) {
    if (!m_hasBeenModified && value) {
        m_hasBeenModified = true;
        for (auto& cb : m_hasBeenModifiedListeners)
            if (cb) cb();
    } else {
        m_hasBeenModified = value;
    }
}

void NodeEditorGraphicsScene::addHasBeenModifiedListener(const std::function<void()>& cb) { m_hasBeenModifiedListeners.push_back(cb); }
void NodeEditorGraphicsScene::addItemSelectedListener(const std::function<void()>& cb)    { m_itemSelectedListeners.push_back(cb); }
void NodeEditorGraphicsScene::addItemsDeselectedListener(const std::function<void()>& cb) { m_itemsDeselectedListeners.push_back(cb); }

void NodeEditorGraphicsScene::resetLastSelectedStates() {
    for (auto* node : nodes)
        if (node) node->setLastSelectedState(false);
    for (auto* edge : edges)
        if (edge && edge->getEdgeGraphicsItem())
            edge->getEdgeGraphicsItem()->setLastSelectedState(false);
}

void NodeEditorGraphicsScene::onItemSelected() {
    QList<QGraphicsItem*> current = selectedItems();
    if (current != lastSelectedItems) {
        lastSelectedItems = current;
        for (auto& cb : m_itemSelectedListeners) cb();
    }
}

void NodeEditorGraphicsScene::onItemsDeselected() {
    resetLastSelectedStates();
    if (!lastSelectedItems.isEmpty()) {
        lastSelectedItems.clear();
        for (auto& cb : m_itemsDeselectedListeners) cb();
    }
}

QGraphicsView* NodeEditorGraphicsScene::getView() {
    if (views().isEmpty()) return nullptr;
    return views().first();
}
