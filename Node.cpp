#include "Node.h"
#include "Scene.h"
#include "NodeGraphicsItem.h"
#include "NodeGraphicsScene.h"

Node::Node(Scene *scene, const QString &title)
    : scene(scene), title(title)
{
    grNode = new NodeGraphicsItem();
    grNode->setTitle(title);

    scene->addNode(this);                        // Register logic node
    scene->graphicsScene()->addItem(grNode);     // Add graphics item to scene
}

void Node::addInput(Node *input) {
    inputs.append(input);
}

void Node::addOutput(Node *output) {
    outputs.append(output);
}
