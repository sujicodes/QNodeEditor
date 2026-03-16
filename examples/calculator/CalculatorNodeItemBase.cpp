
#include <QLabel>
#include <QLineEdit>
#include <QPainter>
#include <QDebug>

#include "CalculatorNodeItemBase.h"
#include "SocketItem.h"
#include "CalculatorNodes.h"

#include <QLabel>
#include <QLineEdit>
#include <QPainter>
#include <QDebug>

CalculatorNodeItemBase::CalculatorNodeItemBase(
    NodeEditorGraphicsScene* scene,
    const QString& opTitle,
    const int& inputs,
    const int& outputs
)
    : NodeItem(scene, opTitle, inputs, outputs)
{
    setWidth(160);
    setHeight(74);
    setEdgeRoundness(0);
    setEdgePadding(10);
    setTitleHorizontalPadding(4);
    setTitleVerticalPadding(4);

    setInputSocketPosition(SocketItem::LEFT_CENTER);
    setOutputSocketPosition(SocketItem::RIGHT_CENTER);

    markDirty();
}

QJsonObject CalculatorNodeItemBase::serialize() const
{
    QJsonObject obj = NodeItem::serialize();
    return obj;
}

QWidget* CalculatorNodeItemBase::setItemWidget() const
{
    if(nodeType() == "OutputNode")
    {
        QLabel* lbl = new QLabel("42");
        lbl->setAlignment(Qt::AlignLeft);
        return lbl;
    }
    else if(nodeType() == "InputNode")
    {
        const InputNode* inputNode = dynamic_cast<const InputNode*>(this);

        if(inputNode)
        {
            QLineEdit* lbl = new QLineEdit(QString::number(inputNode->getValue()));
            lbl->setAlignment(Qt::AlignLeft);

            QObject::connect(
                lbl,
                &QLineEdit::textEdited,
                [inputNode](const QString& text)
                {
                    const_cast<InputNode*>(inputNode)->markDirty(true);
                    const_cast<InputNode*>(inputNode)->markDescendantsDirty(true);
                    const_cast<InputNode*>(inputNode)->setValue(text.toInt());
                    const_cast<InputNode*>(inputNode)->eval();
                }
            );

            return lbl;
        }
    }

    if(!getContentLabel().isEmpty())
    {
        return new QLabel(getContentLabel());
    }

    return new QWidget();
}

QVariant CalculatorNodeItemBase::eval()
{
    if (!isDirty() && !isInvalid())
        return m_value;

    try
    {
        m_value = evalImplementation();
        markDirty(false);
        markInvalid(false);
        return m_value;
    }
    catch (const std::invalid_argument& e)
    {
        markInvalid(true);
        setToolTip(QString::fromStdString(e.what()));
        markDescendantsDirty();
        return QVariant();
    }
    catch (const std::exception& e)
    {
        markInvalid(true);
        setToolTip(QString::fromStdString(e.what()));
        qDebug() << "Evaluation exception:" << e.what();
        return QVariant();
    }
}

float CalculatorNodeItemBase::evalImplementation()
{
    NodeItem* i1 = getInput(0);
    NodeItem* i2 = getInput(1);

    if (!i1 || !i2)
    {
        markInvalid(true);
        markDescendantsDirty(true);
        setToolTip("Connect all inputs");
        return float();
    }

    float v1 = i1->eval().toFloat();
    float v2 = i2->eval().toFloat();

    m_value = evalOperation(v1, v2);

    markDirty(false);
    markInvalid(false);

    setToolTip("");

    markDescendantsDirty(true);
    evalChildren();

    return m_value;
}

void CalculatorNodeItemBase::paint(QPainter* painter,
                               const QStyleOptionGraphicsItem* option,
                               QWidget* widget)
{
    NodeItem::paint(painter, option, widget);

    qreal offset = 24.0;

    if(isDirty())
        offset = 0.0;
    else if(isInvalid())
        offset = 48.0;

    painter->drawImage(
        QRectF(-10.0, -10.0, 24.0, 24.0),
        m_icons,
        QRectF(offset, 0.0, 24.0, 24.0)
    );
}
