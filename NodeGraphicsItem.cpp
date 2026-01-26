#include "NodeGraphicsItem.h"
#include <QPainter>
#include <QPainterPath>
#include <QGraphicsProxyWidget>
#include <QVBoxLayout>
#include <QLabel>
#include <QTextEdit>

#include "Node.h"
#include "Scene.h"
#include "history.h"
#include "UndoCommands.h"
#include <QObject>
#include <qgraphicssceneevent.h>

NodeGraphicsItem::NodeGraphicsItem(Node *node, QGraphicsItem *parent)
    : QGraphicsItem(parent)
    , node(node)
{

}

void NodeGraphicsItem::initUI()
{
    setFlag(QGraphicsItem::ItemIsSelectable, true);
    setFlag(QGraphicsItem::ItemIsMovable, true);
    initTitle();
    setTitle("Node Graphics Item");
    initItemWidget();
}

void NodeGraphicsItem::initTitle()
{
    titleItem = new QGraphicsTextItem(this);
    titleItem->setDefaultTextColor(_titleColor);
    titleItem->setFont(_titleFont);
    titleItem->setPos(getTitleHorizontalPadding(), 0);
    titleItem->setTextWidth(width - 2 * getTitleHorizontalPadding());
}

QRectF NodeGraphicsItem::boundingRect() const
{
    return QRectF(
               0,
               0,
                width,
                height
               ).normalized();
}

void NodeGraphicsItem::setTitle(const QString &title)
{
    _title = title;
    if (titleItem)
        titleItem->setPlainText(_title);
}

QString NodeGraphicsItem::title() const
{
    return _title;
}

/**
 * @brief Sets the widget inside the Node.
 *
 * @return QWidget of what is diplayed.
 */
QWidget* NodeGraphicsItem::setItemWidget() const
{
    QWidget *contentWidget = new QWidget();

    // Set layout
    QVBoxLayout *layout = new QVBoxLayout(contentWidget);
    layout->setContentsMargins(0, 0, 0, 0);

    // Add label
    QLabel *label = new QLabel("Some Title");
    layout->addWidget(label);

    // Add text edit
    QTextEdit *textEdit = new QTextEdit("foo");
    layout->addWidget(textEdit);
    return contentWidget;

}

void NodeGraphicsItem::initItemWidget(){
    itemWidget = setItemWidget();
    graphicsProxyWidget = new QGraphicsProxyWidget(this);
    itemWidget->setGeometry(
        edgePadding,
        titleHeight + edgePadding,
        width - 2 * edgePadding,
        height - 2 * edgePadding - titleHeight
        );
    graphicsProxyWidget->setWidget(itemWidget);


}
void NodeGraphicsItem::paint(QPainter *painter, const QStyleOptionGraphicsItem *, QWidget *)
{
    // Title
    QPainterPath pathTitle;
    pathTitle.setFillRule(Qt::WindingFill);
    pathTitle.addRoundedRect(
        0,
        0,
        width,
        titleHeight,
        edgeRoundness,
        edgeRoundness
        );

    pathTitle.addRect(
        0,
        titleHeight - edgeRoundness,
        edgeRoundness,
        edgeRoundness
        );

    pathTitle.addRect(
        width - edgeRoundness,
        titleHeight - edgeRoundness,
        edgeRoundness,
        edgeRoundness
        );


    painter->setPen(Qt::NoPen);
    painter->setBrush(brushTitle);
    painter->drawPath(pathTitle.simplified());

    // Content
    QPainterPath pathContent;
    pathContent.setFillRule(Qt::WindingFill);
    pathContent.addRoundedRect(
        0,
        titleHeight,
        width,
        height - titleHeight,
        edgeRoundness,
        edgeRoundness
        );

    pathContent.addRect(
        0,
        titleHeight,
        edgeRoundness,
        edgeRoundness
        );

    pathContent.addRect(
        width - edgeRoundness,
        titleHeight,
        edgeRoundness,
        edgeRoundness
        );

    painter->setBrush(brushBackground);
    painter->drawPath(pathContent.simplified());

    // Outline
    QPainterPath pathOutline;
    pathOutline.addRoundedRect(0, 0, width, height, edgeRoundness, edgeRoundness);

    painter->setPen(isSelected() ? penSelected : penDefault);
    painter->setBrush(Qt::NoBrush);
    painter->drawPath(pathOutline.simplified());
}

void NodeGraphicsItem::mouseReleaseEvent(QGraphicsSceneMouseEvent* event)
{
    QGraphicsItem::mouseReleaseEvent(event);
    if (node->getScene()->isEdgeDragging())
        event->ignore();
        return;

    bool current = isSelected();

    if (lastSelectedState != current ||  node->getScene()->graphicsScene()->getLastSelectedItems() != node->getScene()->graphicsScene()->selectedItems())
    {
        node->getScene()->resetLastSelectedStates();
        lastSelectedState = current;
        emit node->getScene()->graphicsScene()->itemSelected();
    }
}

void NodeGraphicsItem::onNodeMoved() {
    lastSelectedState = true;
}


int NodeGraphicsItem::getEdgeRoundness() const
{
    return edgeRoundness;
}

int NodeGraphicsItem::getEdgePadding() const
{
    return edgePadding;
}

int NodeGraphicsItem::getTitleHorizontalPadding() const
{
    return titleHorizontalPadding;
}

int NodeGraphicsItem::getTitleVerticalPadding() const
{
    return titleVerticalPadding;
}

void NodeGraphicsItem::setEdgeRoundness(int value)
{
    edgeRoundness = value;
}

void NodeGraphicsItem::setEdgePadding(int value)
{
    edgePadding = value;
}

void NodeGraphicsItem::setTitleHorizontalPadding(int value)
{
    titleHorizontalPadding = value;
}

void NodeGraphicsItem::setTitleVerticalPadding(int value)
{
    titleVerticalPadding = value;
}
