#ifndef NODEEDITORGRAPHICSVIEW_H
#define NODEEDITORGRAPHICSVIEW_H

#include <QGraphicsView>
#include <QGraphicsScene>
#include <QWidget>
#include <QMap>

class Edge;
class Socket;
class SocketGraphicsItem;
class NodeGraphicsScene;
class NodeGraphicsItem;

class NodeEditorGraphicsView : public QGraphicsView {
    Q_OBJECT

public:
    NodeEditorGraphicsView(NodeGraphicsScene* grScene, QWidget* parent = nullptr);
    void deleteSelected();
    QPointF getLastSceneMousePosition() const { return lastSceneMousePosition;}

signals:
    void scenePosChanged(int x, int y);

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
    void edgeDragStart(SocketGraphicsItem* item);
    bool edgeDragEnd(QGraphicsItem* item);
    bool distanceBetweenClickAndReleaseIsOff(QMouseEvent* event);
    void onSelectionChanged();

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

    Edge* dragEdge;
    Edge* previousEdge;
    Socket* lastStartSocket;
    //QList<QGraphicsItem*> previousSelection;

    QMap<qint64, QPair<QPointF, QPointF>> m_moveData;
    QList<NodeGraphicsItem*> m_draggedNodes;
    bool m_nodesAreDragging = false;

    QSet<qint64> previousNodeIds;
    QSet<qint64> previousEdgeIds;

};

#endif // NODEEDITORGRAPHICSVIEW_H
