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
    markDirty();
}


QJsonObject CalculatorNodeBase::serialize() const {
    QJsonObject obj = Node::serialize();
    return obj;
}


void CalculatorNodeBase::initGraphics()
{
    setNodeGraphicsItem(new CalculatorGraphicsNode(this));
}

QVariant CalculatorNodeBase::eval()
{
    if (!isDirty() && !isInvalid()) {
         // returning cached value
        return m_value;
    }

    try {
        m_value = evalImplementation();
        markDirty(false);
        markInvalid(false);
        return m_value;
    }
    catch (const std::invalid_argument& e)     // equivalent to ValueError
    {
        markInvalid(true);

        if (getNodeGraphicsItem())
            getNodeGraphicsItem()->setToolTip(QString::fromStdString(e.what()));

        markDescendantsDirty();

        return QVariant();
    }
    catch (const std::exception& e)            // equivalent to Exception
    {
        markInvalid(true);

        if (getNodeGraphicsItem())
            getNodeGraphicsItem()->setToolTip(QString::fromStdString(e.what()));

        qDebug() << "Evaluation exception:" << e.what();

        return QVariant();
    }
}


QVariant CalculatorNodeBase::evalImplementation()
{
    Node* i1 = getInput(0);
    Node* i2 = getInput(1);

    if (!i1 || !i2)
    {
        markInvalid(true);
        markDescendantsDirty(true);

        if (getNodeGraphicsItem())
            getNodeGraphicsItem()->setToolTip("Connect all inputs");

        return QVariant();   // None
    }

    QVariant v1 = i1->eval();
    QVariant v2 = i2->eval();

    m_value = evalOperation(v1, v2);

    markDirty(false);
    markInvalid(false);

    if (getNodeGraphicsItem())
         getNodeGraphicsItem()->setToolTip("");

    markDescendantsDirty(true);
    evalChildren();

    return m_value;
}
