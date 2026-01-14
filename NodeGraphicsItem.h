#ifndef NODEGRAPHICSITEM_H
#define NODEGRAPHICSITEM_H

#include <QGraphicsItem>
#include <QGraphicsTextItem>
#include <QGraphicsProxyWidget>
#include <QPen>
#include <QBrush>
#include <QFont>

class Node;

class NodeGraphicsItem : public QGraphicsItem {
public:
    NodeGraphicsItem(Node *node, QGraphicsItem *parent = nullptr);

    QRectF boundingRect() const override;
    void paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget = nullptr) override;

    void setTitle(const QString &title);
    QString title() const;
    QWidget* itemWidget;
    virtual QWidget* setItemWidget() const;

    Node* getNode() const { return node; }
    float getWidth() const { return width; }
    float getHeight() const { return height; }
    float getEdgeSize() const { return edgeSize; }
    float getPadding() const { return padding; }
    float getTitleHeight() const { return titleHeight; }

    void setNode(Node* n) { node = n;}
    void setWidth(float w) { width = w; }
    void setHeight(float h) { height = h; }
    void setEdgeSize(float e) { edgeSize = e; }
    void setPadding(float p) { padding = p; }
    void setTitleHeight(float t) { titleHeight = t; }
    void setLastSelectedState(bool s) { lastSelectedState = s; }
    void onNodeMoved();

private slots:

protected:
    void mouseReleaseEvent(QGraphicsSceneMouseEvent* event) override;


private:
    void initUI();
    void initTitle();
    void initItemWidget();

    QGraphicsTextItem* titleItem;

    QString _title;
    QColor _titleColor = Qt::white;
    QFont _titleFont = QFont("Ubuntu", 10);

    float width = 180;
    float height = 240;
    float edgeSize = 10.0;
    float titleHeight = 24.0;
    float padding = 4.0;

    bool lastSelectedState = false;

    QPointF startPos;

    Node *node;

    QPen penDefault = QPen(QColor("#7F000000"));
    QPen penSelected = QPen(QColor("#FFFFA637"));

    QBrush brushTitle = QBrush(QColor("#FF313131"));
    QBrush brushBackground = QBrush(QColor("#E3212121"));

    QGraphicsProxyWidget* graphicsProxyWidget{nullptr};
};

#endif // NODEGRAPHICSITEM_H
