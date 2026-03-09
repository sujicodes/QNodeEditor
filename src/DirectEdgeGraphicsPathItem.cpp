#include "DirectEdgeGraphicsPathItem.h"

QPainterPath DirectEdgeGraphicsPathItem::calcPath() const
{
    QPainterPath p(posSource);
    p.lineTo(posDestination);
    return p;
}