#include "NodeItem.h"
#include "Scene.h"
#include "SocketItem.h"
#include "Edge.h"

#include <QPainter>
#include <QPainterPath>
#include <QVBoxLayout>
#include <QLabel>
#include <QTextEdit>
#include <QGraphicsSceneMouseEvent>
#include <QDebug>

NodeItem::NodeItem(Scene* scene,
                   const QString& title,
                   const QList<int>& in,
                   const QList<int>& outs)
    : scene(scene), in(in), outs(outs), m_title(title)
{
    setFlag(ItemIsSelectable);
    setFlag(ItemIsMovable);


    scene->graphicsScene()->addItem(this);
    scene->addNode(this);

}

void NodeItem::addInput(SocketItem* input)
{
    inputs.push_back(input);
}

void NodeItem::addOutput(SocketItem* output)
{
    outputs.push_back(output);
}

std::pair<float, float> NodeItem::getSocketPosition(int index, int position, int type)
{
    float x = (position == SocketItem::LEFT_TOP || position == SocketItem::LEFT_CENTER || position == SocketItem::LEFT_BOTTOM) ? 0.0f : width;
    float y = 0.0f;
    int numOutOf = (type == SocketItem::INPUT) ? inputs.size() : outputs.size();

    if (position == SocketItem::LEFT_BOTTOM || position == SocketItem::RIGHT_BOTTOM) {
        y = height - edgeRoundness - titleVerticalPadding - index * socketSpacing;
    } else if (position == SocketItem::LEFT_CENTER || position == SocketItem::RIGHT_CENTER) {
        float topOffset = titleHeight + 2 * titleVerticalPadding + edgePadding;
        float availableHeight = height - topOffset;
        y = topOffset + availableHeight / 2.0f + (index - 0.5f) * socketSpacing;
        if (numOutOf > 1)
            y -= socketSpacing * (numOutOf - 1) / 2.0f;
    } else if (position == SocketItem::LEFT_TOP || position == SocketItem::RIGHT_TOP) {
        y = titleHeight + titleVerticalPadding + edgeRoundness + index * socketSpacing;
    }

    return { x, y };
}

int NodeItem::getInputSocketPosition() const
{
    return inputSocketPosition;
}

int NodeItem::getOutputSocketPosition() const
{
    return outputSocketPosition;
}

void NodeItem::setInputSocketPosition(int value)
{
    inputSocketPosition = value;
    updateSockets();
}

void NodeItem::setOutputSocketPosition(int value)
{
    outputSocketPosition = value;
    updateSockets();
}

void NodeItem::updateConnectedEdges()
{
    for (SocketItem* socket : inputs)
        for (Edge* edge : socket->getConnectedEdges())
            edge->updatePositions();

    for (SocketItem* socket : outputs)
        for (Edge* edge : socket->getConnectedEdges())
            edge->updatePositions();
}

void NodeItem::updateSockets()
{
    for (SocketItem* socket : inputs) {
        socket->setPosition(SocketItem::LEFT_CENTER);
        socket->updateSocketPosition();
    }
    for (SocketItem* socket : outputs) {
        socket->setPosition(SocketItem::RIGHT_CENTER);
        socket->updateSocketPosition();
    }
}

QList<Edge*> NodeItem::getConnectedEdges()
{
    QList<Edge*> edges;
    for (SocketItem* socket : inputs)
        edges.append(socket->getConnectedEdges());
    for (SocketItem* socket : outputs)
        edges.append(socket->getConnectedEdges());
    return edges;
}

std::vector<NodeItem*> NodeItem::getChildrenNodes() const
{
    std::vector<NodeItem*> result;
    for (SocketItem* outSocket : outputs) {
        for (Edge* edge : outSocket->getConnectedEdges()) {
            SocketItem* other = edge->getOtherSocket(outSocket);
            if (other && other->getNode())
                result.push_back(other->getNode());
        }
    }
    return result;
}

NodeItem* NodeItem::getInput(int index)
{
    if (index < 0 || index >= inputs.size()) return nullptr;
    SocketItem* socket = inputs[index];
    if (socket->getConnectedEdges().empty()) return nullptr;
    SocketItem* other = socket->getConnectedEdges().front()->getOtherSocket(socket);
    return other ? other->getNode() : nullptr;
}

QList<NodeItem*> NodeItem::getInputNodes(int index)
{
    QList<NodeItem*> result;
    if (index >= inputs.size()) return result;
    SocketItem* socket = inputs[index];
    for (Edge* edge : socket->getConnectedEdges()) {
        SocketItem* other = edge->getOtherSocket(socket);
        if (other && other->getNode())
            result.append(other->getNode());
    }
    return result;
}

