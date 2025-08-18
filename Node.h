#ifndef NODE_H
#define NODE_H

#include "Serializable.h"
#include <string>
#include <vector>
#include <utility>
#include <sstream>

class Scene;
class NodeGraphicsItem;
class Socket;
class Serializable;

class Node : public Serializable {
public:
    Node(Scene* scene,
         const QString& title = "Undefined Node",
         const std::vector<int>& in = {},
         const std::vector<int>& outs = {});

    std::pair<float, float> getSocketPosition(int index, int position);

    void addInput(Socket* input);
    void addOutput(Socket* output);
    NodeGraphicsItem* getNodeGraphicsItem() { return grNode; }
    void updateConnectedEdges();
    QPointF pos() const;
    void setPos(float x, float y);
    void remove();

    // Serializable implementation
    QJsonObject serialize() const override;
    void deserialize(
        const QJsonObject& data,
        std::unordered_map<qint64, Serializable*>& hashmap


    ) override;

    std::vector<Socket*> inputs;
    std::vector<Socket*> outputs;
    Serializable* content = nullptr;

    QString getTitle() { return m_title; }
    void setTitle(const QString &value);

private:
    Scene* scene;
    QString m_title;
    float socketSpacing = 22.0f;
    NodeGraphicsItem* grNode;
};

#endif // NODE_H
