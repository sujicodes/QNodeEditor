#ifndef NODE_H
#define NODE_H

#include "Serializable.h"
#include <string>
#include <vector>
#include <utility>
#include <sstream>
#include "NodeRegistry.h"

class Scene;
class NodeGraphicsItem;
class Socket;
class Edge;
class Serializable;

class Node : public Serializable {
public:
    Node(Scene* scene,
         const QString& title = "Undefined Node",
         const QList<int>& in = {},
         const QList<int>& outs = {});
    
    ~Node();

    std::pair<float, float> getSocketPosition(int index, int position, int type);

    virtual QString nodeType()const {return "Node";}
    void addInput(Socket* input);
    void addOutput(Socket* output);
    virtual NodeGraphicsItem* getNodeGraphicsItem() { return grNode; }
    void setNodeGraphicsItem(NodeGraphicsItem* nodeGraphicsItem);
    Scene* getScene() { return scene; }
    void updateConnectedEdges();
    void updateSockets();

    QList<Edge*> getConnectedEdges();
    QPointF pos() const;
    void setPos(float x, float y);
    void remove();


    // Serializable implementation
    QJsonObject serialize() const override;
    void deserialize(
        const QJsonObject& data,
        std::unordered_map<qint64, Serializable*>& hashmap,
        bool restoreId = true

    ) override;

    QList<Socket*> inputs;
    QList<Socket*> outputs;
    Serializable* content = nullptr;

    QString getTitle() { return m_title; }
    void setTitle(const QString &value);

    int getInputSocketPosition() const;
    int getOutputSocketPosition() const;

    void setInputSocketPosition(int value);
    void setOutputSocketPosition(int value);

    bool isDirty() const;
    void markDirty(bool newValue = true);
    void onMarkedDirty() {return;};

    void markChildrenDirty(bool newValue = true);
    void markDescendantsDirty(bool newValue = true);

    bool isInvalid() const;
    void markInvalid(bool newValue = true);
    void onMarkedInvalid(){return;}

    void markChildrenInvalid(bool newValue = true);
    void markDescendantsInvalid(bool newValue = true);

    virtual void onEdgeConnectionChanged(Edge* edge);
    virtual void onInputChanged(Edge* edge);

    virtual QVariant eval();
    void evalChildren();

    std::vector<Node*> getChildrenNodes() const;

    Node* getInput(int index = 0);
    QList<Node*> getInputs(int index = 0);
    QList<Node*> getOutputs(int index = 0);


private:
    Scene* scene;
    QString m_title;
    float socketSpacing = 22.0f;
    NodeGraphicsItem* grNode = nullptr;
    int inputSocketPosition;
    int outputSocketPosition;
    bool m_isDirty = false;
    bool m_isInvalid = false;

};

REGISTER_NODE(Node, "Node");
#endif // NODE_H
