#ifndef EDGEGRAPHICSPATHITEM_H
#define EDGEGRAPHICSPATHITEM_H

#include <QGraphicsPathItem>
#include <QPen>
#include <QBrush>
#include <QColor>
#include <QPainterPath>

class Edge;  // Forward declaration

class EdgeGraphicsPathItem : public QGraphicsPathItem
{
public:
    explicit EdgeGraphicsPathItem(Edge* edge, QGraphicsItem* parent = nullptr);

    // Required API
    void setSource(const QPointF& pos);
    void setDestination(const QPointF& pos);

    QPointF getSource() const;
    QPointF getDestination() const;

    // Overrides
    //virtual void updatePath() = 0;
    QRectF boundingRect() const override;
    QPainterPath shape() const override;
    void paint(QPainter* painter, const QStyleOptionGraphicsItem* option, QWidget* widget) override;
    void mouseReleaseEvent(QGraphicsSceneMouseEvent* event) override;

    // Collision helper
    bool intersectsWith(const QPointF& p1, const QPointF& p2);

    inline static const int EDGE_CP_ROUNDNESS = 100;

    Edge* getEdge() const {return edge;}
    void updatePath();
    void setLastSelectedState(bool s) { lastSelectedState = s; }

protected:
    // Must be overridden by subclasses (just like Python)
    virtual QPainterPath calcPath() const = 0;
    QPointF posSource {0, 0};
    QPointF posDestination { 200, 100};

    bool lastSelectedState = false;

    // Pens
    QPen pen;
    QPen penSelected;
    QPen penDragging;
    Edge* edge;

private:
    void initAssets();
    void initUI();
    void onSelected();
};

#endif // EDGEGRAPHICSPATHITEM_H
