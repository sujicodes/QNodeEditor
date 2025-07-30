#ifndef EDGE_H
#define EDGE_H

class Scene;
class Socket;
class EdgeGraphicsPathItem;

class Edge {
public:
    inline static const int EDGE_TYPE_DIRECT = 1;
    inline static const int EDGE_TYPE_BEZIER = 2;

    Edge(Scene* scene, Socket* startSocket, Socket* endSocket, int type = EDGE_TYPE_DIRECT);
    ~Edge();

    Scene* getScene() const { return scene; }
    EdgeGraphicsPathItem* getGraphicsEdge() const { return grEdge; }

    void updatePositions();
    void removeFromSockets();
    void remove();

private:
    Scene* scene;
    Socket* startSocket;
    Socket* endSocket;

    EdgeGraphicsPathItem* grEdge;
};

#endif // EDGE_H