QList<NodeItem*> NodeItem::getOutputNodes(int index)
{
    QList<NodeItem*> result;
    if (index >= outputs.size()) return result;
    SocketItem* socket = outputs[index];
    for (Edge* edge : socket->getConnectedEdges()) {
        SocketItem* other = edge->getOtherSocket(socket);
        if (other && other->getNode())
            result.append(other->getNode());
    }
    return result;
}

void NodeItem::onEdgeConnectionChanged(Edge* edge)
{
    qDebug() << "NodeItem::onEdgeConnectionChanged" << edge;
}

void NodeItem::onInputChanged(Edge* edge)
{
    qDebug() << "NodeItem::onInputChanged" << edge;
    markDirty();
    eval();
}

QVariant NodeItem::eval()
{
    markDirty(false);
    markInvalid(false);
    return QVariant(0);
}

void NodeItem::evalChildren()
{
    for (NodeItem* node : getChildrenNodes())
        node->eval();
}

void NodeItem::setTitle(const QString& title)
{
    m_title = title;
    if (titleItem) titleItem->setPlainText(title);
}

void NodeItem::initNode()
{
    titleItem = new QGraphicsTextItem(this);
    titleItem->setDefaultTextColor(Qt::white);
    titleItem->setFont(QFont("Ubuntu", 10));
    titleItem->setPos(titleHorizontalPadding, 0);
    titleItem->setTextWidth(width - 2 * titleHorizontalPadding);
    titleItem->setPlainText(m_title);

    itemWidget = setItemWidget();
    graphicsProxyWidget = new QGraphicsProxyWidget(this);
    itemWidget->setGeometry(edgePadding, titleHeight + edgePadding,
                            width - 2 * edgePadding,
                            height - 2 * edgePadding - titleHeight);
    graphicsProxyWidget->setWidget(itemWidget);


    int counter = 0;
    for (int i : in)
        addInput(new SocketItem(this, SocketItem::INPUT, counter++, SocketItem::LEFT_CENTER));

    counter = 0;
    for (int i : outs)
        addOutput(new SocketItem(this, SocketItem::OUTPUT, counter++, SocketItem::RIGHT_CENTER));

    updateSockets();
}

QWidget* NodeItem::setItemWidget() const
{
    QWidget* widget = new QWidget();
    QVBoxLayout* layout = new QVBoxLayout(widget);
    layout->setContentsMargins(0, 0, 0, 0);
    layout->addWidget(new QLabel("Node Content"));
    layout->addWidget(new QTextEdit("Value"));
    return widget;
}

void NodeItem::mouseReleaseEvent(QGraphicsSceneMouseEvent* event)
{
    QGraphicsItem::mouseReleaseEvent(event);
}

bool NodeItem::isDirty() const { return m_isDirty; }
void NodeItem::markDirty(bool newValue) { m_isDirty = newValue; }

void NodeItem::markChildrenDirty(bool newValue)
{
    for(NodeItem* node : getChildrenNodes())
    {
        node->markDirty(newValue);
    }
}

void NodeItem::markDescendantsDirty(bool newValue)
{
    for(NodeItem* node : getChildrenNodes())
    {
        node->markDirty(newValue);
        node->markChildrenDirty(newValue);
    }
}

bool NodeItem::isInvalid() const 
{ 
    return m_isInvalid; 
}

void NodeItem::markInvalid(bool newValue) 
{ 
    m_isInvalid = newValue;
}

void NodeItem::markChildrenInvalid(bool newValue)
{
    for(NodeItem* node : getChildrenNodes())
    {
        node->markInvalid(newValue);
    }
}

void NodeItem::markDescendantsInvalid(bool newValue)
{
    for(NodeItem* node : getChildrenNodes())
    {
        node->markInvalid(newValue);
        node->markChildrenInvalid(newValue);
    }
}

QRectF NodeItem::boundingRect() const { return QRectF(0, 0, width, height).normalized(); }

void NodeItem::paint(QPainter* painter,
                     const QStyleOptionGraphicsItem*,
                     QWidget*)
{
    //ensureInitialized();
    QPainterPath pathTitle;
    pathTitle.setFillRule(Qt::WindingFill);
    pathTitle.addRoundedRect(0, 0, width, titleHeight, edgeRoundness, edgeRoundness);
    painter->setPen(Qt::NoPen);
    painter->setBrush(QBrush(QColor("#FF313131")));
    painter->drawPath(pathTitle.simplified());

    QPainterPath pathContent;
    pathContent.setFillRule(Qt::WindingFill);
    pathContent.addRoundedRect(0, titleHeight, width, height - titleHeight, edgeRoundness, edgeRoundness);
    painter->setBrush(QBrush(QColor("#E3212121")));
    painter->drawPath(pathContent.simplified());

    QPainterPath pathOutline;
    pathOutline.addRoundedRect(0, 0, width, height, edgeRoundness, edgeRoundness);
    painter->setPen(QPen(QColor("#7F000000")));
    painter->setBrush(Qt::NoBrush);
    painter->drawPath(pathOutline.simplified());
}

