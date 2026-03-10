#ifndef CALCULATORNODEITEMBASE_H
#define CALCULATORNODEITEMBASE_H

#include "NodeItem.h"
#include <QVariant>
#include <QImage>

class CalculatorNodeItemBase : public NodeItem
{
public:
    CalculatorNodeItemBase(
        Scene* scene,
        const QString& opTitle,
        const QList<int>& inputs = {2, 2},
        const QList<int>& outputs = {1}
    );

    QWidget* setItemWidget() const override;

    QVariant eval() override;

    QJsonObject serialize() const override;

    virtual QString getContentLabel() const { return m_contentLabel; }
    virtual QString nodeType() const { return m_nodeType; }

    void setContentLabel(const QString& label) { m_contentLabel = label; }
    void setNodeType(const QString& type) { m_nodeType = type; }

protected:
    void paint(QPainter* painter,
               const QStyleOptionGraphicsItem* option,
               QWidget* widget = nullptr) override;
    float m_value;

    virtual float evalImplementation();
    virtual float evalOperation(const float& v1, const float& v2) { return 0; }

private:
    QString m_contentLabel;
    QString m_nodeType;

    QImage m_icons = QImage("C:\\Users\\sujan\\Downloads\\status_icons.png");
};

#endif
