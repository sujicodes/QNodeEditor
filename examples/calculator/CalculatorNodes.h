#ifndef CalculatorNodes_H
#define CalculatorNodes_H

#include "CalculatorNodeBase.h"
#include "NodeGraphicsItem.h"
#include <qlabel.h>
#include <qlineedit.h>
#include <qwidget.h>

class AddNode final : public CalculatorNodeBase {
public:
    static inline const QString Icon = "icons/add.png";
    static inline const QString OpTitle = "Add";
    static inline const QString ContentLabel = "+";

    explicit AddNode(Scene* scene)
        : CalculatorNodeBase(scene, OpTitle) {
         initGraphics();
    }
        
    QString getContentLabel() const override { return ContentLabel; }
    QString nodeType() const override { return "AddNode"; }
};

// ----------------------------------
// Subtract
// ----------------------------------
class SubtractNode final : public CalculatorNodeBase {
public:
    static inline const QString Icon = "icons/sub.png";
    static inline const QString OpTitle = "Subtract";
    static inline const QString ContentLabel = "-";

    explicit SubtractNode(Scene* scene)
        : CalculatorNodeBase(scene, OpTitle)
    {
        initGraphics();
    }

    QString getContentLabel() const override { return ContentLabel; }
    QString nodeType() const override { return "SubtractNode"; }
};

// ----------------------------------
// Multiply
// ----------------------------------
class MultiplyNode final : public CalculatorNodeBase {
public:
    static inline const QString Icon = "icons/mul.png";
    static inline const QString OpTitle = "Multiply";
    static inline const QString ContentLabel = "*";

    explicit MultiplyNode(Scene* scene)
        : CalculatorNodeBase(scene, OpTitle)
    {
        initGraphics();
    }

    QString getContentLabel() const override { return ContentLabel; }
    QString nodeType() const override { return "MultiplyNode"; }
};

// ----------------------------------
// Divide
// ----------------------------------
class DivideNode final : public CalculatorNodeBase {
public:
    static inline const QString Icon = "icons/divide.png";
    static inline const QString OpTitle = "Divide";
    static inline const QString ContentLabel = "/";

    explicit DivideNode(Scene* scene)
        : CalculatorNodeBase(scene, OpTitle)
    {
        initGraphics();
    }

    QString getContentLabel() const override { return ContentLabel; }
    QString nodeType() const override { return "OutputNode"; }
};

// ----------------------------------
// Input
// ----------------------------------
class InputNode final : public CalculatorNodeBase {
public:
    static inline const QString Icon = "icons/in.png";
    static inline const QString OpTitle = "Input";

    explicit InputNode(Scene* scene)
        : CalculatorNodeBase(scene, OpTitle, {}, {3})
    {
        initGraphics();
    }

    QString getContentLabel() const override { return ""; }
    QString nodeType() const override { return "InputNode"; }
    void setValue(const QString& val) {m_value = val;}
    const QVariant& getValue() {return m_value;}

    QVariant evalImplementation() override
    {

        bool ok = false;
        int s_value = getValue().toInt(&ok);

        if (!ok)
            throw std::invalid_argument("Invalid integer input");

        value = s_value;
        markDirty(false);
        markInvalid(false);

        markDescendantsInvalid(false);
        markDescendantsDirty();

        if (getNodeGraphicsItem())
             getNodeGraphicsItem()->setToolTip("");

        evalChildren();

        return value;
    }


    QJsonObject serialize() const override {
        QJsonObject obj = CalculatorNodeBase::serialize();
        obj["value"] = value.toJsonObject();
        return obj;
    }

    void deserialize(const QJsonObject& data,
                     std::unordered_map<qint64, Serializable*>& hashmap,
                     bool restoreId) override {
        CalculatorNodeBase::deserialize(data, hashmap, restoreId);
        value = data["value"].toString();
        dynamic_cast<QLineEdit*>(getNodeGraphicsItem()->itemWidget)->setText(value.toString());

    }

private:
    QVariant value;
};

// ----------------------------------
// Output
// ----------------------------------
class OutputNode final : public CalculatorNodeBase {
public:
    static inline const QString Icon = "icons/out.png";
    static inline const QString OpTitle = "Output";

    explicit OutputNode(Scene* scene)
        : CalculatorNodeBase(scene, OpTitle, {1}, {})
    {
        initGraphics();
    }

    QString getContentLabel() const override { return ""; }
    QString nodeType() const override { return "OutputNode"; }
    QVariant evalImplementation() override
    {
        Node* inputNode = getInput(0);

        if (!inputNode)
        {
            if (getNodeGraphicsItem())
                getNodeGraphicsItem()->setToolTip("Input is not connected");

            markInvalid(true);
            return QVariant();
        }

        QVariant val = inputNode->eval();

        if (!val.isValid())
        {
            if (getNodeGraphicsItem())
                getNodeGraphicsItem()->setToolTip("Input is NaN");

            markInvalid(true);
            return QVariant();
        }

        dynamic_cast<QLabel*>(getNodeGraphicsItem()->itemWidget)->setText(val.toString());

        markInvalid(false);
        markDirty(false);

        if (getNodeGraphicsItem())
            getNodeGraphicsItem()->setToolTip("");

        return val;
    }
};

#endif // CalculatorNodes_H