void NodeItem::remove()
{
    qDebug() << "> Removing Node" << this;
    qDebug() << " - remove all edges from sockets";

    for(SocketItem* socket : inputs)
    {
        if(socket->hasConnectedEdge())
        {
            qDebug() << "    - removing from socket:" << socket
                     << "edge:" << socket->getConnectedEdges();

            for(Edge* edge : socket->getConnectedEdges())
            {
                edge->remove();
            }
        }
    }

    for(SocketItem* socket : outputs)
    {
        if(socket->hasConnectedEdge())
        {
            qDebug() << "    - removing from socket:" << socket
                     << "edge:" << socket->getConnectedEdges();
            
            for(Edge* edge : socket->getConnectedEdges())
            {
                edge->remove();
            }
        }
    }

    qDebug() << " - remove grNode";
    if (this->scene)
        this->scene->graphicsScene()->removeItem(this);
        scene->removeNode(this);
        scene = nullptr;



    qDebug() << " - remove node from the scene";

    qDebug() << " - everything was done.";
}

void NodeItem::onNodeMoved()
{
    lastSelectedState = true;
}

QJsonObject NodeItem::serialize() const
{
    QJsonObject obj;
    obj["type"] = nodeType();
    obj["id"] = id;
    obj["title"] = m_title;
    obj["pos_x"] = scenePos().x();
    obj["pos_y"] = scenePos().y();

    QJsonArray inputsArray;
    for(const SocketItem* socket : inputs)
    {
        if(socket) inputsArray.append(socket->serialize());
    }

    obj["inputs"] = inputsArray;

    QJsonArray outputsArray;
    for(const SocketItem* socket : outputs)
    {
        if(socket) outputsArray.append(socket->serialize());
    }

    obj["outputs"] = outputsArray;

    return obj;
}

void NodeItem::deserialize(
    const QJsonObject& data,
    std::unordered_map<qint64, Serializable*>& hashmap,
    bool restoreId
    )
{
    if(restoreId)
    {
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

    for(const auto& v : inputsArray)
    {
        inputsList.push_back(v.toObject());
    }

    std::sort(inputsList.begin(), inputsList.end(), [](const QJsonObject& a, const QJsonObject& b)
    {
        int aKey = a["index"].toInt() + a["position"].toInt() * 10000;
        int bKey = b["index"].toInt() + b["position"].toInt() * 10000;
        return aKey < bKey;
    });

    // Deserialize inputs
    for(SocketItem* socket : inputs)
    {
        if(socket)
            scene->graphicsScene()->removeItem(socket);
        delete socket;
    }

    for(SocketItem* socket : outputs)
    {
        if(socket)
            scene->graphicsScene()->removeItem(socket);
        delete socket;
    }

    inputs.clear();
    outputs.clear();

    for(const auto& socketData : inputs)
        qDebug() << "socktee" << socketData; 
    
    for(const auto& socketData : inputsList)
    {
        auto* newSocket = new SocketItem(this,
                                     socketData["type"].toInt(),
                                     socketData["index"].toInt(),
                                     socketData["position"].toInt());
        newSocket->deserialize(socketData, hashmap, restoreId);
        inputs.push_back(newSocket);
    }

    // Extract outputs array
    QJsonArray outputsArray = data["outputs"].toArray();
    std::vector<QJsonObject> outputsList;

    for(const auto& v : outputsArray)
    {
        outputsList.push_back(v.toObject());
    }
    
    std::sort(outputsList.begin(), outputsList.end(), [](const QJsonObject& a, const QJsonObject& b)
    {
        int aKey = a["index"].toInt() + a["position"].toInt() * 10000;
        int bKey = b["index"].toInt() + b["position"].toInt() * 10000;
        return aKey < bKey;
    });

    for(const auto& socketData : outputs)
        qDebug() << "socktee" << socketData; 
    
    for(const auto& socketData : outputsList)
    {
        auto* newSocket = new SocketItem(this,
                                     socketData["type"].toInt(),
                                     socketData["index"].toInt(),
                                     socketData["position"].toInt());
        newSocket->deserialize(socketData, hashmap, restoreId);
        outputs.push_back(newSocket);
    }

    updateSockets();
}
