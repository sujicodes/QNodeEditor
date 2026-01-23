#ifndef CalculatorNodeBase_H
#define CalculatorNodeBase_H
#include "Node.h"
#include <QString>
#include <QVector>
#include "NodeRegistry.h"

class CalculatorNodeBase : public Node
{
public:
    static constexpr int OpCode = 0;
    static inline const QString OpTitle = "Undefined";
    static inline const QString contentLabel = "";
    static inline const QString contentLabelObjName = "calc_node_bg";


    CalculatorNodeBase(
        Scene* scene,
        const QString& opTitle = "",
        const std::vector<int>& inputs = {2, 2},
        const std::vector<int>& outputs = {1}
    );

    CalculatorNodeBase& operator=(const CalculatorNodeBase& other)
    {
        qDebug() << "Copy assignment called for" << typeid(*this).name();
        if (this != &other) {
            Node::operator=(other);
        }
        return *this;
    }

    QJsonObject serialize() const override;
    QString nodeType() const override { return "CalculatorNodeBase"; }
    virtual QString getContentLabel() const { return ""; }
    

};

REGISTER_NODE(CalculatorNodeBase, "CalculatorNodeBase");

#endif // CalculatorNodeBase_H
