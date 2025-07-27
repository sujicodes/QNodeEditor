#ifndef NODE_H
#define NODE_H

#include <QString>
#include <QVector>

class Scene;
class NodeGraphicsItem;

class Node {
public:
    Node(Scene *scene, const QString &title = "Undefined Node");

    void addInput(Node *input);    // optional
    void addOutput(Node *output);  // optional

private:
    Scene *scene;
    QString title;

    NodeGraphicsItem *grNode;

    QVector<Node*> inputs;
    QVector<Node*> outputs;
};

#endif // NODE_H
