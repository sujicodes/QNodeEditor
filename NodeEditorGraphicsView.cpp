#include "NodeEditorGraphicsView.h"
#include <QPainter>
#include <QMouseEvent>
#include <QEvent>
#include <QDebug>


#include "NodeEditorGraphicsView.h"
#include "NodeGraphicsScene.h"
#include "Socket.h"
#include "SocketGraphicsItem.h"
#include "Edge.h"
#include "EdgeGraphicsPathItem.h"

#include <QMouseEvent>
#include <QGraphicsItem>
#include <QDebug>

NodeEditorGraphicsView::NodeEditorGraphicsView(NodeGraphicsScene* grScene, QWidget* parent)
    : QGraphicsView(parent), m_grScene(grScene), mode(MODE_NOOP),
      zoomInFactor(1.25), zoomStep(1), zoom(10),
      zoomClamp(true), zoomRange({0, 10})
{
    initUI();
    setScene(m_grScene);
}

void NodeEditorGraphicsView::initUI() {
    setRenderHints(QPainter::Antialiasing |
                   QPainter::TextAntialiasing |
                   QPainter::SmoothPixmapTransform);

    setViewportUpdateMode(QGraphicsView::FullViewportUpdate);
    setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
    setVerticalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
    setTransformationAnchor(QGraphicsView::AnchorUnderMouse);
}

void NodeEditorGraphicsView::mousePressEvent(QMouseEvent* event) {
    if (event->button() == Qt::MiddleButton) {
        middleMouseButtonPress(event);
    } else if (event->button() == Qt::LeftButton) {
        leftMouseButtonPress(event);
    } else if (event->button() == Qt::RightButton) {
        rightMouseButtonPress(event);
    } else {
        QGraphicsView::mousePressEvent(event);
    }
}

void NodeEditorGraphicsView::mouseReleaseEvent(QMouseEvent* event) {
    if (event->button() == Qt::MiddleButton) {
        middleMouseButtonRelease(event);
    } else if (event->button() == Qt::LeftButton) {
        leftMouseButtonRelease(event);
    } else if (event->button() == Qt::RightButton) {
        rightMouseButtonRelease(event);
    } else {
        QGraphicsView::mouseReleaseEvent(event);
    }
}

void NodeEditorGraphicsView::middleMouseButtonPress(QMouseEvent* event) {
    QMouseEvent releaseEvent(QEvent::MouseButtonRelease,
                             event->localPos(), event->windowPos(), event->screenPos(),
                             Qt::LeftButton, Qt::NoButton, event->modifiers());
    QGraphicsView::mouseReleaseEvent(&releaseEvent);

    setDragMode(QGraphicsView::ScrollHandDrag);

    QMouseEvent fakeEvent(QEvent::MouseButtonPress,
                          event->localPos(), event->windowPos(), event->screenPos(),
                          Qt::LeftButton, event->buttons() | Qt::LeftButton, event->modifiers());
    QGraphicsView::mousePressEvent(&fakeEvent);
}

void NodeEditorGraphicsView::middleMouseButtonRelease(QMouseEvent* event) {
    QMouseEvent fakeEvent(QEvent::MouseButtonRelease,
                          event->localPos(), event->windowPos(), event->screenPos(),
                          Qt::LeftButton, event->buttons() & ~Qt::LeftButton, event->modifiers());
    QGraphicsView::mouseReleaseEvent(&fakeEvent);

    setDragMode(QGraphicsView::NoDrag);
}

void NodeEditorGraphicsView::leftMouseButtonPress(QMouseEvent* event) {
    QGraphicsItem* item = getItemAtClick(event);
    lastLeftClickScenePos = mapToScene(event->pos());

    if (auto socketItem = dynamic_cast<SocketGraphicsItem*>(item)) {
        if (mode == MODE_NOOP) {
            mode = MODE_EDGE_DRAG;
            edgeDragStart(socketItem);
            return;
        }
    }

    if (mode == MODE_EDGE_DRAG) {
        if (edgeDragEnd(item)) return;
    }

    QGraphicsView::mousePressEvent(event);
}

