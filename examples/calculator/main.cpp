#include <QApplication>
#include "CalculatorWindow.h"

int main(int argc, char *argv[])
{
    Q_INIT_RESOURCE(resources);

    QApplication app(argc, argv);

    CalculatorWindow wnd;
    wnd.show();

    return app.exec();
}