
#include "Edge.h"
#include "DirectEdgeGraphicsPathItem.h"
#include "BezierEdgeGraphicsPathItem.h"
#include "Scene.h"
#include "SocketItem.h"
#include "NodeItem.h"

Edge::Edge(Scene* scene, SocketItem* startSocket, SocketItem* endSocket, int type)
    : scene(scene), m_startSocket(startSocket), m_endSocket(endSocket)

{
    setStartSocket(startSocket);
    setEndSocket(endSocket);
    grEdge = new BezierEdgeGraphicsPathItem(this);
    scene->graphicsScene()->addItem(grEdge);
    scene->addEdge(this);
    if(startSocket)
    {
        updatePositions();
    }
}

Edge::~Edge()
{
    remove();
}

void Edge::updatePositions()
{
    QPointF sourcePos = m_startSocket->getSocketPosition();
    QPointF nodePos = m_startSocket->getNode()->pos();
    sourcePos.rx() += nodePos.x();
    sourcePos.ry() += nodePos.y();
    grEdge->setSource(sourcePos);

    if(m_endSocket)
    {
        QPointF endPos = m_endSocket->getSocketPosition();
        QPointF endNodePos = m_endSocket->getNode()->pos();
        endPos.rx() += endNodePos.x();
        endPos.ry() += endNodePos.y();
        grEdge->setDestination(endPos);
    }
    else
    {
        grEdge->setDestination(sourcePos);
    }

    grEdge->update();
}

void Edge::setStartSocket(SocketItem* socket)
{
    if(m_startSocket) m_startSocket->removeEdge(this);

    m_startSocket = socket;
    
    if(socket) m_startSocket->addEdge(this);
}

void Edge::setEndSocket(SocketItem* socket)
{
    if (m_endSocket) m_endSocket->removeEdge(this);
    
    m_endSocket = socket;
    
    if(socket) m_endSocket->addEdge(this);
}

void Edge::removeFromSockets()
{
    if(m_startSocket) m_startSocket->removeEdge(this);
    if(m_endSocket) m_endSocket->removeEdge(this);

    m_startSocket = nullptr;
    m_endSocket = nullptr;
}

void Edge::remove()
{
    std::vector<SocketItem*> oldSockets = { m_startSocket, m_endSocket };

    removeFromSockets();
    if(scene && grEdge)
    {
        scene->graphicsScene()->removeItem(grEdge);
        grEdge = nullptr;
    }

    if(scene) scene->removeEdge(this);

    qDebug() << " - everything is done.";

    try
    {
        // notify nodes from old sockets
        for(SocketItem* socket : oldSockets)
        {
            if(socket && socket->getNode())
            {
                socket->getNode()->onEdgeConnectionChanged(this);

                if(socket->isInput())
                    socket->getNode()->onInputChanged(this);
            }
        }
    }
    catch (...)
    {
        qDebug() << "Exception while removing edge";
    }
}

QJsonObject Edge::serialize() const
{
    QJsonObject obj;
    obj["id"] = static_cast<qint64>(id);
    // Assuming edge_type is convertible to int or QString

    obj["start"] = m_startSocket ? static_cast<qint64>(m_startSocket->getId()) : QJsonValue();
    obj["end"] = m_endSocket ? static_cast<qint64>(m_endSocket->getId()) : QJsonValue();

    return obj;
}

void Edge::deserialize(
    const QJsonObject& data,
    std::unordered_map<qint64, Serializable*>& hashmap,
    bool restoreId
    ) 
{
    if (restoreId)
    {
        // Set ID and add to hashmap
        id = static_cast<qint64>(data["id"].toDouble());
    }

    qint64 startId = static_cast<qint64>(data["start"].toDouble());
    auto startIt = hashmap.find(startId);

    if(startIt != hashmap.end())
    {
        setStartSocket(dynamic_cast<SocketItem*>(startIt->second));
    }
    else
    {
        remove();
        return;
    }

    qint64 endId = static_cast<qint64>(data["end"].toDouble());
    auto endIt = hashmap.find(endId);
    if(endIt != hashmap.end())
    {
        setEndSocket(dynamic_cast<SocketItem*>(endIt->second));
    }
    else
    {
        remove();
        return;
    }

    if(m_startSocket)
    {
        updatePositions();
    }
}