#include "DirectEdgeGraphicsPathItem.h"


void DirectEdgeGraphicsPathItem::updatePath() {
    QPainterPath p(posSource);
    p.lineTo(posDestination);
    setPath(p);
}

