#ifndef NODEEDITORGRAPHICSVIEW_H
#define NODEEDITORGRAPHICSVIEW_H

#include <QGraphicsView>
#include <QGraphicsScene>
#include <QWidget>

class Edge;
class Socket;

class NodeEditorGraphicsView : public QGraphicsView {
    Q_OBJECT

public:
    NodeEditorGraphicsView(QGraphicsScene* grScene, QWidget* parent = nullptr);

protected:
    void initUI();

    void mousePressEvent(QMouseEvent* event) override;
    void mouseReleaseEvent(QMouseEvent* event) override;
    void wheelEvent(QWheelEvent* event) override;

    void middleMouseButtonPress(QMouseEvent* event);
    void middleMouseButtonRelease(QMouseEvent* event);
    void leftMouseButtonPress(QMouseEvent* event);
    void leftMouseButtonRelease(QMouseEvent* event);
    void rightMouseButtonPress(QMouseEvent* event);
    void rightMouseButtonRelease(QMouseEvent* event);

    QGraphicsItem* getItemAtClick(QMouseEvent* event);
    void edgeDragStart(QGraphicsItem* item);
    bool edgeDragEnd(QGraphicsItem* item);
    bool distanceBetweenClickAndReleaseIsOff(QMouseEvent* event);

private:
    static const int MODE_NOOP = 1;
    static const int MODE_EDGE_DRAG = 2;
    static const int EDGE_DRAG_START_THRESHOLD = 10;

    QGraphicsScene* m_grScene;

    int mode = MODE_NOOP;
    float zoomInFactor;
    int zoomStep;
    int zoom;
    bool zoomClamp;
    std::pair<int, int> zoomRange;

    QPointF lastLeftClickScenePos;

    Edge* dragEdge;
    Edge* previousEdge;
    Socket* lastStartSocket;
};

#endif // NODEEDITORGRAPHICSVIEW_H
