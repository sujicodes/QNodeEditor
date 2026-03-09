#ifndef BEZIEREDGEGRAPHICSPATHITEM_H
#define BEZIEREDGEGRAPHICSPATHITEM_H

#include "EdgeGraphicsPathItem.h"

class BezierEdgeGraphicsPathItem : public EdgeGraphicsPathItem {
public:
    BezierEdgeGraphicsPathItem(Edge* edge, QGraphicsItem* parent = nullptr)
        : EdgeGraphicsPathItem(edge, parent) {}
protected:
   QPainterPath calcPath() const override;
};

#endif // BEZIEREDGEGRAPHICSPATHITEM_H
