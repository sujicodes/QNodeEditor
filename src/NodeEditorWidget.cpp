#include "NodeEditorWidget.h"
#include "NodeEditorGraphicsView.h"
#include "NodeEditorGraphicsScene.h"
#include "NodeItem.h"
#include "Edge.h"
#include <qapplication.h>
#include <qfileinfo.h>
#include <QMessageBox>

NodeEditorWidget::NodeEditorWidget(QWidget *parent)
    : QWidget(parent)
{
    setGeometry(200, 200, 800, 600);
    setWindowTitle("Node Editor");

    m_layout = new QVBoxLayout(this);
    m_layout->setContentsMargins(0, 0, 0, 0);
    m_scene = new NodeEditorGraphicsScene();                  // m_scene holds the logic
    // Create graphics view
    m_view = new NodeEditorGraphicsView(m_scene, this);
    m_layout->addWidget(m_view);
}

void NodeEditorWidget::setGraphicsView(NodeEditorGraphicsView* view)
{
    if (m_view) {
        m_layout->replaceWidget(m_view, view);
        delete m_view; // <-- delete the old view, don't leave it alive
    }
    m_view = view;
}

bool NodeEditorWidget::isModified() const
{
    return m_scene && m_scene->hasBeenModified();
}

bool NodeEditorWidget::isFilenameSet() const
{
    return !m_filename.isEmpty();
}

QString NodeEditorWidget::getFilename() const
{
    return m_filename;
}

QString NodeEditorWidget::getUserFriendlyFilename() const
{
    QString name;

    if(isFilenameSet())
    {
        QFileInfo fi(m_filename);
        name = fi.fileName();
    }
    else
    {
        name = "New Graph";
    }

    if (isModified()) name += "*";

    return name;
}

void NodeEditorWidget::fileNew()
{
    if(m_scene)
    {
        m_scene->clearScene();
        m_scene->getHistory()->clear();
    }

    m_filename.clear();
}

bool NodeEditorWidget::fileLoad(const QString& name)
{
    QApplication::setOverrideCursor(Qt::WaitCursor);
    QString errorMsg;
    bool ok = m_scene && m_scene->loadFromFile(name, &errorMsg);

    QApplication::restoreOverrideCursor();

    if(!ok)
    {
        QMessageBox::warning(
            this,
            QString("Error loading %1").arg(QFileInfo(m_filename).fileName()),
            errorMsg.isEmpty() ? "Unknown error" : errorMsg
            );
        return false;
    }

    m_scene->getHistory()->clear();
    m_filename = name;
    return true;
}

bool NodeEditorWidget::fileSave(const QString& name)
{
    // When a non-empty m_filename is passed, update stored m_filename
    if(!name.isEmpty())
    {
        this->m_filename = name;
    }

    QApplication::setOverrideCursor(Qt::WaitCursor);
    m_scene->saveToFile(this->m_filename);
    QApplication::restoreOverrideCursor();

    return true;
}

QList<QGraphicsItem*> NodeEditorWidget::getSelectedItems() const
{
    return m_scene->getSelectedItems();
}

bool NodeEditorWidget::hasSelectedItems() const
{
    return !m_scene->getSelectedItems().isEmpty();
}

bool NodeEditorWidget::canUndo() const
{
    return m_scene->getHistory()->canUndo();
}

bool NodeEditorWidget::canRedo() const
{
    return m_scene->getHistory()->canRedo();
}
