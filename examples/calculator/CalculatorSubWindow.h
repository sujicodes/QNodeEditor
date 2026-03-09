#ifndef CALCULATORSUBWINDOW_H
#define CALCULATORSUBWINDOW_H

#include "NodeEditorWidget.h"
#include <QObject>

class CalculatorSubWindow : public NodeEditorWidget
{
    Q_OBJECT

    signals:
        void closeRequested(NodeEditorWidget* widget, QCloseEvent* event);

    public:
        explicit CalculatorSubWindow(QWidget *parent = nullptr);
        void onDragEnter(QDragEnterEvent* event);
        void onDrop(QDropEvent* event);

    protected:
        void closeEvent(QCloseEvent* event) override;

    public slots:
        void setTitle();
};

#endif // CALCULATORSUBWINDOW_H