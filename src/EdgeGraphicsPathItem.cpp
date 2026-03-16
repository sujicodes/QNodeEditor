#include "EdgeGraphicsPathItem.h"
#include "Edge.h"
#include <QPainter>
#include "NodeEditorGraphicsScene.h"

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
    m_pen = QPen(QColor("#001000"));
    m_penSelected = QPen(QColor("#00ff00"));
    penDragging = QPen(QColor("#001000"));
    penDragging.setStyle(Qt::DashLine);

    m_pen.setWidthF(2.0);
    m_penSelected.setWidthF(2.0);
    penDragging.setWidthF(2.0);
}

void EdgeGraphicsPathItem::onSelected()
{
    // Python equivalent:
    //    self.edge.scene.grScene.itemSelected.emit()
    if (edge && edge->getScene())
        emit edge->getScene()->itemSelected();  // closest Qt equivalent
}

void EdgeGraphicsPathItem::mouseReleaseEvent(QGraphicsSceneMouseEvent* event)
{
    QGraphicsPathItem::mouseReleaseEvent(event);

    if(m_lastSelectedState != isSelected())
    {
        if(edge && edge->getScene())
            edge->getScene()->resetLastSelectedStates();
        m_lastSelectedState = isSelected();
        onSelected();
    }
}

void EdgeGraphicsPathItem::setSource(const QPointF& pos)
{
    posSource = pos;
    updatePath();
}

void EdgeGraphicsPathItem::setDestination(const QPointF& pos)
{
    posDestination = pos;
    updatePath();
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

void EdgeGraphicsPathItem::updatePath()
{
    prepareGeometryChange();
    setPath(calcPath());
    update();
}

void EdgeGraphicsPathItem::paint(QPainter* painter, const QStyleOptionGraphicsItem* option, QWidget* widget)
{
    Q_UNUSED(option);
    Q_UNUSED(widget);

    if(!edge || !edge->getEndSocket())
    {
        painter->setPen(penDragging);
    }
    else
    {
        painter->setPen(isSelected() ? m_penSelected : m_pen);
    }

    painter->setBrush(Qt::NoBrush);
    painter->drawPath(path());
}
