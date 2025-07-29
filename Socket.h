#ifndef SOCKET_H
#define SOCKET_H

#include <QObject>
#include <QGraphicsItem>
#include "SocketGraphicsItem.h"



class Node;

class Socket {
public:
    Socket(Node* node, int index = 0, int position = LEFT_TOP);

    SocketGraphicsItem* getGraphicsSocket() const { return grSocket; }
    inline static const int LEFT_TOP = 1;
    inline static const int LEFT_BOTTOM = 2;
    inline static const int RIGHT_TOP = 3;
    inline static const int RIGHT_BOTTOM = 4;

private:
    Node* node;
    int index;
    int position;

    SocketGraphicsItem* grSocket;
};

#endif // SOCKET_H
