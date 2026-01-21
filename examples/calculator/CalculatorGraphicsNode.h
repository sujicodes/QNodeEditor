#ifndef CALCULATORGRAPHICSNODE_H
#define CALCULATORGRAPHICSNODE_H

#include "NodeGraphicsItem.h"
#include "CalculatorNode.h"
#include <QJsonObject>
#include <QJsonValue>
#include <QLabel>

class CalculatorNode;

class CalculatorGraphicsNode : public NodeGraphicsItem
{
public:
    explicit CalculatorGraphicsNode(Node* node);
    
    void setNodeContent(const QJsonValue &data) override{ return;}
    QJsonObject getNodeContent() const override {return QJsonObject();}
    CalculatorNode* getCalculatorNode() const;


protected:
    QWidget* setItemWidget() const override;
};
#endif // CALCULATORGRAPHICSNODE_H
