#include "NodeEditorWindow.h"
#include "NodeEditorWidget.h"
#include "NodeEditorGraphicsView.h"
#include "NodeEditorGraphicsScene.h"

#include <QMenuBar>
#include <QFileDialog>
#include <QStatusBar>
#include <QDebug>
#include <QFile>
#include <QGraphicsView>
#include <QApplication>
#include <QClipboard>
#include <QJsonDocument>
#include <QJsonObject>
#include <QMessageBox>
#include <QFileInfo>
#include <QCloseEvent>

NodeEditorWindow::NodeEditorWindow(QWidget *parent)
    : QMainWindow(parent),
      m_statusMousePos(nullptr)
{
    initUI();
}

QAction* NodeEditorWindow::createAct(const QString &name,
                                     const QString &shortcut,
                                     const QString &tooltip,
                                     const QObject* receiver,
                                     const char* member)
{
    QAction *act = new QAction(name, this);
    act->setShortcut(QKeySequence(shortcut));
    act->setToolTip(tooltip);
    connect(act, SIGNAL(triggered()), receiver, member);
    return act;
}

void NodeEditorWindow::initUI()
{
    createActions();
    createMenus();
    // Node editor widget
    m_nodeEditorWidget = new NodeEditorWidget(this);

    m_nodeEditorWidget->getScene()->addHasBeenModifiedListener([this]()
    {
        this->setTitle();
    });

    setCentralWidget(m_nodeEditorWidget);

    // Status bar
    statusBar()->showMessage(QString());
    m_statusMousePos = new QLabel(QString());
    statusBar()->addPermanentWidget(m_statusMousePos);

    connect(m_nodeEditorWidget->getGraphicsView(), &NodeEditorGraphicsView::scenePosChanged, this, &NodeEditorWindow::onScenePosChanged);
}

void NodeEditorWindow::createActions()
{
    m_actNew = new QAction("New", this);
    m_actNew->setShortcut(QKeySequence::New);
    m_actNew->setStatusTip("Create new graph");
    connect(m_actNew, &QAction::triggered, this, &NodeEditorWindow::onFileNew);

    m_actOpen = new QAction("Open", this);
    m_actOpen->setShortcut(QKeySequence::Open);
    m_actOpen->setStatusTip("Open file");
    connect(m_actOpen, &QAction::triggered, this, &NodeEditorWindow::onFileOpen);

    m_actSave = new QAction("Save", this);
    m_actSave->setShortcut(QKeySequence::Save);
    m_actSave->setStatusTip("Save file");
    connect(m_actSave, &QAction::triggered, this, &NodeEditorWindow::onFileSave);

    m_actSaveAs = new QAction("Save As...", this);
    m_actSaveAs->setShortcut(QKeySequence("Ctrl+Shift+S"));
    m_actSaveAs->setStatusTip("Save file as...");
    connect(m_actSaveAs, &QAction::triggered, this, &NodeEditorWindow::onFileSaveAs);

    m_actExit = new QAction("Exit", this);
    m_actExit->setShortcut(QKeySequence::Quit);
    m_actExit->setStatusTip("Exit application");
    connect(m_actExit, &QAction::triggered, this, &NodeEditorWindow::close);

    m_actUndo = new QAction("&Undo", this);
    m_actUndo->setShortcut(QKeySequence::Undo);
    m_actUndo->setStatusTip("Undo last operation");
    connect(m_actUndo, &QAction::triggered, this, &NodeEditorWindow::onEditUndo);

    m_actRedo = new QAction("Redo", this);
    m_actRedo->setShortcut(QKeySequence("Ctrl+Shift+Z"));
    m_actRedo->setStatusTip("Redo last operation");
    connect(m_actRedo, &QAction::triggered, this, &NodeEditorWindow::onEditRedo);

    m_actCut = new QAction("Cut", this);
    m_actCut->setShortcut(QKeySequence::Cut);
    m_actCut->setStatusTip("Cut to clipboard");
    connect(m_actCut, &QAction::triggered, this, &NodeEditorWindow::onEditCut);

    m_actCopy = new QAction("Copy", this);
    m_actCopy->setShortcut(QKeySequence::Copy);
    m_actCopy->setStatusTip("Copy to clipboard");
    connect(m_actCopy, &QAction::triggered, this, &NodeEditorWindow::onEditCopy);

    m_actPaste = new QAction("Paste", this);
    m_actPaste->setShortcut(QKeySequence::Paste);
    m_actPaste->setStatusTip("Paste from clipboard");
    connect(m_actPaste, &QAction::triggered, this, &NodeEditorWindow::onEditPaste);

    m_actDelete = new QAction("Delete", this);
    m_actDelete->setShortcut(QKeySequence::Delete);
    m_actDelete->setStatusTip("Delete selected items");
    connect(m_actDelete, &QAction::triggered, this, &NodeEditorWindow::onEditDelete);
}

void NodeEditorWindow::createMenus()
{
    QMenuBar *menubar = menuBar();

    QMenu *fileMenu = menubar->addMenu("File");
    fileMenu->addAction(m_actNew);
    fileMenu->addSeparator();
    fileMenu->addAction(m_actOpen);
    fileMenu->addAction(m_actSave);
    fileMenu->addAction(m_actSaveAs);
    fileMenu->addSeparator();
    fileMenu->addAction(m_actExit);

    QMenu *editMenu = menubar->addMenu("Edit");
    editMenu->addAction(m_actUndo);
    editMenu->addAction(m_actRedo);
    editMenu->addSeparator();
    editMenu->addAction(m_actCut);
    editMenu->addAction(m_actCopy);
    editMenu->addAction(m_actPaste);
    editMenu->addSeparator();
    editMenu->addAction(m_actDelete);
}

