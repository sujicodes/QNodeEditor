#ifndef SOCKETGRAPHICSITEM_H
#define SOCKETGRAPHICSITEM_H

#include <QGraphicsItem>
#include <QPen>
#include <QBrush>
#include <QColor>

class SocketGraphicsItem : public QGraphicsItem {
public:
    SocketGraphicsItem(QGraphicsItem* parent = nullptr);

    QRectF boundingRect() const override;
    void paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget = nullptr) override;

private:
    qreal radius;
    qreal outlineWidth;

    QPen pen;
    QBrush brush;
};

#endif
