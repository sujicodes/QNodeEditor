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
    void mouseMoveEvent(QGraphicsSceneMouseEvent* event) override;


    void setTitle(const QString &title);
    QString title() const;
    QWidget* itemWidget;
    QWidget* setItemWidget() const;

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

private:
    void initUI();
    void initTitle();
    void initItemWidget();

private:
    QGraphicsTextItem *titleItem;

    QString _title;
    QColor _titleColor = Qt::white;
    QFont _titleFont = QFont("Ubuntu", 10);

    float width = 180;
    float height = 240;
    float edgeSize = 10.0;
    float titleHeight = 24.0;
    float padding = 4.0;

    Node *node;

    QPen penDefault = QPen(QColor("#7F000000"));
    QPen penSelected = QPen(QColor("#FFFFA637"));

    QBrush brushTitle = QBrush(QColor("#FF313131"));
    QBrush brushBackground = QBrush(QColor("#E3212121"));

    QGraphicsProxyWidget* graphicsProxyWidget{nullptr};
};

#endif // NODEGRAPHICSITEM_H
