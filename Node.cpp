#include "Node.h"
#include "Edge.h"
#include "Scene.h"
#include "Socket.h"
#include "NodeGraphicsItem.h"
#include "NodeGraphicsScene.h"
#include <QDebug>
#include <QJsonArray>



Node::Node(Scene* scene, const QString& title, const QList<int>& in, const QList<int>& outs)
    : scene(scene){

    setNodeGraphicsItem(new NodeGraphicsItem(this));
    setTitle(title);
    setInputSocketPosition(Socket::LEFT_CENTER);
    setOutputSocketPosition(Socket::RIGHT_CENTER);
    scene->addNode(this);

    int counter = 0;
    for (int i : in) {
        Socket* socket = new Socket(this, Socket::INPUT, counter++, getInputSocketPosition());
        addInput(socket);
    }

    counter = 0;
    for (int i : outs) {
        Socket* socket = new Socket(this, Socket::OUTPUT, counter++, getOutputSocketPosition(), true);
        addOutput(socket);
    }
}

Node::~Node() {
    remove();
}


void Node::setTitle(const QString &value) {
    m_title = value;
    grNode->setTitle(value);
}

void Node::addInput(Socket *input) {
    inputs.push_back(input);
}

void Node::addOutput(Socket *output) {
    outputs.push_back(output);
}

void Node::updateSockets()
{
    for (int i = 0; i < inputs.size(); ++i) {
        inputs[i]->setPosition(inputSocketPosition);
        inputs[i]->updateSocketPosition();
    }

    for (int i = 0; i < outputs.size(); ++i) {
        outputs[i]->setPosition(outputSocketPosition);
        outputs[i]->updateSocketPosition();
    }
}

std::pair<float, float> Node::getSocketPosition(int index, int position, int type)
{
    float x = (position == Socket::LEFT_TOP || position == Socket::LEFT_CENTER || position == Socket::LEFT_BOTTOM)
    ? 0.0f
    : grNode->getWidth();

    float y = 0.0f;

    int numOutOf;
    if(type == Socket::INPUT){
        qDebug() <<"inputs:"<< inputs.length();
        numOutOf = inputs.length();

    } else if (type == Socket::OUTPUT){
        qDebug() <<"outputs:" << outputs.length();
        numOutOf = outputs.length();
    }

    if (position == Socket::LEFT_BOTTOM || position == Socket::RIGHT_BOTTOM) {
        // start from bottom
        y = grNode->getHeight()
            - grNode->getEdgeRoundness()
            - grNode->getTitleVerticalPadding()
            - index * socketSpacing;

    } else if (position == Socket::LEFT_CENTER || position == Socket::RIGHT_CENTER) {
        // center-aligned sockets
        float nodeHeight = grNode->getHeight();
        float topOffset = grNode->getTitleHeight()
                          + 2 * grNode->getTitleVerticalPadding()
                          + grNode->getEdgePadding();
        float availableHeight = nodeHeight - topOffset;

        float totalHeightOfAllSockets = numOutOf * socketSpacing;
        float newTop = availableHeight - totalHeightOfAllSockets;

        y = topOffset + availableHeight / 2.0f + (index - 0.5f) * socketSpacing;

        if (numOutOf > 1) {
            y -= socketSpacing * (numOutOf - 1) / 2.0f;
        }

    } else if (position == Socket::LEFT_TOP || position == Socket::RIGHT_TOP) {
        // start from top
        y = grNode->getTitleHeight()
            + grNode->getTitleVerticalPadding()
            + grNode->getEdgeRoundness()
            + index * socketSpacing;

    } else {
        // fallback, should never happen
        y = 0.0f;
    }

    return { x, y };
}

QPointF Node::pos() const {
    return grNode->pos();
}

void Node::setPos(float x, float y) {
    grNode->setPos(x, y);
}


void Node::updateConnectedEdges()
{
    for (size_t i = 0; i < inputs.size(); ++i) {
        Socket* socket = inputs.at(i);

        for(Edge* edge : socket->getConnectedEdges()){
            edge->updatePositions();
        }
    }

    for (size_t i = 0; i < outputs.size(); ++i) {
        Socket* socket = outputs.at(i);
        if (!socket) {
            continue;
        }
        for(Edge* edge : socket->getConnectedEdges()){
            edge->updatePositions();
        }
    }
}

QList<Edge*> Node::getConnectedEdges(){

    QList<Edge*> edges;

    for (size_t i = 0; i < inputs.size(); ++i) {
        Socket* socket = inputs.at(i);
        edges.append(socket->getConnectedEdges());
    }

    for (size_t i = 0; i < outputs.size(); ++i) {
        Socket* socket = outputs.at(i);
        if (!socket) {
            continue;
        }
        if (socket->hasConnectedEdge()) {
            edges.append(socket->getConnectedEdges());
        }
    }
    return edges;
}


