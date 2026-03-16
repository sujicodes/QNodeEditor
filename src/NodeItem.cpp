#include "NodeItem.h"
#include "NodeEditorGraphicsScene.h"
#include "SocketItem.h"
#include "Edge.h"
#include "Theme.h"

#include <QPainter>
#include <QPainterPath>
#include <QVBoxLayout>
#include <QLabel>
#include <QTextEdit>
#include <QGraphicsSceneMouseEvent>
#include <QDebug>

NodeItem::NodeItem(NodeEditorGraphicsScene* scene,
                   const QString& title,
                   const int& in,
                   const int& outs)
    : scene(scene), m_inputSize(in), m_outputSize(outs), m_title(title)
{
    setFlag(ItemIsSelectable);
    setFlag(ItemIsMovable);


    scene->addItem(this);
    scene->addNode(this);

    m_penSelected = QPen(QColor(Theme::instance().nodeSelectedColor));
    m_brushBackground = QBrush(QColor(Theme::instance().nodeContentBackgroundColor));
    m_brushTitle = QBrush(QColor(Theme::instance().nodeTitleBackgroundColor));


}

void NodeItem::addInput(SocketItem* input)
{
    m_inputs.push_back(input);
}

void NodeItem::addOutput(SocketItem* output)
{
    m_outputs.push_back(output);
}

std::pair<float, float> NodeItem::getSocketPosition(int index, int position, int type)
{
    float x = (position == SocketItem::LEFT_TOP || position == SocketItem::LEFT_CENTER || position == SocketItem::LEFT_BOTTOM) ? 0.0f : m_width;
    float y = 0.0f;
    int numOutOf = (type == SocketItem::INPUT) ? m_inputs.size() : m_outputs.size();

    if (position == SocketItem::LEFT_BOTTOM || position == SocketItem::RIGHT_BOTTOM) {
        y = m_height - m_edgeRoundness - m_titleVerticalPadding - index * m_socketSpacing;
    } else if (position == SocketItem::LEFT_CENTER || position == SocketItem::RIGHT_CENTER) {
        float topOffset = m_titleHeight + 2 * m_titleVerticalPadding + m_edgePadding;
        float availableHeight = m_height - topOffset;
        y = topOffset + availableHeight / 2.0f + (index - 0.5f) * m_socketSpacing;
        if (numOutOf > 1)
            y -= m_socketSpacing * (numOutOf - 1) / 2.0f;
    } else if (position == SocketItem::LEFT_TOP || position == SocketItem::RIGHT_TOP) {
        y = m_titleHeight + m_titleVerticalPadding + m_edgeRoundness + index * m_socketSpacing;
    }

    return { x, y };
}

int NodeItem::getInputSocketPosition() const
{
    return m_inputSocketPosition;
}

int NodeItem::getOutputSocketPosition() const
{
    return m_outputSocketPosition;
}

void NodeItem::setInputSocketPosition(int value)
{
    m_inputSocketPosition = value;
    updateSockets();
}

void NodeItem::setOutputSocketPosition(int value)
{
    m_outputSocketPosition = value;
    updateSockets();
}

void NodeItem::updateConnectedEdges()
{
    for (SocketItem* socket : m_inputs)
        for (Edge* edge : socket->getConnectedEdges())
            edge->updatePositions();

    for (SocketItem* socket : m_outputs)
        for (Edge* edge : socket->getConnectedEdges())
            edge->updatePositions();
}

void NodeItem::updateSockets()
{
    for (SocketItem* socket : m_inputs) {
        socket->setPosition(SocketItem::LEFT_CENTER);
        socket->updateSocketPosition();
    }
    for (SocketItem* socket : m_outputs) {
        socket->setPosition(SocketItem::RIGHT_CENTER);
        socket->updateSocketPosition();
    }
}

QList<Edge*> NodeItem::getConnectedEdges()
{
    QList<Edge*> edges;
    for (SocketItem* socket : m_inputs)
        edges.append(socket->getConnectedEdges());
    for (SocketItem* socket : m_outputs)
        edges.append(socket->getConnectedEdges());
    return edges;
}

