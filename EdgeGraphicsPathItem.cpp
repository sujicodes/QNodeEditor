#include "EdgeGraphicsPathItem.h"
#include "Edge.h"
#include <QPainter>

EdgeGraphicsPathItem::EdgeGraphicsPathItem(Edge* edge, QGraphicsItem* parent)
    : QGraphicsPathItem(parent), edge(edge)
{
    pen.setWidthF(2.0);
    penSelected.setWidthF(2.0);
    penDragging.setWidthF(2.0);

    setFlag(QGraphicsItem::ItemIsSelectable);
    setZValue(-1);
}

void EdgeGraphicsPathItem::paint(QPainter* painter, const QStyleOptionGraphicsItem* option, QWidget* widget)
{
    updatePath();
    if (!edge || !edge->getEndSocket()) {
        painter->setPen(penDragging);
    } else {
        painter->setPen(isSelected() ? penSelected : pen);
    }
    painter->setBrush(Qt::NoBrush);
    painter->drawPath(path());
}

QPointF EdgeGraphicsPathItem::getSource() const {
    return posSource;
}

QPointF EdgeGraphicsPathItem::getDestination() const {
    return posDestination;
}

void EdgeGraphicsPathItem::setSource(QPointF pos) {
    posSource = pos;
}

void EdgeGraphicsPathItem::setDestination(QPointF pos) {
    posDestination = pos;
}
