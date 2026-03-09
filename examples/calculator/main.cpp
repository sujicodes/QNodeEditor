#include <QApplication>
#include "CalculatorWindow.h"

int main(int argc, char *argv[])
{
    QApplication app(argc, argv);

    CalculatorWindow wnd;
    wnd.show();

    return app.exec();
}
