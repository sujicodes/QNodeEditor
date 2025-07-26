#include "NodeEditorWindow.h"
#include "NodeEditorGraphicsView.h"
#include "Scene.h"

NodeEditorWindow::NodeEditorWindow(QWidget *parent)
    : QWidget(parent)
{
    setGeometry(200, 200, 800, 600);
    setWindowTitle("Node Editor");

    layout = new QVBoxLayout(this);
    layout->setContentsMargins(0, 0, 0, 0);
    scene = new Scene(this);                  // Scene holds the logic
    graphicsScene = scene->graphicsScene();         // Access the QGraphicsScene

    // Create graphics view
    view = new NodeEditorGraphicsView(graphicsScene, this);
    layout->addWidget(view);;

    layout->addWidget(view);
}
