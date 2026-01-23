#ifndef CALCULATORNODE_H
#define CALCULATORNODE_H
#include "Node.h"
#include <QString>
#include <QVector>
#include "NodeRegistry.h"

class CalculatorNode : public Node
{
public:
    static constexpr int OpCode = 0;
    static inline const QString OpTitle = "Undefined";
    static inline const QString contentLabel = "";
    static inline const QString contentLabelObjName = "calc_node_bg";


    CalculatorNode(
        Scene* scene,
        const QString& opTitle = OpTitle,
        const std::vector<int>& inputs = {2, 2},
        const std::vector<int>& outputs = {1}
    );



    QJsonObject serialize() const override;
    QString nodeType() const override { return "CalculatorNode"; }
    

};

REGISTER_NODE(CalculatorNode, "CalculatorNode");

#endif // CALCULATORNODE_H
