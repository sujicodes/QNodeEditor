#ifndef CalculatorNodes_H
#define CalculatorNodes_H

#include "CalculatorNodeBase.h"
#include "NodeGraphicsItem.h"
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
    void setValue(const QString& val) {value = val;}
    const QString& getValue() {return value;}

    QJsonObject serialize() const override {
        QJsonObject obj = CalculatorNodeBase::serialize();
        obj["value"] = value;
        return obj;
    }

    void deserialize(const QJsonObject& data,
                     std::unordered_map<qint64, Serializable*>& hashmap,
                     bool restoreId) override {
        CalculatorNodeBase::deserialize(data, hashmap, restoreId);
        value = data["value"].toString();
        dynamic_cast<QLineEdit*>(getNodeGraphicsItem()->itemWidget)->setText(value);

    }

private:
    QString value;
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
};

#endif // CalculatorNodes_H
