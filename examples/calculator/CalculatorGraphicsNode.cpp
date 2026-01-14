#include "CalculatorGraphicsNode.h"

CalculatorGraphicsNode::CalculatorGraphicsNode(Node* node)
    : NodeGraphicsItem(node)
{
    setWidth(160);
    setHeight(74);
    setEdgeSize(5);
    setPadding(8);
}