void NodeEditorGraphicsView::leftMouseButtonRelease(QMouseEvent* event) {
    QGraphicsItem* item = getItemAtClick(event);

    if (mode == MODE_EDGE_DRAG && distanceBetweenClickAndReleaseIsOff(event)) {
        if (edgeDragEnd(item)) return;
    }

    QGraphicsView::mouseReleaseEvent(event);
}

void NodeEditorGraphicsView::rightMouseButtonPress(QMouseEvent* event) {
    QGraphicsView::mousePressEvent(event);
    QGraphicsItem* item = getItemAtClick(event);
    if (!item) return;

    qDebug() << "RMB DEBUG: Clicked on" << item;
}

void NodeEditorGraphicsView::rightMouseButtonRelease(QMouseEvent* event) {
    QGraphicsView::mouseReleaseEvent(event);
}

QGraphicsItem* NodeEditorGraphicsView::getItemAtClick(QMouseEvent* event) {
    return itemAt(event->pos());
}

void NodeEditorGraphicsView::edgeDragStart(SocketGraphicsItem* socketItem) {
    qDebug() << "Edge Drag Start";
    Socket* sock = socketItem->getSocket();
    if(!sock){
        qDebug() << "not socklewt";
    }
    previousEdge = socketItem->getSocket()->getEdge();
    lastStartSocket = socketItem->getSocket();

    dragEdge = new Edge(m_grScene->getScene(), socketItem->getSocket(), nullptr, Edge::EDGE_TYPE_BEZIER);
    //m_grScene->addItem(dragEdge->getEdgeGraphicsItem());
}

bool NodeEditorGraphicsView::edgeDragEnd(QGraphicsItem* item) {
    mode = MODE_NOOP;

    if (auto endSocketItem = dynamic_cast<SocketGraphicsItem*>(item)) {
        if (previousEdge) previousEdge->remove();

        dragEdge->setStartSocket(lastStartSocket);
        dragEdge->setEndSocket(endSocketItem->getSocket());

        lastStartSocket->setEdge(dragEdge);
        endSocketItem->getSocket()->setEdge(dragEdge);

        dragEdge->updatePositions();
        return true;
    }

    if (dragEdge) {
        dragEdge->remove();
        dragEdge = nullptr;
    }

    if (previousEdge) {
        previousEdge->getStartSocket()->setEdge(previousEdge);
    }

    return false;
}

bool NodeEditorGraphicsView::distanceBetweenClickAndReleaseIsOff(QMouseEvent* event) {
    QPointF newScenePos = mapToScene(event->pos());
    QPointF delta = newScenePos - lastLeftClickScenePos;
    double distSq = delta.x() * delta.x() + delta.y() * delta.y();
    return distSq > (EDGE_DRAG_START_THRESHOLD * EDGE_DRAG_START_THRESHOLD);
}

void NodeEditorGraphicsView::mouseMoveEvent(QMouseEvent* event) {
    if (mode == MODE_EDGE_DRAG){ //&& dragEdge/) {
        QPointF pos = mapToScene(event->pos());
        dragEdge->getEdgeGraphicsItem()->setDestination(pos);
        dragEdge->getEdgeGraphicsItem()->update();
    }

    QGraphicsView::mouseMoveEvent(event);
}

void NodeEditorGraphicsView::wheelEvent(QWheelEvent* event) {
    double zoomOutFactor = 1.0 / zoomInFactor;
    double zoomFactor = (event->angleDelta().y() > 0) ? zoomInFactor : zoomOutFactor;

    zoom += (event->angleDelta().y() > 0) ? zoomStep : -zoomStep;

    bool clamped = false;
    if (zoom < zoomRange.first) { zoom = zoomRange.first; clamped = true; }
    if (zoom > zoomRange.second) { zoom = zoomRange.second; clamped = true; }

    if (!clamped || !zoomClamp) {
        scale(zoomFactor, zoomFactor);
    }
}
