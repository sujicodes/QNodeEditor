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

void Socket::setConnectedEdge(Edge* edge) {
    this->edge = edge;
}


bool Socket::hasConnectedEdge() const
{
    return edge != nullptr;
}

Edge* Socket::getConnectedEdge() const
{
    return edge;
}

QJsonObject Socket::serialize() const {
    QJsonObject obj;
    obj["id"] = static_cast<qint64>(id);
    obj["index"] = index;
    obj["position"] = position; // can be int, enum, or QString

    return obj;
}

void Socket::deserialize(
    const QJsonObject& data,
    std::unordered_map<qint64, Serializable*>& hashmap,
    bool restoreId
    ) {
    if(restoreId){
        id =  static_cast<qint64>(data["id"].toDouble());   // or .toVariant().toLongLong()
    }
    hashmap[data["id"].toDouble()] = this;

}
