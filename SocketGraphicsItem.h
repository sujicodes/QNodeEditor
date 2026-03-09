#ifndef SOCKETGRAPHICSITEM_H
#define SOCKETGRAPHICSITEM_H

#include <QGraphicsItem>
#include <QPen>
#include <QBrush>
#include <QColor>

class Socket;

class SocketGraphicsItem : public QGraphicsItem {
public:
    SocketGraphicsItem(Socket* socket = nullptr);

    QRectF boundingRect() const override;
    void paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget = nullptr) override;
    Socket* getSocket() const { return socket; }
private:
    qreal radius;
    qreal outlineWidth;

    QPen pen;
    QBrush brush;

    Socket* socket;
};

#endif
