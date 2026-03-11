#ifndef EDGE_H
#define EDGE_H

#include "NodeEditorGraphicsScene.h"
#include "Serializable.h"

class Scene;
class SocketItem;
class EdgeGraphicsPathItem;
class QJsonObject;

class Edge : public Serializable
{
    public:

        Edge(NodeEditorGraphicsScene* scene, SocketItem* startSocket = nullptr, SocketItem* endSocket = nullptr);
        ~Edge();

        NodeEditorGraphicsScene* getScene() const { return scene; }
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

        NodeEditorGraphicsScene* scene = nullptr;
        SocketItem* m_startSocket = nullptr;
        SocketItem* m_endSocket = nullptr;
        EdgeGraphicsPathItem* grEdge;
};

#endif // EDGE_H
