#ifndef DIRECTEDGEGRAPHICSPATHITEM_H
#define DIRECTEDGEGRAPHICSPATHITEM_H

#include "EdgeGraphicsPathItem.h"

class DirectEdgeGraphicsPathItem: public EdgeGraphicsPathItem {
public:
    DirectEdgeGraphicsPathItem(Edge* edge, QGraphicsItem* parent = nullptr)
        : EdgeGraphicsPathItem(edge, parent) {}

    QPainterPath calcPath() const override;
};
#endif // DIRECTEDGEGRAPHICSPATHITEM_H
