#include "NodeEditorGraphicsView.h"
#include <QPainter>
#include <QMouseEvent>
#include <QEvent>
#include <QDebug>
#include <QString>

#include "Node.h"
#include "NodeGraphicsItem.h"
#include "NodeEditorGraphicsView.h"
#include "NodeGraphicsScene.h"
#include "Scene.h"
#include "Socket.h"
#include "SocketGraphicsItem.h"
#include "Edge.h"
#include "EdgeGraphicsPathItem.h"
#include "history.h"
#include "UndoCommands.h"

#include <QMouseEvent>
#include <QGraphicsItem>
#include <QDebug>
#include <QList>

QString debug_modifiers(QInputEvent* event)
{
    QString out = "MODS: ";

    Qt::KeyboardModifiers mods = event->modifiers();

    if (mods & Qt::ShiftModifier)   out += "SHIFT ";
    if (mods & Qt::ControlModifier) out += "CTRL ";
    if (mods & Qt::AltModifier)     out += "ALT ";

    return out;
}

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
    setDragMode(QGraphicsView::RubberBandDrag);
}

void NodeEditorGraphicsView::keyPressEvent(QKeyEvent* event)
{
    /*
    if (event->key() == Qt::Key_Delete) {
        deleteSelected();
    }
    else if ((event->key() == Qt::Key_S) && (event->modifiers() & Qt::ControlModifier)) {
        if (m_grScene && m_grScene->getScene()) {
            m_grScene->getScene()->saveToFile("C:\\Users\\sujan\\Documents\\GitHub\\QNodeEditor\\graph.json.txt");
        }
        event->accept();
    }
    else if ((event->key() == Qt::Key_L) && (event->modifiers() & Qt::ControlModifier)) {
        if (m_grScene && m_grScene->getScene()) {
            m_grScene->getScene()->loadFromFile("C:\\Users\\sujan\\Documents\\GitHub\\QNodeEditor\\graph.json.txt");
        }
        event->accept();
    }
    else if (event->key() == Qt::Key_Z &&
         (event->modifiers() & Qt::ControlModifier) &&
         !(event->modifiers() & Qt::ShiftModifier)) {

        m_grScene->getScene()->getHistory()->undo();
        event->accept();
    }
    else if (event->key() == Qt::Key_Z &&
         (event->modifiers() & Qt::ControlModifier) &&
         (event->modifiers() & Qt::ShiftModifier)) {

        m_grScene->getScene()->getHistory()->redo();
        event->accept();
    }
    else if (event->key() == Qt::Key_H) {
        qDebug() << "HISTORY: len(" << m_grScene->getScene()->getHistory()->getStack().size()
                 << ") -- current_step" << m_grScene->getScene()->getHistory()->getCurrentStep();
        int ix = 0;
        for (const QJsonObject &item : m_grScene->getScene()->getHistory()->getStack()) {
            qDebug() << "#" << ix << "--" << item["desc"].toString();
            ix++;
        }
        event->accept();
    }
    else {
    */
        QGraphicsView::keyPressEvent(event);  // call base class
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
    if (auto obj = dynamic_cast<QObject*>(item))
        qDebug() << "Item type:" << obj->metaObject()->className();

    qDebug() << debug_modifiers(event) << item;
    

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


    if (event->modifiers() & Qt::ShiftModifier)
    {
        qDebug() << "LMB + Shift on" << item;

        QMouseEvent fakeEvent(
            QEvent::MouseButtonPress,
            event->localPos(),
            event->screenPos(),
            Qt::LeftButton,
            event->buttons() | Qt::LeftButton,
            event->modifiers() | Qt::ControlModifier
        );

        QGraphicsView::mousePressEvent(&fakeEvent);
        return;
    }
    QGraphicsView::mousePressEvent(event);
}

void NodeEditorGraphicsView::leftMouseButtonRelease(QMouseEvent* event) {
    QGraphicsItem* item = getItemAtClick(event);

    if (event->modifiers() & Qt::ShiftModifier)
    {
        qDebug() << "LMB Release + Shift on" << item;

        QMouseEvent fakeEvent(
            event->type(),            
            event->localPos(),
            event->screenPos(),
            Qt::LeftButton,
            Qt::NoButton,             
            event->modifiers() | Qt::ControlModifier
        );

        QGraphicsView::mouseReleaseEvent(&fakeEvent);
        return;
    }

    if (mode == MODE_EDGE_DRAG && distanceBetweenClickAndReleaseIsOff(event)) {
        if (edgeDragEnd(item)) return;
    }

    if (dragMode() == QGraphicsView::RubberBandDrag) {
        QList<QGraphicsItem*> selection;
        for (QGraphicsItem* item : m_grScene->selectedItems()) {
            selection.append(item);
        }
        m_grScene->getScene()->getHistory()->push(
            new SelectionChangedCommand(m_grScene->getScene(), previousSelection, selection)
        );

        previousSelection = selection;

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
    previousEdge = socketItem->getSocket()->getConnectedEdge();
    lastStartSocket = socketItem->getSocket();

    dragEdge = new Edge(m_grScene->getScene(), socketItem->getSocket(), nullptr, Edge::EDGE_TYPE_BEZIER);
}

bool NodeEditorGraphicsView::edgeDragEnd(QGraphicsItem* item) {
     mode = MODE_NOOP;
    qDebug() << "Edge Drag END";

    if (auto endSocketItem = dynamic_cast<SocketGraphicsItem*>(item)) {
        if (endSocketItem->getSocket() != lastStartSocket) {

            // capture what needs to be removed
            Edge* conflictingEdge = nullptr;
            if (endSocketItem->getSocket()->hasConnectedEdge())
                conflictingEdge = endSocketItem->getSocket()->getConnectedEdge();

            Edge* prevEdge = previousEdge; // saved in dragStart

            // Push proper undo command that owns this edge + conflicts
            m_grScene->getScene()->getHistory()->push(
                new CreateEdgeCommand(m_grScene->getScene(),
                                      dragEdge,
                                      lastStartSocket,
                                      endSocketItem->getSocket(),
                                      prevEdge,
                                      conflictingEdge)
            );

            dragEdge = nullptr; // ownership now inside command
            return true;
        }
    }

    // cancel preview edge if invalid
    if (dragEdge) {
        dragEdge->remove();
        dragEdge = nullptr;
    }

    // restore previousEdge if drag failed
    if (previousEdge) {
        previousEdge->getStartSocket()->setConnectedEdge(previousEdge);
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
    
    lastSceneMousePosition = mapToScene(event->pos());
    
    emit scenePosChanged(static_cast<int>(lastSceneMousePosition.x()),
                         static_cast<int>(lastSceneMousePosition.y()));

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


void NodeEditorGraphicsView::deleteSelected() {

    QList<QGraphicsItem*> selected = m_grScene->selectedItems();
    if (selected.isEmpty()) return;

    m_grScene->getScene()->getHistory()->push(
        new DeleteSelectedCommand(m_grScene->getScene(), selected)
    );
}




