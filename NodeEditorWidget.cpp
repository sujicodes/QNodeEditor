#include "NodeEditorWidget.h"
#include "NodeEditorGraphicsView.h"
#include "Scene.h"
#include "Node.h"
#include "Edge.h"
#include <qapplication.h>
#include <qfileinfo.h>
#include <QMessageBox>

NodeEditorWidget::NodeEditorWidget(QWidget *parent)
    : QWidget(parent)
{
    setGeometry(200, 200, 800, 600);
    setWindowTitle("Node Editor");

    layout = new QVBoxLayout(this);
    layout->setContentsMargins(0, 0, 0, 0);
    scene = new Scene();                  // Scene holds the logic
    // Create graphics view
    view = new NodeEditorGraphicsView(scene->graphicsScene(), this);
    layout->addWidget(view);

    layout->addWidget(view);
}

bool NodeEditorWidget::isModified() const
{
    return scene && scene->hasBeenModified();
    
}

bool NodeEditorWidget::isFilenameSet() const
{
    return !filename.isEmpty();
}

QString NodeEditorWidget::getFilename() const{
    return filename;
}

QString NodeEditorWidget::getUserFriendlyFilename() const
{
    QString name;

    if (isFilenameSet()) {
        QFileInfo fi(filename);
        name = fi.fileName();
    } else {
        name = "New Graph";
    }

    if (isModified())
        name += "*";

    return name;
}

void NodeEditorWidget::fileNew()
{
    if (scene)
        scene->clearScene();
        scene->getHistory()->clear();

    filename.clear();
}

bool NodeEditorWidget::fileLoad(const QString& name)
{
    QApplication::setOverrideCursor(Qt::WaitCursor);
    QString errorMsg;
    bool ok = scene && scene->loadFromFile(name, &errorMsg);

    QApplication::restoreOverrideCursor();

    if (!ok) {
        QMessageBox::warning(
            this,
            tr("Error loading %1").arg(QFileInfo(filename).fileName()),
            errorMsg.isEmpty() ? tr("Unknown error") : errorMsg
            );
        return false;
    }
    scene->getHistory()->clear();
    filename = name;
    return true;
}

bool NodeEditorWidget::fileSave(const QString& name)
{
    // When a non-empty filename is passed, update stored filename
    if (!name.isEmpty()) {
        this->filename = name;
    }

    QApplication::setOverrideCursor(Qt::WaitCursor);
    scene->saveToFile(this->filename);
    QApplication::restoreOverrideCursor();

    return true;
}

QList<QGraphicsItem*> NodeEditorWidget::getSelectedItems() const
{
    return scene->getSelectedItems();
}

bool NodeEditorWidget::hasSelectedItems() const
{
    return !scene->getSelectedItems().isEmpty();
}

bool NodeEditorWidget::canUndo() const
{
    return scene->getHistory()->canUndo();
}

bool NodeEditorWidget::canRedo() const
{
    return scene->getHistory()->canRedo();
}
