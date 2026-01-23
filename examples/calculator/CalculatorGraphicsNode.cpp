#include "CalculatorGraphicsNode.h"
#include "CalculatorNodeBase.h"


CalculatorGraphicsNode::CalculatorGraphicsNode(Node* node)
    : NodeGraphicsItem(node)
{
    setWidth(160);
    setHeight(74);
    setEdgeSize(5);
    setPadding(8);
}

CalculatorNodeBase* CalculatorGraphicsNode::getCalculatorNodeBase() const
{
    auto* calcNode = dynamic_cast<CalculatorNodeBase*>(getNode());

    qDebug() << typeid(*calcNode).name();         // MultiplyNode
    qDebug() << calcNode->getContentLabel();  
    qDebug() << "IMNRWRWRESAS";
    return calcNode;
}

QWidget* CalculatorGraphicsNode::setItemWidget() const

{
    auto* calcNode = getCalculatorNodeBase();
    if(!calcNode) return new QWidget();

    QString label = calcNode->getContentLabel();  // virtual call
    if(!label.isEmpty()){
        return new QLabel(label);
    }

    return new QWidget();
}

