#include "Node.h"
#include "Edge.h"
#include "Scene.h"
#include "Socket.h"
#include "NodeGraphicsItem.h"
#include "NodeGraphicsScene.h"

Node::Node(Scene* scene, const QString& title, const std::vector<int>& in, const std::vector<int>& outs)
    : scene(scene), title(title){

    grNode = new NodeGraphicsItem(this);
    grNode->setTitle(title);

    scene->addNode(this);
    scene->graphicsScene()->addItem(grNode);
    int counter = 0;
    for (int i : in) {
        Socket* socket = new Socket(this, counter++, Socket::LEFT_TOP);
        addInput(socket);
    }

    counter = 0;
    for (int i : outs) {
        Socket* socket = new Socket(this, counter++, Socket::RIGHT_TOP);
        addOutput(socket);
    }
}

void Node::addInput(Socket *input) {
    inputs.push_back(input);
}

void Node::addOutput(Socket *output) {
    outputs.push_back(output);
}

std::pair<float, float> Node::getSocketPosition(int index, int position){

    float x = (position == Socket::LEFT_TOP || position == Socket::LEFT_BOTTOM) ? 0.0f : grNode->getWidth();

    float y = 0.0f;
    if (position == Socket::LEFT_BOTTOM || position == Socket::RIGHT_BOTTOM) {
        y = grNode->getHeight() - grNode->getEdgeSize() - grNode->getPadding() - index * socketSpacing;
    } else {
        y = grNode->getTitleHeight() + grNode->getPadding() + grNode->getEdgeSize() + index * socketSpacing;
    }

    return { x, y };
}


QPointF Node::pos() const {
    return grNode->pos();
}

void Node::setPos(float x, float y) {
    grNode->setPos(x, y);
}

void Node::setPos(const QPointF& point) {
    grNode->setPos(point);
}

void Node::updateConnectedEdges()
{   
    for (size_t i = 0; i < inputs.size(); ++i) {
        Socket* socket = inputs.at(i);
        if (socket->hasEdge()) {
            socket->getEdge()->updatePositions();
        }
    }

    for (size_t i = 0; i < outputs.size(); ++i) {
        Socket* socket = outputs.at(i);
        if (!socket) {
            continue;
        }
        if (socket->hasEdge()) {
            socket->getEdge()->updatePositions();
        }
    }
}