void Node::remove() {
    qDebug() << "> Removing Node" << this;
    qDebug() << " - remove all edges from sockets";

    for (Socket* socket : inputs) {
        if (socket->hasConnectedEdge()) {
            qDebug() << "    - removing from socket:" << socket
                     << "edge:" << socket->getConnectedEdges();
            for(Edge* edge : socket->getConnectedEdges()){
                edge->remove();
            }
        }
    }
    for (Socket* socket : outputs) {
        if (socket->hasConnectedEdge()) {
            qDebug() << "    - removing from socket:" << socket
                     << "edge:" << socket->getConnectedEdges();
            for(Edge* edge : socket->getConnectedEdges()){
                edge->remove();
            }
        }
    }

    qDebug() << " - remove grNode";
    scene->graphicsScene()->removeItem(grNode);
    grNode = nullptr;

    qDebug() << " - remove node from the scene";
    scene->removeNode(this);

    qDebug() << " - everything was done.";
}

QJsonObject Node::serialize() const {
    QJsonObject obj;
    obj["type"] = nodeType();
    obj["id"] = id;
    obj["title"] = m_title;
    obj["pos_x"] = grNode ? grNode->scenePos().x() : 0.0;
    obj["pos_y"] = grNode ? grNode->scenePos().y() : 0.0;

    QJsonArray inputsArray;
    for (const Socket* socket : inputs) {
        if (socket) inputsArray.append(socket->serialize());
    }
    obj["inputs"] = inputsArray;

    QJsonArray outputsArray;
    for (const Socket* socket : outputs) {
        if (socket) outputsArray.append(socket->serialize());
    }
    obj["outputs"] = outputsArray;

    obj["content"] = grNode->getNodeContent();
    return obj;
}

void Node::deserialize(
    const QJsonObject& data,
    std::unordered_map<qint64, Serializable*>& hashmap,
    bool restoreId
    ) {

    if (restoreId) {
        // Set ID and add to hashmap
        id = static_cast<qint64>(data["id"].toDouble());
        hashmap[data["id"].toDouble()] = this;
    }
    // Position
    setPos(data["pos_x"].toDouble(), data["pos_y"].toDouble());

    // Title
    setTitle(data["title"].toString());

    // Extract inputs array
    QJsonArray inputsArray = data["inputs"].toArray();
    std::vector<QJsonObject> inputsList;
    for (const auto& v : inputsArray) {
        inputsList.push_back(v.toObject());
    }
    std::sort(inputsList.begin(), inputsList.end(), [](const QJsonObject& a, const QJsonObject& b) {
        int aKey = a["index"].toInt() + a["position"].toInt() * 10000;
        int bKey = b["index"].toInt() + b["position"].toInt() * 10000;
        return aKey < bKey;
    });

    // Deserialize inputs
    for (Socket* socket : inputs) {
            if (socket->getGraphicsSocket())
                scene->graphicsScene()->removeItem(socket->getGraphicsSocket());
            delete socket;
        }

        for (Socket* socket : outputs) {
            if (socket->getGraphicsSocket())
                scene->graphicsScene()->removeItem(socket->getGraphicsSocket());
            delete socket;
        }

        inputs.clear();
        outputs.clear();


    for (const auto& socketData : inputs)
        qDebug() << "socktee" << socketData; 
    
    for (const auto& socketData : inputsList) {
        auto* newSocket = new Socket(this,
                                     socketData["type"].toInt(),
                                     socketData["index"].toInt(),
                                     socketData["position"].toInt());
        newSocket->deserialize(socketData, hashmap, restoreId);
        inputs.push_back(newSocket);
    }

    // Extract outputs array
    QJsonArray outputsArray = data["outputs"].toArray();
    std::vector<QJsonObject> outputsList;
    for (const auto& v : outputsArray) {
        outputsList.push_back(v.toObject());
    }
    std::sort(outputsList.begin(), outputsList.end(), [](const QJsonObject& a, const QJsonObject& b) {
        int aKey = a["index"].toInt() + a["position"].toInt() * 10000;
        int bKey = b["index"].toInt() + b["position"].toInt() * 10000;
        return aKey < bKey;
    });

    for (const auto& socketData : outputs)
        qDebug() << "socktee" << socketData; 
    
    
    for (const auto& socketData : outputsList) {
        auto* newSocket = new Socket(this,
                                     socketData["type"].toInt(),
                                     socketData["index"].toInt(),
                                     socketData["position"].toInt());
        newSocket->deserialize(socketData, hashmap, restoreId);
        outputs.push_back(newSocket);
    }

    updateSockets();

    grNode->setNodeContent(data["content"]);
}


