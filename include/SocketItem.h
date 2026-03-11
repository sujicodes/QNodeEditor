#ifndef SOCKETITEM_H
#define SOCKETITEM_H

#include <QGraphicsItem>
#include <QList>
#include <QPen>
#include <QBrush>
#include "Serializable.h"

class NodeItem;
class Edge;

class SocketItem : public QGraphicsItem, public Serializable
{
public:
    SocketItem(NodeItem* node, int type, int index = 0, int position = LEFT_TOP,
               bool allowMultiEdges = false);

    SocketItem* getGraphicsSocket() const { return const_cast<SocketItem*>(this); }

    static const int LEFT_TOP = 1;
    static const int LEFT_CENTER = 2;
    static const int LEFT_BOTTOM = 3;
    static const int RIGHT_TOP = 4;
    static const int RIGHT_CENTER = 5;
    static const int RIGHT_BOTTOM = 6;

    static const int INPUT = 6;
    static const int OUTPUT = 8;

    bool allowedMultiEdges = false;

    void setSocketType(int type) { socketType = type; }
    bool isInput() const { return socketType == INPUT; }

    void addEdge(Edge* edge);
    void removeEdge(Edge* edge);
    void removeAllEdges();
    bool hasConnectedEdge() const;
    QList<Edge*> getConnectedEdges() const;

    NodeItem* getNode() const { return node; }
    int getIndex() const { return index; }
    int getPosition() const { return position; }
    void setPosition(int pos) { position = pos; }
    QPointF getSocketPosition() const;
    void updateSocketPosition();

    QJsonObject serialize() const override;
    void deserialize(const QJsonObject& data,
                     std::unordered_map<qint64, Serializable*>& hashmap,
                     bool restoreId = true) override;

    QRectF boundingRect() const override;
    void paint(QPainter* painter, const QStyleOptionGraphicsItem* option, QWidget* widget = nullptr) override;

private:
    NodeItem* node = nullptr;
    int index = 0;
    int position = LEFT_TOP;
    int socketType = INPUT;
    QList<Edge*> edges;

    qreal radius = 6.0;
    qreal outlineWidth = 1.0;
    QPen pen;
    QBrush brush;
};

#endif // SOCKETITEM_H