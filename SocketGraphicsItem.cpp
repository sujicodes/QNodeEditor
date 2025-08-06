#include "SocketGraphicsItem.h"
#include "Node.h"
#include "Socket.h"
#include "NodeGraphicsItem.h"
#include "Theme.h"
#include <QPainter>

SocketGraphicsItem::SocketGraphicsItem(Socket* sock)
    : QGraphicsItem(sock->getNode()->getNodeGraphicsItem()),
    radius(6.0),
    outlineWidth(1.0)
{
    QColor backgroundColor(Theme::instance().socketBackgroundColor);
    QColor outlineColor(Theme::instance().socketOutlineColor);

    pen = QPen(outlineColor);
    pen.setWidthF(outlineWidth);

    brush = QBrush(backgroundColor);
    socket = sock;
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
