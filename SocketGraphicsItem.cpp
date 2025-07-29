#include "SocketGraphicsItem.h"
#include "Theme.h"
#include <QPainter>

SocketGraphicsItem::SocketGraphicsItem(QGraphicsItem* parent)
    : QGraphicsItem(parent),
    radius(6.0),
    outlineWidth(1.0)
{
    QColor backgroundColor(Theme::instance().socketBackgroundColor);
    QColor outlineColor(Theme::instance().socketOutlineColor);

    pen = QPen(outlineColor);
    pen.setWidthF(outlineWidth);

    brush = QBrush(backgroundColor);
}

QRectF SocketGraphicsItem::boundingRect() const {
    return QRectF(
        -radius - outlineWidth,
        -radius - outlineWidth,
        2 * (radius + outlineWidth),
        2 * (radius + outlineWidth)
        );
}

void SocketGraphicsItem::paint(QPainter *painter, const QStyleOptionGraphicsItem *, QWidget *) {
    painter->setBrush(brush);
    painter->setPen(pen);
    painter->drawEllipse(-radius, -radius, 2 * radius, 2 * radius);
}
