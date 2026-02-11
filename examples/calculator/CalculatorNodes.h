#ifndef CalculatorNodes_H
#define CalculatorNodes_H

#include "CalculatorNodeBase.h"
#include "NodeGraphicsItem.h"
#include <qlabel.h>
#include <qlineedit.h>
#include <qwidget.h>

// ----------------------------------
// Add
// ----------------------------------
class AddNode final : public CalculatorNodeBase {
public:
    static inline const QString Icon = "icons/add.png";
    static inline const QString OpTitle = "Add";
    static inline const QString ContentLabel = "+";

    explicit AddNode(Scene* scene)
        : CalculatorNodeBase(scene, OpTitle)
    {
        initGraphics();
    }

    QString getContentLabel() const override { return ContentLabel; }
    QString nodeType() const override { return "AddNode"; }

protected:
    QVariant evalOperation(const QVariant& v1, const QVariant& v2) override
    {
        return v1.toInt() + v2.toInt();
    }
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

protected:
    QVariant evalOperation(const QVariant& v1, const QVariant& v2) override
    {
        return v1.toInt() - v2.toInt();
    }
};

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

protected:
    QVariant evalOperation(const QVariant& v1, const QVariant& v2) override
    {
        return v1.toInt() * v2.toInt();
    }
};

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
    QString nodeType() const override { return "DivideNode"; }

protected:
    QVariant evalOperation(const QVariant& v1, const QVariant& v2) override
    {
        int b = v2.toInt();
        if (b == 0) return QVariant();
        return v1.toInt() / b;
    }
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
    void setValue(const int& val) {m_value = val;}
    int getValue() const {
        bool ok = false;
        int s_value =  m_value.toInt(&ok);
        if (!ok && m_value != "")
            throw std::invalid_argument("Invalid integer input");
        return s_value;

    }

    QVariant evalImplementation() override
    {

        bool ok = false;
        int s_value = getValue();

        markDirty(false);
        markInvalid(false);

        markDescendantsInvalid(false);
        markDescendantsDirty();

        if (getNodeGraphicsItem())
             getNodeGraphicsItem()->setToolTip("");

        evalChildren();

        return s_value;
    }


    QJsonObject serialize() const override {
        QJsonObject obj = CalculatorNodeBase::serialize();
        bool ok = false;
        obj["value"] = getValue();
        return obj;
    }

    void deserialize(const QJsonObject& data,
                     std::unordered_map<qint64, Serializable*>& hashmap,
                     bool restoreId) override {
        CalculatorNodeBase::deserialize(data, hashmap, restoreId);
        setValue(data["value"].toInt());
        QLineEdit* field = dynamic_cast<QLineEdit*>(getNodeGraphicsItem()->itemWidget);
        if(field){
            field->setText(QString::number(getValue()));

        }

    }

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
