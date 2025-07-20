#include "mainwindow.h"

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
{
    nodeEditor = new NodeEditorWindow(this);
    setCentralWidget(nodeEditor);
    setWindowTitle("MainWindow with Node Editor");
}

MainWindow::~MainWindow(){}
