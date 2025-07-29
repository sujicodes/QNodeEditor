#include "NodeGraphicsItem.h"
#include <QPainter>
#include <QPainterPath>
#include <QGraphicsProxyWidget>
#include <QVBoxLayout>
#include <QLabel>
#include <QTextEdit>

NodeGraphicsItem::NodeGraphicsItem(QGraphicsItem *parent)
    : QGraphicsItem(parent)
{
    initTitle();
    setTitle("Node Graphics Item");
    initUI();
    itemWidget = setItemWidget();
    initItemWidget();
}

void NodeGraphicsItem::initUI()
{
    setFlag(QGraphicsItem::ItemIsSelectable);
    setFlag(QGraphicsItem::ItemIsMovable);
}

void NodeGraphicsItem::initTitle()
{
    titleItem = new QGraphicsTextItem(this);
    titleItem->setDefaultTextColor(_titleColor);
    titleItem->setFont(_titleFont);
    titleItem->setPos(padding, 0);
    titleItem->setTextWidth(width - 2 * padding);
}

QRectF NodeGraphicsItem::boundingRect() const
{
    return QRectF(
               0,
               0,
               2 * edgeSize + width,
               2 * edgeSize + height
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
    graphicsProxyWidget = new QGraphicsProxyWidget(this);
    itemWidget->setGeometry(edgeSize, titleHeight+edgeSize, 
            width-2*edgeSize, height-2*edgeSize-titleHeight);
    graphicsProxyWidget->setWidget(itemWidget);


}
void NodeGraphicsItem::paint(QPainter *painter, const QStyleOptionGraphicsItem *, QWidget *)
{
    // Title
    QPainterPath pathTitle;
    pathTitle.setFillRule(Qt::WindingFill);
    pathTitle.addRoundedRect(0, 0, width, titleHeight, edgeSize, edgeSize);
    pathTitle.addRect(0, titleHeight - edgeSize, edgeSize, edgeSize);
    pathTitle.addRect(width - edgeSize, titleHeight - edgeSize, edgeSize, edgeSize);

    painter->setPen(Qt::NoPen);
    painter->setBrush(brushTitle);
    painter->drawPath(pathTitle.simplified());

    // Content
    QPainterPath pathContent;
    pathContent.setFillRule(Qt::WindingFill);
    pathContent.addRoundedRect(0, titleHeight, width, height - titleHeight, edgeSize, edgeSize);
    pathContent.addRect(0, titleHeight, edgeSize, edgeSize);
    pathContent.addRect(width - edgeSize, titleHeight, edgeSize, edgeSize);

    painter->setBrush(brushBackground);
    painter->drawPath(pathContent.simplified());

    // Outline
    QPainterPath pathOutline;
    pathOutline.addRoundedRect(0, 0, width, height, edgeSize, edgeSize);

    painter->setPen(isSelected() ? penSelected : penDefault);
    painter->setBrush(Qt::NoBrush);
    painter->drawPath(pathOutline.simplified());
}
