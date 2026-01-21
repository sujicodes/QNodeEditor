#include "CalculatorNode.h"
#include "CalculatorGraphicsNode.h"

CalculatorNode::CalculatorNode(
    Scene* scene,
    int opCode,
    const QString& opTitle,
    const std::vector<int>& inputs,
    const std::vector<int>& outputs
)
    : Node(scene, opTitle, inputs, outputs),
      m_opCode(opCode),
      m_opTitle(opTitle)
{
    setNodeGraphicsItem(new CalculatorGraphicsNode(this));
    m_contentLabel = "";
    m_contentLabelObjName = "calc_node_bg";
    qDebug() << "IM NEWWWWWWW";
}


QJsonObject CalculatorNode::serialize() const {
    QJsonObject obj = Node::serialize();
    obj["type"] = QString("CalculatorNode");
    return obj;
}