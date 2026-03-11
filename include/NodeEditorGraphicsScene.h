#ifndef NODEEDITORGRAPHICSSCENE_H
#define NODEEDITORGRAPHICSSCENE_H

#include <QGraphicsScene>
#include <QObject>
#include <QVector>
#include <QUndoStack>
#include <QPen>
#include <qevent.h>
#include "Serializable.h"

class NodeItem;
class Edge;

class NodeEditorGraphicsScene : public QGraphicsScene, public Serializable {
    Q_OBJECT
    friend class NodeEditorGraphicsView;

public:
    NodeEditorGraphicsScene();

    void setGraphicsScene(int width, int height);

    NodeItem* createNode(QString type);
    void addNode(NodeItem* node);
    void addEdge(Edge* edge);
    void removeNode(NodeItem* node);
    void removeEdge(Edge* edge);

    const QVector<Edge*>& getEdges() const { return edges; }
    const QVector<NodeItem*>& getNodes() const { return nodes; }
    NodeEditorGraphicsScene* graphicsScene() { return this; }
    QUndoStack* getHistory() const { return history; }

    void clearScene();
    bool loadFromFile(const QString& filename, QString* errorMsg);
    bool saveToFile(const QString& filename);

    QJsonObject serialize() const override;
    void deserialize(
        const QJsonObject& data,
        std::unordered_map<qint64, Serializable*>& hashmap,
        bool restoreId = true
        ) override;

    QJsonObject serializeSelected(bool del = true);
    void deserializeFromClipboard(const QJsonObject& data);

    NodeItem* getNodeById(qint64 id) const;
    Edge* getEdgeById(qint64 id) const;

    bool hasBeenModified() const;
    void setHasBeenModified(bool value);
    void addHasBeenModifiedListener(const std::function<void()>& callback);
    void addItemSelectedListener(const std::function<void()>& callback);
    void addItemsDeselectedListener(const std::function<void()>& callback);

    QList<QGraphicsItem*> getSelectedItems() const { return selectedItems(); }
    QList<QGraphicsItem*> getLastSelectedItems() { return lastSelectedItems; }
    void onItemSelected();
    void onItemsDeselected();
    void resetLastSelectedStates();
    QGraphicsView* getView();

signals:
    void itemSelected();
    void itemsDeselected();

protected:
    void drawBackground(QPainter *painter, const QRectF &rect) override;
    void dragMoveEvent(QGraphicsSceneDragDropEvent* event) override;

private:
    // Scene data
    QVector<NodeItem*> nodes;
    QVector<Edge*> edges;
    int sceneWidth;
    int sceneHeight;
    QUndoStack* history;
    bool m_hasBeenModified = false;
    QList<QGraphicsItem*> lastSelectedItems;
    std::vector<std::function<void()>> m_itemSelectedListeners;
    std::vector<std::function<void()>> m_itemsDeselectedListeners;
    std::vector<std::function<void()>> m_hasBeenModifiedListeners;

    // Grid drawing
    int gridSize;
    int gridSquares;
    QColor colorBackground, colorLight, colorDark;
    QPen penLight, penDark;
};

#endif // NODEEDITORGRAPHICSSCENE_H
