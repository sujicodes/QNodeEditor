#include "EdgeGraphicsPathItem.h"
#include "Edge.h"
#include <QPainter>
#include "Scene.h"
EdgeGraphicsPathItem::EdgeGraphicsPathItem(Edge* edge, QGraphicsItem* parent)
    : QGraphicsPathItem(parent), edge(edge)
{
    initAssets();
    initUI();
}

void EdgeGraphicsPathItem::initUI()
{
    setFlag(QGraphicsItem::ItemIsSelectable);
    setZValue(-1);
}

void EdgeGraphicsPathItem::initAssets()
{
    pen = QPen(QColor("#001000"));
    penSelected = QPen(QColor("#00ff00"));
    penDragging = QPen(QColor("#001000"));
    penDragging.setStyle(Qt::DashLine);

    pen.setWidthF(2.0);
    penSelected.setWidthF(2.0);
    penDragging.setWidthF(2.0);
}

void EdgeGraphicsPathItem::onSelected()
{
    // Python equivalent:
    //    self.edge.scene.grScene.itemSelected.emit()
    if (edge && edge->getScene() && edge->getScene()->graphicsScene())
        emit edge->getScene()->graphicsScene()->itemSelected();  // closest Qt equivalent
}

void EdgeGraphicsPathItem::mouseReleaseEvent(QGraphicsSceneMouseEvent* event)
{
    QGraphicsPathItem::mouseReleaseEvent(event);

    if (lastSelectedState != isSelected()) {
        if (edge && edge->getScene())
            edge->getScene()->resetLastSelectedStates();
        lastSelectedState = isSelected();
        onSelected();
    }
}

void EdgeGraphicsPathItem::setSource(const QPointF& pos)
{
    posSource = pos;
}

void EdgeGraphicsPathItem::setDestination(const QPointF& pos)
{
    posDestination = pos;
}

QPointF EdgeGraphicsPathItem::getSource() const
{
    return posSource;
}

QPointF EdgeGraphicsPathItem::getDestination() const
{
    return posDestination;
}

QRectF EdgeGraphicsPathItem::boundingRect() const
{
    return shape().boundingRect();
}

QPainterPath EdgeGraphicsPathItem::shape() const
{
    return calcPath();
}

void EdgeGraphicsPathItem::paint(QPainter* painter, const QStyleOptionGraphicsItem* option, QWidget* widget)
{
    Q_UNUSED(option);
    Q_UNUSED(widget);

    // Python: self.setPath(self.calcPath())
    setPath(calcPath());

    if (!edge || !edge->getEndSocket()) {
        painter->setPen(penDragging);
    } else {
        painter->setPen(isSelected() ? penSelected : pen);
    }

    painter->setBrush(Qt::NoBrush);
    painter->drawPath(path());
}

bool EdgeGraphicsPathItem::intersectsWith(const QPointF& p1, const QPointF& p2)
{
    QPainterPath cutPath(p1);
    cutPath.lineTo(p2);

    return cutPath.intersects(calcPath());
}
