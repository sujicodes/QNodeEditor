#include "SocketItem.h"
#include "NodeItem.h"
#include "Edge.h"
#include "Theme.h"
#include <QPainter>
#include <QDebug>

SocketItem::SocketItem(NodeItem* node, int type, int index, int position,
                       bool allowMultiEdges)
    : m_node(node), m_socketType(type), m_index(index), m_position(position), allowedMultiEdges(allowMultiEdges)
{
    QColor backgroundColor(Theme::instance().socketBackgroundColor);
    QColor outlineColor(Theme::instance().socketOutlineColor);

    m_pen = QPen(outlineColor);
    m_pen.setWidthF(m_outlineWidth);

    m_brush = QBrush(backgroundColor);

    setParentItem(m_node);
    updateSocketPosition();
}

QPointF SocketItem::getSocketPosition() const
{
    auto posPair = m_node->getSocketPosition(m_index, m_position, m_socketType);
    return QPointF(posPair.first, posPair.second);
}

void SocketItem::updateSocketPosition()
{
    QPointF pos = getSocketPosition();
    setPos(pos);
}

void SocketItem::addEdge(Edge* edge)
{
    m_edges.append(edge);
}

bool SocketItem::hasConnectedEdge() const
{
    return !m_edges.isEmpty();
}

QList<Edge*> SocketItem::getConnectedEdges() const
{
    return m_edges;
}

void SocketItem::removeEdge(Edge* edge)
{
    m_edges.removeOne(edge);
}

void SocketItem::removeAllEdges()
{
    while (!m_edges.isEmpty()) {
        Edge* edge = m_edges.takeFirst();
        if (edge) edge->remove();
    }
}

QJsonObject SocketItem::serialize() const
{
    QJsonObject obj;
    obj["id"] = static_cast<qint64>(m_id);
    obj["index"] = m_index;
    obj["allowed_multi_edges"] = allowedMultiEdges;
    obj["position"] = m_position;
    obj["socket_type"] = m_socketType;
    return obj;
}

void SocketItem::deserialize(const QJsonObject& data,
                             std::unordered_map<qint64, Serializable*>& hashmap,
                             bool restoreId)
{
    if (restoreId)
        m_id = static_cast<qint64>(data["id"].toDouble());

    allowedMultiEdges = data["allowed_multi_edges"].toBool();
    m_socketType = data["socket_type"].toInt();
    hashmap[data["id"].toDouble()] = this;
}

QRectF SocketItem::boundingRect() const
{
    return QRectF(-m_radius - m_outlineWidth, -m_radius - m_outlineWidth,
                  2 * (m_radius + m_outlineWidth), 2 * (m_radius + m_outlineWidth));
}

void SocketItem::paint(QPainter* painter, const QStyleOptionGraphicsItem*, QWidget*)
{
    painter->setBrush(m_brush);
    painter->setPen(m_pen);
    painter->drawEllipse(-m_radius, -m_radius, 2 * m_radius, 2 * m_radius);
}