#ifndef EDGE_H
#define EDGE_H

#include "Serializable.h"

class Scene;
class SocketItem;
class EdgeGraphicsPathItem;
class QJsonObject;

class Edge : public Serializable
{
    public:

        inline static const int EDGE_TYPE_DIRECT = 1;
        inline static const int EDGE_TYPE_BEZIER = 2;

        Edge(Scene* scene, SocketItem* startSocket = nullptr, SocketItem* endSocket = nullptr, int type = EDGE_TYPE_DIRECT);
        ~Edge();

        Scene* getScene() const { return scene; }
        EdgeGraphicsPathItem* getEdgeGraphicsItem() const { return grEdge; }
        void setEndSocket(SocketItem* socket);
        SocketItem* getEndSocket() { return m_endSocket; }
        void setStartSocket(SocketItem* socket);
        SocketItem* getStartSocket() { return m_startSocket; }

        SocketItem* getOtherSocket(SocketItem* known_socket) const
        {
            return (known_socket == m_endSocket) ? m_startSocket : m_endSocket;
        }

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

        Scene* scene = nullptr;
        SocketItem* m_startSocket = nullptr;
        SocketItem* m_endSocket = nullptr;

        EdgeGraphicsPathItem* grEdge;
};

#endif // EDGE_H