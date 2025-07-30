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
    inline static const int LEFT_TOP = 1;
    inline static const int LEFT_BOTTOM = 2;
    inline static const int RIGHT_TOP = 3;
    inline static const int RIGHT_BOTTOM = 4;

    void setConnectedEdge(Edge* edge = nullptr);
    Edge* getConnectedEdge() const;

    inline int getIndex() const { return index; }
    inline int getPosition() const { return position; }
    inline Node* getNode() const { return node; }
    QPointF getSocketPosition() const;

    bool hasEdge() const;
    Edge* getEdge() const;

private:
    Node* node;
    int index;
    int position;
    Edge* edge = nullptr;

    SocketGraphicsItem* grSocket;
    static constexpr bool DEBUG = false;
};

#endif // SOCKET_H
