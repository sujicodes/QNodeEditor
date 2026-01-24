#ifndef CalculatorNodes_H
#define CalculatorNodes_H

#include "CalculatorNodeBase.h"

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
};

#endif // CalculatorNodes_H
