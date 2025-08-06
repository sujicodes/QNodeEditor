#include "Edge.h"

#include "Edge.h"
#include "DirectEdgeGraphicsPathItem.h"
#include "BezierEdgeGraphicsPathItem.h"
#include "NodeGraphicsItem.h"
#include "Scene.h"
#include "Socket.h"
#include "Node.h"

Edge::Edge(Scene* scene, Socket* startSocket, Socket* endSocket, int type)
    : scene(scene), startSocket(startSocket), endSocket(endSocket)

{
    startSocket->setEdge(this);
    if (endSocket != nullptr) {
        endSocket->setEdge(this);
    }

    if (type == EDGE_TYPE_DIRECT){
        grEdge = new DirectEdgeGraphicsPathItem(this);
    } else {
        grEdge = new BezierEdgeGraphicsPathItem(this);
    }
    updatePositions();
    scene->graphicsScene()->addItem(grEdge);
}

Edge::~Edge() {
    remove();
}

void Edge::updatePositions() {
    QPointF sourcePos = startSocket->getSocketPosition();
    QPointF nodePos = startSocket->getNode()->getNodeGraphicsItem()->pos();
    sourcePos.rx() += nodePos.x();
    sourcePos.ry() += nodePos.y();
    grEdge->setSource(sourcePos);

    if (endSocket) {
        QPointF endPos = endSocket->getSocketPosition();
        QPointF endNodePos = endSocket->getNode()->getNodeGraphicsItem()->pos();
        endPos.rx() += endNodePos.x();
        endPos.ry() += endNodePos.y();
        grEdge->setDestination(endPos);
    } else {
        grEdge->setDestination(sourcePos);
    }

#ifdef QT_DEBUG
    qDebug() << " SS:" << startSocket;
    qDebug() << " ES:" << endSocket;
#endif

    grEdge->updatePath();
}

void Edge::removeFromSockets() {
    if (startSocket) startSocket->setEdge();
    if (endSocket) endSocket->setEdge();
    startSocket = nullptr;
    endSocket = nullptr;
}

void Edge::remove() {
    removeFromSockets();
    if (scene && grEdge) {
        scene->graphicsScene()->removeItem(grEdge);
    }
    delete grEdge;
    grEdge = nullptr;
    if (scene) scene->removeEdge(this);
}