std::vector<NodeItem*> NodeItem::getChildrenNodes() const
{
    std::vector<NodeItem*> result;
    for (SocketItem* outSocket : m_outputs) {
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
    if (index < 0 || index >= m_inputs.size()) return nullptr;
    SocketItem* socket = m_inputs[index];
    if (socket->getConnectedEdges().empty()) return nullptr;
    SocketItem* other = socket->getConnectedEdges().front()->getOtherSocket(socket);
    return other ? other->getNode() : nullptr;
}

QList<NodeItem*> NodeItem::getInputNodes(int index)
{
    QList<NodeItem*> result;
    if (index >= m_inputs.size()) return result;
    SocketItem* socket = m_inputs[index];
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
    if (index >= m_outputs.size()) return result;
    SocketItem* socket = m_outputs[index];
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
    if (m_titleItem) m_titleItem->setPlainText(title);
}

void NodeItem::initNode()
{
    m_titleItem = new QGraphicsTextItem(this);
    m_titleItem->setDefaultTextColor(Qt::white);
    m_titleItem->setFont(QFont("Ubuntu", 10));
    m_titleItem->setPos(m_titleHorizontalPadding, 0);
    m_titleItem->setTextWidth(m_width - 2 * m_titleHorizontalPadding);
    m_titleItem->setPlainText(m_title);

    m_itemWidget = setItemWidget();
    m_graphicsProxyWidget = new QGraphicsProxyWidget(this);
    m_itemWidget->setGeometry(m_edgePadding, m_titleHeight + m_edgePadding,
                            m_width - 2 * m_edgePadding,
                            m_height - 2 * m_edgePadding - m_titleHeight);
    m_graphicsProxyWidget->setWidget(m_itemWidget);


    int counter = 0;
    for (int i = 0; i < m_inputSize; ++i)
        addInput(new SocketItem(this, SocketItem::INPUT, counter++, SocketItem::LEFT_CENTER));

    counter = 0;
    for (int i = 0; i < m_outputSize; ++i)
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

QRectF NodeItem::boundingRect() const { return QRectF(0, 0, m_width, m_height).normalized(); }

void NodeItem::paint(QPainter* painter,
                     const QStyleOptionGraphicsItem*,
                     QWidget*)
{
    // Title
    QPainterPath pathTitle;
    pathTitle.setFillRule(Qt::WindingFill);
    pathTitle.addRoundedRect(
        0,
        0,
        m_width,
        m_titleHeight,
        m_edgeRoundness,
        m_edgeRoundness
        );

    pathTitle.addRect(
        0,
        m_titleHeight - m_edgeRoundness,
        m_edgeRoundness,
        m_edgeRoundness
        );

    pathTitle.addRect(
        m_width - m_edgeRoundness,
        m_titleHeight - m_edgeRoundness,
        m_edgeRoundness,
        m_edgeRoundness
        );


    painter->setPen(Qt::NoPen);
    painter->setBrush(m_brushTitle);
    painter->drawPath(pathTitle.simplified());

    // Content
    QPainterPath pathContent;
    pathContent.setFillRule(Qt::WindingFill);
    pathContent.addRoundedRect(
        0,
        m_titleHeight,
        m_width,
        m_height - m_titleHeight,
        m_edgeRoundness,
        m_edgeRoundness
        );

    pathContent.addRect(
        0,
        m_titleHeight,
        m_edgeRoundness,
        m_edgeRoundness
        );

    pathContent.addRect(
        m_width - m_edgeRoundness,
        m_titleHeight,
        m_edgeRoundness,
        m_edgeRoundness
        );

    painter->setBrush(m_brushBackground);
    painter->drawPath(pathContent.simplified());

    // Outline
    QPainterPath pathOutline;
    pathOutline.addRoundedRect(0, 0, m_width, m_height, m_edgeRoundness, m_edgeRoundness);

    painter->setPen(isSelected() ? m_penSelected : m_penDefault);
    painter->setBrush(Qt::NoBrush);
    painter->drawPath(pathOutline.simplified());
}

void NodeItem::remove()
{
    qDebug() << "> Removing Node" << this;
    qDebug() << " - remove all edges from sockets";

    for(SocketItem* socket : m_inputs)
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

    for(SocketItem* socket : m_outputs)
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
        this->scene->removeItem(this);
        scene->removeNode(this);
        scene = nullptr;



    qDebug() << " - remove node from the scene";

    qDebug() << " - everything was done.";
}

void NodeItem::onNodeMoved()
{
    m_lastSelectedState = true;
}

QJsonObject NodeItem::serialize() const
{
    QJsonObject obj;
    obj["type"] = nodeType();
    obj["id"] = m_id;
    obj["title"] = m_title;
    obj["pos_x"] = scenePos().x();
    obj["pos_y"] = scenePos().y();

    QJsonArray inputsArray;
    for(const SocketItem* socket : m_inputs)
    {
        if(socket) inputsArray.append(socket->serialize());
    }

    obj["inputs"] = inputsArray;

    QJsonArray outputsArray;
    for(const SocketItem* socket : m_outputs)
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
        m_id = static_cast<qint64>(data["id"].toDouble());
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
    for(SocketItem* socket : m_inputs)
    {
        if(socket)
            scene->removeItem(socket);
        delete socket;
    }

    for(SocketItem* socket : m_outputs)
    {
        if(socket)
            scene->removeItem(socket);
        delete socket;
    }

    m_inputs.clear();
    m_outputs.clear();

    for(const auto& socketData : m_inputs)
        qDebug() << "socktee" << socketData; 
    
    for(const auto& socketData : inputsList)
    {
        auto* newSocket = new SocketItem(this,
                                     socketData["type"].toInt(),
                                     socketData["index"].toInt(),
                                     socketData["position"].toInt());
        newSocket->deserialize(socketData, hashmap, restoreId);
        m_inputs.push_back(newSocket);
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

    for(const auto& socketData : m_outputs)
        qDebug() << "socktee" << socketData; 
    
    for(const auto& socketData : outputsList)
    {
        auto* newSocket = new SocketItem(this,
                                     socketData["type"].toInt(),
                                     socketData["index"].toInt(),
                                     socketData["position"].toInt());
        newSocket->deserialize(socketData, hashmap, restoreId);
        m_outputs.push_back(newSocket);
    }

    updateSockets();
}
