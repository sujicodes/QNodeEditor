#ifndef NODE_H
#define NODE_H

#include <QString>
#include <QVector>
#include <QPointF>

class Scene;
class NodeGraphicsItem;
class Socket;

class Node {
public:
    Node(Scene* scene, const QString& title = "Undefined Node",
         const std::vector<int>& in = {}, const std::vector<int>& outs = {});

    std::pair<float, float> getSocketPosition(int index, int position);

    void addInput(Socket *input);
    void addOutput(Socket *output);
    NodeGraphicsItem* getNodeGraphicsItem(){return grNode;};
    void updateConnectedEdges();

    QPointF pos() const;
    void setPos(float x, float y);
    void setPos(const QPointF& point);
    std::vector<Socket*> inputs;
    std::vector<Socket*> outputs;

private:
    Scene *scene;
    QString title;
    float socketSpacing = 22.0f;

    NodeGraphicsItem *grNode;
};

#endif // NODE_H
