#ifndef EDGEGRAPHICSPATHITEM_H
#define EDGEGRAPHICSPATHITEM_H

#include <QGraphicsPathItem>
#include <QPen>
#include <QBrush>
#include <QColor>
#include <QPainterPath>

class Edge;  // Forward declaration

class EdgeGraphicsPathItem : public QGraphicsPathItem {
public:
    explicit EdgeGraphicsPathItem(Edge* edge, QGraphicsItem* parent = nullptr);

    virtual void updatePath() = 0;

    void paint(QPainter* painter, const QStyleOptionGraphicsItem* option, QWidget* widget = nullptr) override;

protected:
    Edge* edge;

    QColor color = QColor("#001000");
    QColor colorSelected = QColor("#00ff00");

    QPen pen = QPen(color);
    QPen penSelected = QPen(colorSelected);

    QPointF posSource = QPointF(0, 0);
    QPointF posDestination = QPointF(200, 100);
};

#endif // EDGEGRAPHICSPATHITEM_H
