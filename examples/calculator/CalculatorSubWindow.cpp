#include "CalculatorSubWindow.h"
#include "../../Scene.h"
#include "Node.h"
#include "NodeEditorGraphicsView.h"
#include "examples/calculator/CalculatorConfig.h"
#include <QDebug>
#include <QEvent>
#include <QMimeData>
#include <QDragEnterEvent>
#include <QDropEvent>
#include <QDataStream>
#include <QPixmap>
#include <QDebug>
#include <qgraphicsview.h>

CalculatorSubWindow::CalculatorSubWindow(QWidget *parent)
    : NodeEditorWidget(parent)
{
    // Delete this widget automatically when the window is closed
    setAttribute(Qt::WA_DeleteOnClose);

    // Set initial window title
    setTitle();

    // Connect scene modification signal to update title dynamically
    // (Assuming your NodeEditorScene has a "hasBeenModified" or similar signal)
    if (getScene()) {
        getScene()->addHasBeenModifiedListener(
            [this]() {
                this->setTitle();
            }
        );
    }

    connect(getGraphicsView(), &NodeEditorGraphicsView::nodeDragEntered,
            this, &CalculatorSubWindow::onDragEnter);

    connect(getGraphicsView(), &NodeEditorGraphicsView::nodeDropped,
            this, &CalculatorSubWindow::onDrop);
}

void CalculatorSubWindow::setTitle()
{
    // Assuming NodeEditorWidget provides getUserFriendlyFilename()
    setWindowTitle(getUserFriendlyFilename());
}


void CalculatorSubWindow::closeEvent(QCloseEvent* event)
{
    // Let the main window decide
    qDebug() << " singallll@";
    emit closeRequested(this, event);
}

void CalculatorSubWindow::onDragEnter(QDragEnterEvent* event)
{
    if (event->mimeData()->hasFormat(LISTBOX_MIMETYPE)) {
        event->acceptProposedAction();
    } else {
        event->setAccepted(false);
    }
}

void CalculatorSubWindow::onDrop(QDropEvent* event)
{
    if (!event->mimeData()->hasFormat(LISTBOX_MIMETYPE)) {
        event->ignore();
        return;
    }

    QByteArray eventData = event->mimeData()->data(LISTBOX_MIMETYPE);
    QDataStream dataStream(&eventData, QIODevice::ReadOnly);

    QPixmap pixmap;
    qint32 opCode;
    QString text;

    dataStream >> opCode;        // Read operation code
    dataStream >> text;
    dataStream >> pixmap;     // Read display text

    // Map the mouse position in the widget to the scene coordinates
    QPointF scenePos = getScene()->graphicsScene()->views().first()->mapToScene(event->pos());

    qDebug() << "GOT DROP: [" << opCode << "] '" << text << "'"
             << "mouse:" << event->pos()
             << "scene:" << scenePos;

    // TODO: Update node constructor / inputs & outputs as needed
    Node* node = new Node(getScene(), text, {1,1}, {2});
    node->setPos(scenePos.x(), scenePos.y());
    getScene()->addNode(node);

    event->setDropAction(Qt::MoveAction);
    event->accept();
}
