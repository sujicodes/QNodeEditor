#include "socket.h"
#include "node.h"
#include "SocketGraphicsItem.h"
#include "NodeGraphicsItem.h"


Socket::Socket(Node* node, int index, int position)
    : node(node), index(index), position(position)
{
    if (DEBUG)
        qDebug() << "Socket -- creating with index" << index << "position" << static_cast<int>(position) << "for node" << node;

    grSocket = new SocketGraphicsItem(node->getNodeGraphicsItem());

    // Position the socket based on the node's layout logic
    std::pair<float, float> pos = node->getSocketPosition(index, position);
    grSocket->setPos(QPointF(pos.first, pos.second));
}

QPointF Socket::getSocketPosition() const {
    if (DEBUG)
        qDebug() << "  GSP:" << index << static_cast<int>(position) << "node:" << node;

    std::pair<float, float> posPair = node->getSocketPosition(index, position);
    QPointF res(posPair.first, posPair.second);

    if (DEBUG)
        qDebug() << "  result:" << res;

    return res;
}

void Socket::setConnectedEdge(Edge* edge) {
    this->edge = edge;
}

Edge* Socket::getConnectedEdge() const {
    return edge;
}

bool Socket::hasEdge() const
{
    return edge != nullptr;
}

Edge* Socket::getEdge() const
{
    return edge;
}
