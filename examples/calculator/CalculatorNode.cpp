#include "CalculatorNode.h"
#include "CalculatorGraphicsNode.h"

CalculatorNode::CalculatorNode(
    Scene* scene,
    const QString& opTitle,
    const std::vector<int>& inputs,
    const std::vector<int>& outputs
)
    : Node(scene, opTitle, inputs, outputs)
{
    setNodeGraphicsItem(new CalculatorGraphicsNode(this));
    qDebug() << "IM NEWWWWWWW";
}


QJsonObject CalculatorNode::serialize() const {
    QJsonObject obj = Node::serialize();
    return obj;
}