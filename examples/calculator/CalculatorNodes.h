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

        explicit AddNode(NodeEditorGraphicsScene* scene)
            : CalculatorNodeItemBase(scene, OpTitle)
        {
        }

        QString getContentLabel() const override { return ContentLabel; }
        QString nodeType() const override { return "AddNode"; }

    protected:

        float evalOperation(const float& v1, const float& v2) override
        {
            return v1 + v2;
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

        explicit SubtractNode(NodeEditorGraphicsScene* scene)
            : CalculatorNodeItemBase(scene, OpTitle)
        {
           
        }

        QString getContentLabel() const override { return ContentLabel; }
        QString nodeType() const override { return "SubtractNode"; }

    protected:

        float evalOperation(const float& v1, const float& v2) override
        {
            return v1 - v2;
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

        explicit MultiplyNode(NodeEditorGraphicsScene* scene)
            : CalculatorNodeItemBase(scene, OpTitle)
        {
           
        }

        QString getContentLabel() const override { return ContentLabel; }
        QString nodeType() const override { return "MultiplyNode"; }

    protected:

        float evalOperation(const float& v1, const float& v2) override
        {
            return v1 * v2;
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

        explicit DivideNode(NodeEditorGraphicsScene* scene)
            : CalculatorNodeItemBase(scene, OpTitle)
        {
           
        }

        QString getContentLabel() const override { return ContentLabel; }
        QString nodeType() const override { return "DivideNode"; }

    protected:

        float evalOperation(const float& v1, const float& v2) override
        {
            int b = v2;
            if (b == 0) return float();
            return v1 / b;
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

        explicit InputNode(NodeEditorGraphicsScene* scene)
            : CalculatorNodeItemBase(scene, OpTitle, 0, 1)
        {
           
        }

        QString getContentLabel() const override { return ""; }
        QString nodeType() const override { return "InputNode"; }
        void setValue(const float& val) {m_value = val;}
        int getValue() const {
            bool ok = false;
            int s_value =  m_value;
            return s_value;

        }

        float evalImplementation() override
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

        explicit OutputNode(NodeEditorGraphicsScene* scene)
            : CalculatorNodeItemBase(scene, OpTitle, 1, 0)
        {
           
        }

        QString getContentLabel() const override { return ""; }
        QString nodeType() const override { return "OutputNode"; }

        float evalImplementation() override
        {
            NodeItem* inputNode = getInput(0);

            if(!inputNode)
            {
                this->setToolTip("Input is not connected");

                markInvalid(true);
                return float();
            }

            QVariant val = inputNode->eval();

            if(!val.isValid())
            {
                this->setToolTip("Input is NaN");

                markInvalid(true);
                return float();
            }

            dynamic_cast<QLabel*>(this->getItemWidget())->setText(val.toString());

            markInvalid(false);
            markDirty(false);

            this->setToolTip("");

            return val.toFloat();
        }
};

REGISTER_NODE(OutputNode, "OutputNode");

#endif
