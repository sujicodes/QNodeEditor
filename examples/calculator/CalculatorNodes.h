#ifndef CALCULATORNODES_H
#define CALCULATORNODES_H

#include "CalculatorNodeItemBase.h"
#include "NodeItem.h"
#include "NodeRegistry.h"
#include <qlabel.h>
#include <qlineedit.h>
#include <qwidget.h>

// ----------------------------------
// Add
// ----------------------------------
class AddNode : public CalculatorNodeItemBase
{
    public:

        static inline const QString Icon = "icons/plus.png";
        static inline const QString OpTitle = "Add";
        static inline const QString ContentLabel = "+";

        explicit AddNode(Scene* scene)
            : CalculatorNodeItemBase(scene, OpTitle)
        {
        }

        QString getContentLabel() const override { return ContentLabel; }
        QString nodeType() const override { return "AddNode"; }

    protected:

        QVariant evalOperation(const QVariant& v1, const QVariant& v2) override
        {
            return v1.toInt() + v2.toInt();
        }
};

REGISTER_NODE(AddNode, "AddNode");

// ----------------------------------
// Subtract
// ----------------------------------
class SubtractNode : public CalculatorNodeItemBase
{
    public:

        static inline const QString Icon = "icons/substract.png";
        static inline const QString OpTitle = "Subtract";
        static inline const QString ContentLabel = "-";

        explicit SubtractNode(Scene* scene)
            : CalculatorNodeItemBase(scene, OpTitle)
        {
           
        }

        QString getContentLabel() const override { return ContentLabel; }
        QString nodeType() const override { return "SubtractNode"; }

    protected:

        QVariant evalOperation(const QVariant& v1, const QVariant& v2) override
        {
            return v1.toInt() - v2.toInt();
        }
};

REGISTER_NODE(SubtractNode, "SubtractNode");

// ----------------------------------
// Multiply
// ----------------------------------
class MultiplyNode : public CalculatorNodeItemBase
{
    public:

        static inline const QString Icon = "icons/muliply.png";
        static inline const QString OpTitle = "Multiply";
        static inline const QString ContentLabel = "*";

        explicit MultiplyNode(Scene* scene)
            : CalculatorNodeItemBase(scene, OpTitle)
        {
           
        }

        QString getContentLabel() const override { return ContentLabel; }
        QString nodeType() const override { return "MultiplyNode"; }

    protected:

        QVariant evalOperation(const QVariant& v1, const QVariant& v2) override
        {
            return v1.toInt() * v2.toInt();
        }
};

REGISTER_NODE(MultiplyNode, "MultiplyNode");

// ----------------------------------
// Divide
// ----------------------------------
class DivideNode : public CalculatorNodeItemBase
{
    public:

        static inline const QString Icon = "icons/divide.png";
        static inline const QString OpTitle = "Divide";
        static inline const QString ContentLabel = "/";

        explicit DivideNode(Scene* scene)
            : CalculatorNodeItemBase(scene, OpTitle)
        {
           
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

REGISTER_NODE(DivideNode, "DivideNode");

// ----------------------------------
// Input
// ----------------------------------
class InputNode : public CalculatorNodeItemBase
{
    public:

        static inline const QString Icon = "icons/in.png";
        static inline const QString OpTitle = "Input";

        explicit InputNode(Scene* scene)
            : CalculatorNodeItemBase(scene, OpTitle, {}, {3})
        {
           
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

            this->setToolTip("");

            evalChildren();

            return s_value;
        }

        QJsonObject serialize() const override {
            QJsonObject obj = CalculatorNodeItemBase::serialize();
            bool ok = false;
            obj["value"] = getValue();
            return obj;
        }

        void deserialize(const QJsonObject& data,
                        std::unordered_map<qint64, Serializable*>& hashmap,
                        bool restoreId) override {
            CalculatorNodeItemBase::deserialize(data, hashmap, restoreId);
            setValue(data["value"].toInt());
            QLineEdit* field = dynamic_cast<QLineEdit*>(this->getItemWidget());
            if(field){
                field->setText(QString::number(getValue()));

            }
        }
};

REGISTER_NODE(InputNode, "InputNode");

// ----------------------------------
// Output
// ----------------------------------
class OutputNode : public CalculatorNodeItemBase
{
    public:

        static inline const QString Icon = "icons/out.png";
        static inline const QString OpTitle = "Output";

        explicit OutputNode(Scene* scene)
            : CalculatorNodeItemBase(scene, OpTitle, {1}, {})
        {
           
        }

        QString getContentLabel() const override { return ""; }
        QString nodeType() const override { return "OutputNode"; }

        QVariant evalImplementation() override
        {
            NodeItem* inputNode = getInput(0);

            if(!inputNode)
            {
                this->setToolTip("Input is not connected");

                markInvalid(true);
                return QVariant();
            }

            QVariant val = inputNode->eval();

            if(!val.isValid())
            {
                this->setToolTip("Input is NaN");

                markInvalid(true);
                return QVariant();
            }

            dynamic_cast<QLabel*>(this->getItemWidget())->setText(val.toString());

            markInvalid(false);
            markDirty(false);

            this->setToolTip("");

            return val;
        }
};

REGISTER_NODE(OutputNode, "OutputNode");

#endif
