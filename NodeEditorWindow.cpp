#include "NodeEditorWindow.h"
#include "NodeEditorGraphicsView.h"
#include "Scene.h"
#include "Node.h"
#include "Theme.h"
#include "Edge.h"

NodeEditorWindow::NodeEditorWindow(QWidget *parent)
    : QWidget(parent)
{
    setGeometry(200, 200, 800, 600);
    setWindowTitle("Node Editor");

    layout = new QVBoxLayout(this);
    layout->setContentsMargins(0, 0, 0, 0);
    scene = new Scene(this);                  // Scene holds the logic
    //graphicsScene = scene->graphicsScene();         // Access the QGraphicsScene
    Node* node1 = new Node(scene, "My Awesome Node 1", {1, 2, 3}, {1});
    Node* node2 = new Node(scene, "My Awesome Node 2", {1, 2, 3}, {1});
    Node* node3 = new Node(scene, "My Awesome Node 3", {1, 2, 3}, {1});

    // Set positions
    node1->setPos(-350, -250);
    node2->setPos(-75, 0);
    node3->setPos(200, -150);

    // Create edges between sockets
    Edge* edge1 = new Edge(scene, node1->outputs[0], node2->inputs[0]);
    Edge* edge2 = new Edge(scene, node2->outputs[0], node3->inputs[0], Edge::EDGE_TYPE_BEZIER);
    // Create graphics view
    view = new NodeEditorGraphicsView(scene->graphicsScene(), this);
    layout->addWidget(view);

    layout->addWidget(view);
}
