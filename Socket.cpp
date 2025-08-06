#include "Socket.h"
#include "Node.h"
#include "SocketGraphicsItem.h"
#include "NodeGraphicsItem.h"


Socket::Socket(Node* node, int index, int position)
    : node(node), index(index), position(position)
{

    grSocket = new SocketGraphicsItem(this);

    // Position the socket based on the node's layout logic
    std::pair<float, float> pos = node->getSocketPosition(index, position);
    grSocket->setPos(QPointF(pos.first, pos.second));
}

QPointF Socket::getSocketPosition() const {

    std::pair<float, float> posPair = node->getSocketPosition(index, position);
    QPointF res(posPair.first, posPair.second);

    return res;
}

void Socket::setEdge(Edge* edge) {
    this->edge = edge;
}


bool Socket::hasEdge() const
{
    return edge != nullptr;
}

Edge* Socket::getEdge() const
{
    return edge;
}
