#ifndef NODE_H
#define NODE_H

#include <QString>
#include <QVector>

class Scene;
class NodeGraphicsItem;
class Socket;

class Node {
public:
    Node(Scene* scene, const QString& title = "Undefined Node",
         const std::vector<int>& inputs = {}, const std::vector<int>& outputs = {});

    std::pair<float, float> getSocketPosition(int index, int position);

    void addInput(Socket *input);
    void addOutput(Socket *output);
    NodeGraphicsItem* getNodeGraphicsItem(){return grNode;};

private:
    Scene *scene;
    QString title;
    float socketSpacing = 22.0f;

    NodeGraphicsItem *grNode;

    QVector<Socket*> inputs;
    QVector<Socket*> outputs;
};

#endif // NODE_H
