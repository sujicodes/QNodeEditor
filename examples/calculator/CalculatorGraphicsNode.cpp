#include "CalculatorGraphicsNode.h"
#include "CalculatorNodeBase.h"
#include "CalculatorNodes.h"
#include <qlineedit.h>
#include <qpainter.h>


CalculatorGraphicsNode::CalculatorGraphicsNode(Node* node)
    : NodeGraphicsItem(node)
{
    setWidth(160);
    setHeight(74);
    setEdgeRoundness(10);
    setEdgePadding(10);
    setTitleHorizontalPadding(4);
    setTitleVerticalPadding(4);
}

CalculatorNodeBase* CalculatorGraphicsNode::getCalculatorNodeBase() const
{
    auto* calcNode = dynamic_cast<CalculatorNodeBase*>(getNode());

    qDebug() << typeid(*calcNode).name();
    qDebug() << calcNode->getContentLabel();  
    qDebug() << "IMNRWRWRESAS";
    return calcNode;
}

QWidget* CalculatorGraphicsNode::setItemWidget() const

{
    auto* calcNode = getCalculatorNodeBase();
    if(!calcNode) return new QWidget();

    QString label = calcNode->getContentLabel();
    // refactor below and combine node obj and graphics item
    if(calcNode->nodeType() == "OutputNode"){
        QLabel* lbl = new QLabel("42");
        lbl->setAlignment(Qt::AlignLeft);
        return lbl;

    } else if (calcNode->nodeType() == "InputNode"){

        InputNode* inputNode = dynamic_cast<InputNode*>(calcNode);
        if (inputNode) {
            QLineEdit* lbl = new QLineEdit(inputNode->getValue().toString() );
            lbl->setAlignment(Qt::AlignLeft);
            QObject::connect(
                lbl,
                &QLineEdit::textChanged,
                [inputNode](const QString& text)
                {
                    inputNode->setValue(text); // setValue is a normal method
                    inputNode->onInputChanged(nullptr);
                }
            );
            return lbl;

        }
    }

    if(!label.isEmpty()){
        return new QLabel(label);
    }

    return new QWidget();
}


void CalculatorGraphicsNode::paint(QPainter* painter,
                                   const QStyleOptionGraphicsItem* option,
                                   QWidget* widget)
{
    // Call base class paint first (VERY important)
    NodeGraphicsItem::paint(painter, option, widget);

    if (!getNode())
        return;

    qreal offset = 24.0;

    if (getNode()->isDirty())
        offset = 0.0;
    else if (getNode()->isInvalid())
        offset = 48.0;

    painter->drawImage(
        QRectF(-10.0, -10.0, 24.0, 24.0),   // target rect
        m_icons,                            // source image
        QRectF(offset, 0.0, 24.0, 24.0)     // source rect
        );
}
