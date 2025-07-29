#include "Edge.h"

#include "Edge.h"
#include "DirectEdgeGraphicsPathItem.h"
#include "BezierEdgeGraphicsPathItem.h"
#include "Scene.h"

Edge::Edge(Scene* scene, Socket* startSocket, Socket* endSocket, int type)
    : scene(scene), startSocket(startSocket), endSocket(endSocket)
{
    if (type == EDGE_TYPE_DIRECT)
        grEdge = new DirectEdgeGraphicsPathItem(this);
    else
        grEdge = new BezierEdgeGraphicsPathItem(this);

    scene->graphicsScene()->addItem(grEdge);
}
