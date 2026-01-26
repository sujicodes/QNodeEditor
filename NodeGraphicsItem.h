#ifndef NODEGRAPHICSITEM_H
#define NODEGRAPHICSITEM_H

#include <QGraphicsItem>
#include <QGraphicsTextItem>
#include <QGraphicsProxyWidget>
#include <QPen>
#include <QBrush>
#include <QFont>
#include <QJsonObject>
#include <QJsonValue>


class Node;

class NodeGraphicsItem : public QGraphicsItem {
public:
    NodeGraphicsItem(Node *node, QGraphicsItem *parent = nullptr);
    virtual ~NodeGraphicsItem() = default;

    QRectF boundingRect() const override;
    void paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget = nullptr) override;

    void setTitle(const QString &title);
    QString title() const;
    QWidget* itemWidget;

    Node* getNode() const { return node; }
    float getWidth() const { return width; }
    float getHeight() const { return height; }
    float getTitleHeight() const { return titleHeight; }
    int getEdgeRoundness() const;
    int getEdgePadding() const;
    int getTitleHorizontalPadding() const;
    int getTitleVerticalPadding() const;

    void setNode(Node* n) { node = n;}
    void setWidth(float w) { width = w; }
    void setHeight(float h) { height = h; }
    void setTitleHeight(float t) { titleHeight = t; }
    void setLastSelectedState(bool s) { lastSelectedState = s; }
    void setEdgeRoundness(int value);
    void setEdgePadding(int value);
    void setTitleHorizontalPadding(int value);
    void setTitleVerticalPadding(int value);


    void onNodeMoved();
    virtual void setNodeContent(const QJsonValue &data) {return;}
    virtual QJsonObject getNodeContent() const {return QJsonObject();};
        void initUI();

private slots:

protected:
    void mouseReleaseEvent(QGraphicsSceneMouseEvent* event) override;
    virtual QWidget* setItemWidget() const;


private:
    void initTitle();
    void initItemWidget();

    QGraphicsTextItem* titleItem;

    QString _title;
    QColor _titleColor = Qt::white;
    QFont _titleFont = QFont("Ubuntu", 10);

    float width = 180;
    float height = 240;
    float titleHeight = 24.0;

    int edgeRoundness = 6;
    int edgePadding = 0;
    int titleHorizontalPadding = 8;
    int titleVerticalPadding = 10;

    bool lastSelectedState = false;

    QPointF startPos;

    Node* node;

    QPen penDefault = QPen(QColor("#7F000000"));
    QPen penSelected = QPen(QColor("#FFFFA637"));

    QBrush brushTitle = QBrush(QColor("#FF313131"));
    QBrush brushBackground = QBrush(QColor("#E3212121"));

    QGraphicsProxyWidget* graphicsProxyWidget{nullptr};
};

#endif // NODEGRAPHICSITEM_H
