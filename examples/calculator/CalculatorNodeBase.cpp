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
        qDebug() << "_> returning cached"
                 << nodeType()
                 << "value:" << m_value;
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
