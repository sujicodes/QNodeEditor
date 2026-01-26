#include "CalculatorNodeBase.h"
#include "CalculatorGraphicsNode.h"
#include "Socket.h"

CalculatorNodeBase::CalculatorNodeBase(
    Scene* scene,
    const QString& opTitle,
    const QList<int>& inputs,
    const QList<int>& outputs
)
    : Node(scene, opTitle, inputs, outputs)
{
    setInputSocketPosition(Socket::LEFT_CENTER);
    setOutputSocketPosition(Socket::RIGHT_CENTER);
}


QJsonObject CalculatorNodeBase::serialize() const {
    QJsonObject obj = Node::serialize();
    return obj;
}


void CalculatorNodeBase::initGraphics()
{
    setNodeGraphicsItem(new CalculatorGraphicsNode(this));
}
