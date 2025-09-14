#ifndef EDGE_H
#define EDGE_H

#include "Serializable.h"


class Scene;
class Socket;
class EdgeGraphicsPathItem;
class QJsonObject;

class Edge : public Serializable {
public:
    inline static const int EDGE_TYPE_DIRECT = 1;
    inline static const int EDGE_TYPE_BEZIER = 2;

    Edge(Scene* scene, Socket* startSocket = nullptr, Socket* endSocket = nullptr, int type = EDGE_TYPE_DIRECT);
    ~Edge();

    Scene* getScene() const { return scene; }
    EdgeGraphicsPathItem* getEdgeGraphicsItem() const { return grEdge; }
    void setEndSocket(Socket* socket);
    Socket* getEndSocket() { return m_endSocket; }
    void setStartSocket(Socket* socket);
    Socket* getStartSocket() { return m_startSocket; }

    void updatePositions();
    void removeFromSockets();
    void remove();

    QJsonObject serialize() const override;
    void deserialize(
        const QJsonObject& data,
        std::unordered_map<qint64, Serializable*>& hashmap,
        bool restoreId = true

        ) override;

private:
    Scene* scene;
    Socket* m_startSocket;
    Socket* m_endSocket;

    EdgeGraphicsPathItem* grEdge;
};

#endif // EDGE_H
