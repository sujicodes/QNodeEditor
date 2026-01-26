#include "CalculatorGraphicsNode.h"
#include "CalculatorNodeBase.h"


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
        QLabel* lbl = new QLabel("1");
        lbl->setAlignment(Qt::AlignLeft);
        return lbl;
    }

    if(!label.isEmpty()){
        return new QLabel(label);
    }

    return new QWidget();
}

