 #ifndef SOCKET_H
#define SOCKET_H

#include <QObject>
#include <QGraphicsItem>
#include <QList>
#include "SocketGraphicsItem.h"
#include "Serializable.h"


class Node;
class Edge;

class Socket : public Serializable {
public:
    Socket(Node* node, int type, int index = 0, int position = LEFT_TOP,  bool allowMultiEdges = false, bool isInput = false);

    SocketGraphicsItem* getGraphicsSocket() const { return grSocket; }
    static const int LEFT_TOP = 1;
    static const int LEFT_CENTER  = 2;
    static const int LEFT_BOTTOM  = 3;
    static const int RIGHT_TOP    = 4;
    static const int RIGHT_CENTER = 5;
    static const int RIGHT_BOTTOM = 6;

    static const int INPUT = 6;
    static const int OUTPUT = 8;


    bool allowedMultiEdges = false;
    void setSocketType(int type){socketType = type;};

    bool isInput(){return socketType==INPUT;}

    void addEdge(Edge* edge);

    int getIndex() const { return index; }
    int getPosition() const { return position; }
    void setPosition(int pos) {position = pos;}
    Node* getNode() const { return node; }
    QPointF getSocketPosition() const;

    bool hasConnectedEdge() const;
    QList<Edge*> getConnectedEdges() const;

    void removeEdge(Edge* edge);
    void removeAllEdges();
    void updateSocketPosition();

    QJsonObject serialize() const override;
    void deserialize(
        const QJsonObject& data,
        std::unordered_map<qint64, Serializable*>& hashmap,
        bool restoreId = true        
        ) override;

private:
    Node* node = nullptr;
    int index;
    int position;
    QList<Edge*> edges;
    int socketType;

    SocketGraphicsItem* grSocket = nullptr;
    static constexpr bool DEBUG = false;
};

#endif // SOCKET_H
