#include "NodeEditorWindow.h"
#include "NodeEditorGraphicsView.h"

NodeEditorWindow::NodeEditorWindow(QWidget *parent)
    : QWidget(parent)
{
    setGeometry(200, 200, 800, 600);
    setWindowTitle("Node Editor");

    layout = new QVBoxLayout(this);
    layout->setContentsMargins(0, 0, 0, 0);

    grScene = new NodeGraphicsScene(this);
    view = new NodeEditorGraphicsView(this->grScene, this);
    view->setScene(grScene);

    layout->addWidget(view);
}
