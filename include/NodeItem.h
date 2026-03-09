#ifndef NODEITEM_H
#define NODEITEM_H

#include <QGraphicsItem>
#include <QGraphicsTextItem>
#include <QGraphicsProxyWidget>
#include <QJsonObject>
#include <QJsonArray>
#include <QPen>
#include <QBrush>
#include <QFont>

#include "Serializable.h"

class Scene;
class SocketItem;
class Edge;

class NodeItem : public QGraphicsItem, public Serializable
{
public:
    NodeItem(Scene* scene,
             const QString& title = "Undefined Node",
             const QList<int>& in = {},
             const QList<int>& outs = {});
    virtual ~NodeItem();

    void addInput(SocketItem* input);
    void addOutput(SocketItem* output);

    std::pair<float, float> getSocketPosition(int index, int position, int type);
    
    int getInputSocketPosition() const;
    int getOutputSocketPosition() const;

    void setInputSocketPosition(int value);
    void setOutputSocketPosition(int value);

    void updateConnectedEdges();
    void updateSockets();
    QList<Edge*> getConnectedEdges();
    std::vector<NodeItem*> getChildrenNodes() const;

    NodeItem* getInput(int index = 0);
    QList<NodeItem*> getInputNodes(int index = 0);
    QList<NodeItem*> getOutputNodes(int index = 0);
    QList<SocketItem*> getInputSockets(){ return inputs; }
    QList<SocketItem*> getOutputSockets() { return outputs; }

    virtual void onEdgeConnectionChanged(Edge* edge);
    virtual void onInputChanged(Edge* edge);
    virtual QVariant eval();
    void evalChildren();
    virtual QString nodeType() const { return "Node"; }

    QJsonObject serialize() const override;
    void deserialize(const QJsonObject& data,
                     std::unordered_map<qint64, Serializable*>& hashmap,
                     bool restoreId = true) override;

    void setTitle(const QString& title);
    QString title() const { return m_title; }

    void initNode();
    virtual QWidget* setItemWidget() const;
    QWidget* getItemWidget() const {return itemWidget;}

    bool isDirty() const;
    void markDirty(bool newValue = true);
    void markChildrenDirty(bool newValue = true);
    void markDescendantsDirty(bool newValue = true);

    bool isInvalid() const;
    void markInvalid(bool newValue = true);
    void onMarkedInvalid() { return; }

    void markChildrenInvalid(bool newValue = true);
    void markDescendantsInvalid(bool newValue = true);

    QRectF boundingRect() const override;
    void paint(QPainter* painter,
               const QStyleOptionGraphicsItem* option,
               QWidget* widget = nullptr) override;

    void mouseReleaseEvent(QGraphicsSceneMouseEvent* event) override;

    float getWidth() const { return width; }
    float getHeight() const { return height; }
    float getTitleHeight() const { return titleHeight; }
    int getEdgeRoundness() const { return edgeRoundness; }
    int getEdgePadding() const { return edgePadding; }
    int getTitleHorizontalPadding() const { return titleHorizontalPadding; }
    int getTitleVerticalPadding() const { return titleVerticalPadding; }

    void setWidth(float w) { width = w; }
    void setHeight(float h) { height = h; }
    void setTitleHeight(float t) { titleHeight = t; }
    void setLastSelectedState(bool s) { lastSelectedState = s; }
    void setEdgeRoundness(int value) { edgeRoundness = value; }
    void setEdgePadding(int value) { edgePadding = value; }
    void setTitleHorizontalPadding(int value) { titleHorizontalPadding = value; }
    void setTitleVerticalPadding(int value) { titleVerticalPadding = value; }
    void onNodeMoved();
    void remove();

private:
    Scene* scene;
    QString m_title;

    QList<SocketItem*> inputs;
    QList<SocketItem*> outputs;

    QGraphicsTextItem* titleItem = nullptr;
    QGraphicsProxyWidget* graphicsProxyWidget = nullptr;
    QWidget* itemWidget = nullptr;

    float width = 180;
    float height = 240;
    float titleHeight = 24.0f;

    int edgeRoundness = 6;
    int edgePadding = 0;
    int titleHorizontalPadding = 8;
    int titleVerticalPadding = 10;

    int inputSocketPosition;
    int outputSocketPosition;

    bool lastSelectedState = false;

    bool m_isDirty = false;
    bool m_isInvalid = false;

    float socketSpacing = 22.0f;
    void ensureInitialized()
    {
        if (!m_initialized) {
            m_initialized = true;
            initNode();
        }
    }

    bool m_initialized = false;
    QList<int> in;
    QList<int> outs;
};

#endif