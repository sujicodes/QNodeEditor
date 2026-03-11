#include "SocketItem.h"
#include "NodeItem.h"
#include "Edge.h"
#include "Theme.h"
#include <QPainter>
#include <QDebug>

SocketItem::SocketItem(NodeItem* node, int type, int index, int position,
                       bool allowMultiEdges)
    : node(node), socketType(type), index(index), position(position), allowedMultiEdges(allowMultiEdges)
{
    QColor backgroundColor(Theme::instance().socketBackgroundColor);
    QColor outlineColor(Theme::instance().socketOutlineColor);

    pen = QPen(outlineColor);
    pen.setWidthF(outlineWidth);

    brush = QBrush(backgroundColor);

    setParentItem(node);
    updateSocketPosition();
}

QPointF SocketItem::getSocketPosition() const
{
    auto posPair = node->getSocketPosition(index, position, socketType);
    return QPointF(posPair.first, posPair.second);
}

void SocketItem::updateSocketPosition()
{
    QPointF pos = getSocketPosition();
    setPos(pos);
}

void SocketItem::addEdge(Edge* edge)
{
    edges.append(edge);
}

bool SocketItem::hasConnectedEdge() const
{
    return !edges.isEmpty();
}

QList<Edge*> SocketItem::getConnectedEdges() const
{
    return edges;
}

void SocketItem::removeEdge(Edge* edge)
{
    edges.removeOne(edge);
}

void SocketItem::removeAllEdges()
{
    while (!edges.isEmpty()) {
        Edge* edge = edges.takeFirst();
        if (edge) edge->remove();
    }
}

QJsonObject SocketItem::serialize() const
{
    QJsonObject obj;
    obj["id"] = static_cast<qint64>(id);
    obj["index"] = index;
    obj["allowed_multi_edges"] = allowedMultiEdges;
    obj["position"] = position;
    obj["socket_type"] = socketType;
    return obj;
}

void SocketItem::deserialize(const QJsonObject& data,
                             std::unordered_map<qint64, Serializable*>& hashmap,
                             bool restoreId)
{
    if (restoreId)
        id = static_cast<qint64>(data["id"].toDouble());

    allowedMultiEdges = data["allowed_multi_edges"].toBool();
    socketType = data["socket_type"].toInt();
    hashmap[data["id"].toDouble()] = this;
}

QRectF SocketItem::boundingRect() const
{
    return QRectF(-radius - outlineWidth, -radius - outlineWidth,
                  2 * (radius + outlineWidth), 2 * (radius + outlineWidth));
}

void SocketItem::paint(QPainter* painter, const QStyleOptionGraphicsItem*, QWidget*)
{
    painter->setBrush(brush);
    painter->setPen(pen);
    painter->drawEllipse(-radius, -radius, 2 * radius, 2 * radius);
}