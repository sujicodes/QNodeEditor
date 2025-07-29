#include "BezierEdgeGraphicsPathItem.h"

void BezierEdgeGraphicsPathItem::updatePath() {
    QPointF s = posSource;
    QPointF d = posDestination;

    qreal dist = (d.x() - s.x()) * 0.5;
    if (s.x() > d.x()) dist *= -1;

    QPainterPath p(s);
    p.cubicTo(s.x() + dist, s.y(), d.x() - dist, d.y(), d.x(), d.y());
    setPath(p);
}
