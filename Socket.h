#ifndef SOCKET_H
#define SOCKET_H

#include <QObject>
#include <QGraphicsItem>
#include "SocketGraphicsItem.h"



class Node;
class Edge;

class Socket {
public:
    Socket(Node* node, int index = 0, int position = LEFT_TOP);

    SocketGraphicsItem* getGraphicsSocket() const { return grSocket; }
    static const int LEFT_TOP = 1;
    static const int LEFT_BOTTOM = 2;
    static const int RIGHT_TOP = 3;
    static const int RIGHT_BOTTOM = 4;

    void setConnectedEdge(Edge* edge = nullptr);

    int getIndex() const { return index; }
    int getPosition() const { return position; }
    Node* getNode() const { return node; }
    QPointF getSocketPosition() const;

    bool hasConnectedEdge() const;
    Edge* getConnectedEdge() const;

private:
    Node* node;
    int index;
    int position;
    Edge* edge = nullptr;

    SocketGraphicsItem* grSocket;
    static constexpr bool DEBUG = false;
};

#endif // SOCKET_H
