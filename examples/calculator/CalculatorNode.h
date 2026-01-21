#ifndef CALCULATORNODE_H
#define CALCULATORNODE_H
#include "Node.h"
#include <QString>
#include <QVector>
#include "NodeRegistry.h"

class CalculatorNode : public Node
{
public:
    // Constructor
    CalculatorNode(
        Scene* scene,
        int opCode = 1,
        const QString& opTitle= "test",
        const std::vector<int>& inputs = {2, 2},
        const std::vector<int>& outputs = {1}
    );


    int m_opCode;
    QString m_opTitle;
    QString m_contentLabel;
    QString m_contentLabelObjName;
    QJsonObject serialize() const override;

};

REGISTER_NODE(CalculatorNode, "CalculatorNode");

#endif // CALCULATORNODE_H
