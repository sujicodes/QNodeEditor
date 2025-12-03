#include "Socket.h"
#include "Edge.h"
#include "Node.h"
#include "SocketGraphicsItem.h"
#include "NodeGraphicsItem.h"


Socket::Socket(Node* node, int index, int position, bool allowMultiEdges)
    : node(node), index(index), position(position), allowedMultiEdges(allowMultiEdges)
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

void Socket::addEdge(Edge* edge) {
    edges.append(edge);
}

bool Socket::hasConnectedEdge() const
{
    return !edges.isEmpty();
}

QList<Edge*> Socket::getConnectedEdges() const
{
    return edges;
}

QJsonObject Socket::serialize() const {
    QJsonObject obj;
    obj["id"] = static_cast<qint64>(id);
    obj["index"] = index;
    obj["allowed_multi_edges"] = allowedMultiEdges;
    obj["position"] = position;

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
    allowedMultiEdges = data["allowed_multi_edges"].toBool(); 
    hashmap[data["id"].toDouble()] = this;

}

void Socket::removeEdge(Edge* edge)
{
    edges.removeOne(edge);
}

void Socket::removeAllEdges()
{
    while (!edges.isEmpty()) {
        Edge* edge = edges.takeFirst();  // Removes and returns the first element
        if (edge)
            edge->remove();              // Calls your edge cleanup method
    }
}
