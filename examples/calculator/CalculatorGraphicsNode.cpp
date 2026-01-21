#include "CalculatorGraphicsNode.h"
#include "CalculatorNode.h"


CalculatorGraphicsNode::CalculatorGraphicsNode(Node* node)
    : NodeGraphicsItem(node)
{
    setWidth(160);
    setHeight(74);
    setEdgeSize(5);
    setPadding(8);
}

CalculatorNode* CalculatorGraphicsNode::getCalculatorNode() const
{
    return dynamic_cast<CalculatorNode*>(getNode());
}

QWidget* CalculatorGraphicsNode::setItemWidget() const
{

    auto* label = new QLabel("test");
    return label;
}