void Node::setNodeGraphicsItem(NodeGraphicsItem* nodeGraphicsItem){
    if(grNode){
        scene->graphicsScene()->removeItem(grNode);

    }
    grNode = nodeGraphicsItem;
    grNode->initUI();
    grNode->setTitle(m_title);

    scene->graphicsScene()->addItem(grNode);

    for (Socket* i : inputs) {
        i->getGraphicsSocket()->setParentItem(grNode);
        i->updateSocketPosition();
    }

    for (Socket* o : outputs) {
        o->getGraphicsSocket()->setParentItem(grNode);
        o->updateSocketPosition();
    }
}

int Node::getInputSocketPosition() const
{
    return inputSocketPosition;
}

int Node::getOutputSocketPosition() const
{
    return outputSocketPosition;
}

void Node::setInputSocketPosition(int value)
{
    inputSocketPosition = value;
    updateSockets();

}

void Node::setOutputSocketPosition(int value)
{
    outputSocketPosition = value;
    updateSockets();
}

bool Node::isDirty() const
{
    return m_isDirty;
}

void Node::markDirty(bool newValue)
{
    m_isDirty = newValue;
    if (m_isDirty)
        onMarkedDirty();
}

void Node::markChildrenDirty(bool newValue)
{
    for (Node* node : getChildrenNodes()) {
        node->markDirty(newValue);
    }
}

void Node::markDescendantsDirty(bool newValue)
{
    for (Node* node : getChildrenNodes()) {
        node->markDirty(newValue);
        node->markChildrenDirty(newValue);
    }
}

bool Node::isInvalid() const
{
    return m_isInvalid;
}

void Node::markInvalid(bool newValue)
{
    m_isInvalid = newValue;
    if (m_isInvalid)
        onMarkedInvalid();
}


void Node::markChildrenInvalid(bool newValue)
{
    for (Node* node : getChildrenNodes()) {
        node->markInvalid(newValue);
    }
}

void Node::markDescendantsInvalid(bool newValue)
{
    for (Node* node : getChildrenNodes()) {
        node->markInvalid(newValue);
        node->markChildrenInvalid(newValue);
    }
}

QVariant Node::eval()
{
    markDirty(false);
    markInvalid(false);
    return 0;
}

void Node::evalChildren()
{
    for (Node* node : getChildrenNodes()) {
        node->eval();
    }
}

std::vector<Node*> Node::getChildrenNodes() const
{
    std::vector<Node*> result;

    if (outputs.empty())
        return result;

    for (Socket* outSocket : outputs) {
        for (Edge* edge : outSocket->getConnectedEdges()) {
            Socket* other = edge->getOtherSocket(outSocket);
            if (other && other->getNode())
                result.push_back(other->getNode());
        }
    }

    return result;
}

void Node::onEdgeConnectionChanged(Edge* edge)
{
    qDebug() << this->nodeType()
             << "::onEdgeConnectionChanged"
             << edge;
}

void Node::onInputChanged(Edge* edge)
{
    qDebug() <<this->nodeType()
             << "::onInputChanged"
             << edge;

    markDirty();
    eval();
}

Node* Node::getInput(int index)
{
    if (index < 0 || index >= inputs.size())
        return nullptr;

    Socket* socket = inputs[index];
    const auto& edges = socket->getConnectedEdges();
    if (edges.empty())
        return nullptr;

    Socket* other = edges.front()->getOtherSocket(socket);
    return other ? other->getNode() : nullptr;
}

QList<Node*> Node::getInputs(int index)
{
    QList<Node*> result;

    if (index >= inputs.size())
        return result;

    Socket* inputSocket = inputs[index];

    for (Edge* edge : inputSocket->getConnectedEdges()) {
        Socket* otherSocket = edge->getOtherSocket(inputSocket);
        if (otherSocket && otherSocket->getNode())
            result.append(otherSocket->getNode());
    }

    return result;
}

QList<Node*> Node::getOutputs(int index)
{
    QList<Node*> result;

    if (index >= outputs.size())
        return result;

    Socket* outputSocket = outputs[index];

    for (Edge* edge : outputSocket->getConnectedEdges()) {
        Socket* otherSocket = edge->getOtherSocket(outputSocket);
        if (otherSocket && otherSocket->getNode())
            result.append(otherSocket->getNode());
    }

    return result;
}