void NodeEditorWindow::onScenePosChanged(int x, int y)
{
    m_statusMousePos->setText(QString("Scene Pos: [%1, %2]").arg(x).arg(y));
}

NodeEditorWidget* NodeEditorWindow::getCurrentNodeEditorWidget() const
{
    return dynamic_cast<NodeEditorWidget*>(centralWidget());
}

void NodeEditorWindow::onFileNew()
{
    if(maybeSave())
    {
        getCurrentNodeEditorWidget()->getScene()->clearScene();
        setTitle();
    }
}

void NodeEditorWindow::onFileOpen()
{
    if(maybeSave())
    {
        QString fname = QFileDialog::getOpenFileName(this, "Open graph from file");
        if(fname.isEmpty()) return;

        QFile file(fname);
        if(file.exists())
        {
            getCurrentNodeEditorWidget()->fileLoad(fname);
            setTitle();
        }
    }
}

bool NodeEditorWindow::onFileSave()
{
    if(getCurrentNodeEditorWidget()->getFilename().isEmpty())
    {
        return onFileSaveAs();
    }

    getCurrentNodeEditorWidget()->fileSave();
    statusBar()->showMessage(QString("Successfully saved %1🧩").arg(getCurrentNodeEditorWidget()->getFilename()));
    emit onFileSaved(getCurrentNodeEditorWidget()->getFilename());
    setTitle();
    return true;
}

bool NodeEditorWindow::onFileSaveAs()
{
    QString fname = QFileDialog::getSaveFileName(this, "Save graph to file");
    if(fname.isEmpty()) return false;
    getCurrentNodeEditorWidget()->fileSave(fname);
    statusBar()->showMessage(QString("Successfully saved %1🧩").arg(getCurrentNodeEditorWidget()->getFilename()));
    emit onFileSaved(getCurrentNodeEditorWidget()->getFilename());
    return true;
}

void NodeEditorWindow::onEditUndo()
{
    NodeEditorWidget* editor = getCurrentNodeEditorWidget();
    if(editor) editor->getScene()->getHistory()->undo();
}

void NodeEditorWindow::onEditRedo()
{
    NodeEditorWidget* editor = getCurrentNodeEditorWidget();
    if(editor) editor->getScene()->getHistory()->redo();
}

void NodeEditorWindow::onEditDelete()
{
    NodeEditorWidget* editor = getCurrentNodeEditorWidget();
    if(editor)
    {
        NodeEditorGraphicsView* view = editor->getGraphicsView();
        if(view) view->deleteSelected();
    }
}

void NodeEditorWindow::onEditCut()
{
    NodeEditorWidget* editor = getCurrentNodeEditorWidget();
    if(editor)
    {
        QJsonObject data = editor->getScene()->serializeSelected(true);
        QJsonDocument doc(data);
        QString strData = doc.toJson(QJsonDocument::Indented);
        qWarning() << "copying: "<< strData;

        QApplication::clipboard()->setText(strData);
    }
}

void NodeEditorWindow::onEditCopy()
{
    NodeEditorWidget* editor = getCurrentNodeEditorWidget();
    if(editor)
    {
        QJsonObject data = editor->getScene()->serializeSelected(false);
        QJsonDocument doc(data);
        QString strData = doc.toJson(QJsonDocument::Indented);

        QApplication::clipboard()->setText(strData);
    }
}

void NodeEditorWindow::onEditPaste()
{
    NodeEditorWidget* editor = getCurrentNodeEditorWidget();
    if(!editor) return;

    QString rawData = QApplication::clipboard()->text();

    QJsonParseError parseError;
    QJsonDocument doc = QJsonDocument::fromJson(rawData.toUtf8(), &parseError);

    if(parseError.error != QJsonParseError::NoError)
    {
        qWarning() << "Pasting invalid JSON data!" << parseError.errorString();
        return;
    }

    if(!doc.isObject())
    {
        qWarning() << "JSON root is not an object!";
        return;
    }

    QJsonObject data = doc.object();

    if(!data.contains("nodes"))
    {
        qWarning() << "JSON does not contain any nodes!";
        return;
    }

    editor->getScene()->deserializeFromClipboard(data);
}

void NodeEditorWindow::setTitle()
{
    QString title = "Node Editor - ";
    title += getCurrentNodeEditorWidget()->getUserFriendlyFilename();
    setWindowTitle(title);
}

void NodeEditorWindow::closeEvent(QCloseEvent* event)
{
    if(maybeSave())
    {
        event->accept();
    } 
    else
    {
        event->ignore();
    }
}

bool NodeEditorWindow::isModified() const
{
    return getCurrentNodeEditorWidget()->isModified();
}

bool NodeEditorWindow::maybeSave()
{
    if(!isModified()) return true;

    QMessageBox::StandardButton res = QMessageBox::warning(
        this,
        QString("Scene Modified"),
        QString("The scene has been modified.\nDo you want to save your changes ?"),
        QMessageBox::Save | QMessageBox::Discard | QMessageBox::Cancel
        );

    if(res == QMessageBox::Save)
    {
        return onFileSave();
    }
    else if(res == QMessageBox::Cancel)
    {
        return false;
    }

    return true;
}