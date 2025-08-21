#include "NodeEditorWindow.h"

#include <QApplication>

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    NodeEditorWindow w;
    w.show();
    return a.exec();
}
