#include "EdgeGraphicsPathItem.h"
#include <QPainter>

EdgeGraphicsPathItem::EdgeGraphicsPathItem(Edge* edge, QGraphicsItem* parent)
    : QGraphicsPathItem(parent), edge(edge)
{
    pen.setWidthF(2.0);
    penSelected.setWidthF(2.0);

    setFlag(QGraphicsItem::ItemIsSelectable);
    setZValue(-1);
}

void EdgeGraphicsPathItem::paint(QPainter* painter, const QStyleOptionGraphicsItem* option, QWidget* widget)
{
    updatePath();
    painter->setPen(isSelected() ? penSelected : pen);
    painter->setBrush(Qt::NoBrush);
    painter->drawPath(path());
}
