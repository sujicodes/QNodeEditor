#ifndef CALCULATORSUBWINDOW_H
#define CALCULATORSUBWINDOW_H
#include "../../NodeEditorWidget.h"
#include <QObject>

class CalculatorSubWindow : public NodeEditorWidget
{
    Q_OBJECT

public:
    explicit CalculatorSubWindow(QWidget *parent = nullptr);

public slots:
    void setTitle();
};

#endif // CALCULATORSUBWINDOW_H
