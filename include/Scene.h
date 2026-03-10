#ifndef SCENE_H
#define SCENE_H

#include <QObject>
#include <QVector>
#include <QUndoStack>
#include <qevent.h>
#include "NodeGraphicsScene.h"
#include "Serializable.h"

class NodeItem;
class Edge;
class Theme;

class Scene : public Serializable
{

    public:

        Scene();

        NodeItem* createNode(QString type);
        void addNode(NodeItem* node);
        void addEdge(Edge* edge);
        void removeNode(NodeItem* node);
        void removeEdge(Edge* edge);
        const QVector<Edge*>& getEdges() const { return edges; }
        const QVector<NodeItem*>& getNodes() const { return nodes; }

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

        NodeItem* getNodeById(qint64 id) const;
        Edge* getEdgeById(qint64 id) const;

        bool hasBeenModified() const;
        void setHasBeenModified(bool value);

        void addHasBeenModifiedListener(const std::function<void()>& callback);
        void addItemSelectedListener(const std::function<void()>& callback);
        void addItemsDeselectedListener(const std::function<void()>& callback);

        QList<QGraphicsItem*> getSelectedItems() const { return _graphicsScene->selectedItems(); }

        void onItemSelected();
        void onItemsDeselected();
        
        void resetLastSelectedStates();
        QGraphicsView* getView();

    private:

        void initUI();

        QVector<NodeItem*> nodes;
        QVector<Edge*> edges;
        int sceneWidth;
        int sceneHeight;
        QUndoStack* history;

        NodeGraphicsScene* _graphicsScene;
        bool m_hasBeenModified;
        std::vector<std::function<void()>> m_itemSelectedListeners;
        std::vector<std::function<void()>> m_itemsDeselectedListeners;
        std::vector<std::function<void()>> m_hasBeenModifiedListeners;
};

#endif // SCENE_H
