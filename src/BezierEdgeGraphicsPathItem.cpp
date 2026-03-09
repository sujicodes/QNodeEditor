#include "BezierEdgeGraphicsPathItem.h"
#include <QPainterPath>
#include <cmath>
#include "Edge.h"
#include "SocketItem.h" // Needed for enums like RIGHT_TOP, etc.

QPainterPath BezierEdgeGraphicsPathItem::calcPath() const
{
    const QPointF& s = posSource;
    const QPointF& d = posDestination;

    qreal dist = (d.x() - s.x()) * 0.5;

    qreal cpx_s = +dist;
    qreal cpx_d = -dist;
    qreal cpy_s = 0;
    qreal cpy_d = 0;

    if (edge && edge->getStartSocket()) 
    {
        int sspos = edge->getStartSocket()->getPosition();

        if ((s.x() > d.x() && (sspos == SocketItem::RIGHT_TOP || sspos == SocketItem::RIGHT_BOTTOM)) ||
            (s.x() < d.x() && (sspos == SocketItem::LEFT_TOP || sspos == SocketItem::LEFT_BOTTOM)))
        {
            cpx_s *= -1;
            cpx_d *= -1;

            cpy_d = ((s.y() - d.y()) / std::fabs((s.y() - d.y()) == 0 ? 0.00001 : (s.y() - d.y()))) * EDGE_CP_ROUNDNESS;
            cpy_s = ((d.y() - s.y()) / std::fabs((d.y() - s.y()) == 0 ? 0.00001 : (d.y() - s.y()))) * EDGE_CP_ROUNDNESS;
        }
    }

    QPainterPath path(s);
    path.cubicTo(s.x() + cpx_s, s.y() + cpy_s, d.x() + cpx_d, d.y() + cpy_d, d.x(), d.y());
    return path;
}