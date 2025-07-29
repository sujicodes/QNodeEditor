#include "Node.h"
#include "Scene.h"
#include "Socket.h"
#include "NodeGraphicsItem.h"
#include "NodeGraphicsScene.h"

Node::Node(Scene* scene, const QString& title, const std::vector<int>& inputs, const std::vector<int>& outputs)
    : scene(scene), title(title)
{
    grNode = new NodeGraphicsItem();
    grNode->setTitle(title);

    scene->addNode(this);
    scene->graphicsScene()->addItem(grNode);
    int counter = 0;
    for (int i : inputs) {
        Socket* socket = new Socket(this, counter++, Socket::LEFT_BOTTOM);
        addInput(socket);
    }

    counter = 0;
    for (int i : outputs) {
        Socket* socket = new Socket(this, counter++, Socket::RIGHT_TOP);
        this->outputs.push_back(socket);
    }
}

void Node::addInput(Socket *input) {
    inputs.append(input);
}

void Node::addOutput(Socket *output) {
    outputs.append(output);
}

std::pair<float, float> Node::getSocketPosition(int index, int position)
{
    float x = (position == Socket::LEFT_TOP || position == Socket::LEFT_BOTTOM) ? 0.0f : grNode->getWidth();

    float y = 0.0f;
    if (position == Socket::LEFT_BOTTOM || position == Socket::RIGHT_BOTTOM) {
        y = grNode->getHeight() - grNode->getEdgeSize() - grNode->getPadding() - index * socketSpacing;
    } else {
        y = grNode->getTitleHeight() + grNode->getPadding() + grNode->getEdgeSize() + index * socketSpacing;
    }

    return { x, y };
}
