#include "socket.h"
#include "node.h"
#include "SocketGraphicsItem.h"
#include "NodeGraphicsItem.h"


Socket::Socket(Node* node, int index, int position)
    : node(node), index(index), position(position)
{
    grSocket = new SocketGraphicsItem(node->getNodeGraphicsItem());

    // Position the socket based on the node's layout logic
    std::pair<float, float> pos = node->getSocketPosition(index, position);
    grSocket->setPos(QPointF(pos.first, pos.second));
}
