#ifndef NODEEDITORGRAPHICSVIEW_H
#define NODEEDITORGRAPHICSVIEW_H

#include "NodeSelectorWidget.h"
#include <QGraphicsView>
#include <QGraphicsScene>
#include <QWidget>
#include <QMap>
#include <QSet>

class Edge;
class SocketItem;
class NodeGraphicsScene;
class NodeItem;

class NodeEditorGraphicsView : public QGraphicsView
{
    Q_OBJECT

    public:

        NodeEditorGraphicsView(NodeGraphicsScene* grScene, QWidget* parent = nullptr);
        void deleteSelected();
        QPointF getLastSceneMousePosition() const { return lastSceneMousePosition;}
        void addDragEnterListener(std::function<void(QDragEnterEvent*)> callback);
        void addDropListener(std::function<void(QDropEvent*)> callback);

    signals:

        void scenePosChanged(int x, int y);
        void nodeDragEntered(QDragEnterEvent*);
        void nodeDropped(QDropEvent*);

    protected:

        void initUI();

        void keyPressEvent(QKeyEvent* event);
        void mousePressEvent(QMouseEvent* event) override;
        void mouseReleaseEvent(QMouseEvent* event) override;
        void wheelEvent(QWheelEvent* event) override;

        void middleMouseButtonPress(QMouseEvent* event);
        void middleMouseButtonRelease(QMouseEvent* event);
        void mouseMoveEvent(QMouseEvent* event);
        void leftMouseButtonPress(QMouseEvent* event);
        void leftMouseButtonRelease(QMouseEvent* event);
        void rightMouseButtonPress(QMouseEvent* event);
        void rightMouseButtonRelease(QMouseEvent* event);

        QGraphicsItem* getItemAtClick(QMouseEvent* event);
        void edgeDragStart(SocketItem* item);
        bool edgeDragEnd(QGraphicsItem* item);
        bool distanceBetweenClickAndReleaseIsOff(QMouseEvent* event);
        void onSelectionChanged();

        void dragEnterEvent(QDragEnterEvent* event) override;
        void dropEvent(QDropEvent* event) override;

    private:

        static const int MODE_NOOP = 1;
        static const int MODE_EDGE_DRAG = 2;
        static const int EDGE_DRAG_START_THRESHOLD = 10;

        NodeGraphicsScene* m_grScene;

        int mode = MODE_NOOP;
        float zoomInFactor;
        int zoomStep;
        int zoom;
        bool zoomClamp;
        std::pair<int, int> zoomRange;

        QPointF lastLeftClickScenePos;
        QPointF lastSceneMousePosition;

        Edge* dragEdge = nullptr;
        Edge* previousEdge = nullptr;
        SocketItem* dragStartSocket = nullptr;;
        //QList<QGraphicsItem*> previousSelection;

        QMap<qint64, QPair<QPointF, QPointF>> m_moveData;
        QList<NodeItem*> m_draggedNodes;
        bool m_nodesAreDragging = false;

        QSet<qint64> previousNodeIds;
        QSet<qint64> previousEdgeIds;
        NodePopupWidget *nodePopup;

        QList<std::function<void(QDragEnterEvent*)>> m_dragEnterListeners;
        QList<std::function<void(QDropEvent*)>> m_dropListeners;
};

#endif // NODEEDITORGRAPHICSVIEW_H