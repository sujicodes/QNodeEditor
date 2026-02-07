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
        const QList<int>& inputs = {2, 2},
        const QList<int>& outputs = {1}
    );

    QVariant eval() override;
    virtual QVariant evalImplementation(){return 123;};


    QJsonObject serialize() const override;
    QString nodeType() const override { return "CalculatorNodeBase"; }
    virtual QString getContentLabel() const { return ""; }

    void initGraphics();

protected:
    QVariant m_value;

    

};

REGISTER_NODE(CalculatorNodeBase, "CalculatorNodeBase");

#endif // CalculatorNodeBase_H
