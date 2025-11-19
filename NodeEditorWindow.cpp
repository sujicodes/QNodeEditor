#include "NodeEditorWindow.h"
#include "NodeEditorWidget.h"
#include "NodeEditorGraphicsView.h"
#include "Scene.h"
#include "history.h"
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
#include "NodeSelectorWidget.h"

NodeEditorWindow::NodeEditorWindow(QWidget *parent)
    : QMainWindow(parent),
      statusMousePos(nullptr)
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
    nodeEditorWidget = new NodeEditorWidget(this);
    nodeEditorWidget->getScene()->addHasBeenModifiedListener([this]() {
        this->setTitle();
    });
    setCentralWidget(nodeEditorWidget);

    // Status bar
    statusBar()->showMessage("");
    statusMousePos = new QLabel("");
    statusBar()->addPermanentWidget(statusMousePos);

    connect(nodeEditorWidget->getGraphicsView(), &NodeEditorGraphicsView::scenePosChanged, this, &NodeEditorWindow::onScenePosChanged);



    //connect(nodePopup, &NodePopup::nodeChosen, this, &NodeEditorWindow::onNodeChosen);


    // Window properties
    setGeometry(200, 200, 800, 600);
    setWindowTitle("Node Editor");
    setTitle();
    show();
}

void NodeEditorWindow::createActions()
{
    actNew = new QAction(tr("&New"), this);
    actNew->setShortcut(QKeySequence::New);
    actNew->setStatusTip(tr("Create new graph"));
    connect(actNew, &QAction::triggered, this, &NodeEditorWindow::onFileNew);

    actOpen = new QAction(tr("&Open"), this);
    actOpen->setShortcut(QKeySequence::Open);
    actOpen->setStatusTip(tr("Open file"));
    connect(actOpen, &QAction::triggered, this, &NodeEditorWindow::onFileOpen);

    actSave = new QAction(tr("&Save"), this);
    actSave->setShortcut(QKeySequence::Save);
    actSave->setStatusTip(tr("Save file"));
    connect(actSave, &QAction::triggered, this, &NodeEditorWindow::onFileSave);

    actSaveAs = new QAction(tr("Save &As..."), this);
    actSaveAs->setShortcut(QKeySequence("Ctrl+Shift+S"));
    actSaveAs->setStatusTip(tr("Save file as..."));
    connect(actSaveAs, &QAction::triggered, this, &NodeEditorWindow::onFileSaveAs);

    actExit = new QAction(tr("E&xit"), this);
    actExit->setShortcut(QKeySequence::Quit);
    actExit->setStatusTip(tr("Exit application"));
    connect(actExit, &QAction::triggered, this, &NodeEditorWindow::close);

    actUndo = new QAction(tr("&Undo"), this);
    actUndo->setShortcut(QKeySequence::Undo);
    actUndo->setStatusTip(tr("Undo last operation"));
    connect(actUndo, &QAction::triggered, this, &NodeEditorWindow::onEditUndo);

    actRedo = new QAction(tr("&Redo"), this);
    actRedo->setShortcut(QKeySequence("Ctrl+Shift+Z"));
    actRedo->setStatusTip(tr("Redo last operation"));
    connect(actRedo, &QAction::triggered, this, &NodeEditorWindow::onEditRedo);

    actCut = new QAction(tr("Cu&t"), this);
    actCut->setShortcut(QKeySequence::Cut);
    actCut->setStatusTip(tr("Cut to clipboard"));
    connect(actCut, &QAction::triggered, this, &NodeEditorWindow::onEditCut);

    actCopy = new QAction(tr("&Copy"), this);
    actCopy->setShortcut(QKeySequence::Copy);
    actCopy->setStatusTip(tr("Copy to clipboard"));
    connect(actCopy, &QAction::triggered, this, &NodeEditorWindow::onEditCopy);

    actPaste = new QAction(tr("&Paste"), this);
    actPaste->setShortcut(QKeySequence::Paste);
    actPaste->setStatusTip(tr("Paste from clipboard"));
    connect(actPaste, &QAction::triggered, this, &NodeEditorWindow::onEditPaste);

    actDelete = new QAction(tr("&Delete"), this);
    actDelete->setShortcut(QKeySequence::Delete);
    actDelete->setStatusTip(tr("Delete selected items"));
    connect(actDelete, &QAction::triggered, this, &NodeEditorWindow::onEditDelete);
}

void NodeEditorWindow::createMenus()
{
    QMenuBar *menubar = menuBar();

    QMenu *fileMenu = menubar->addMenu(tr("&File"));
    fileMenu->addAction(actNew);
    fileMenu->addSeparator();
    fileMenu->addAction(actOpen);
    fileMenu->addAction(actSave);
    fileMenu->addAction(actSaveAs);
    fileMenu->addSeparator();
    fileMenu->addAction(actExit);

    QMenu *editMenu = menubar->addMenu(tr("&Edit"));
    editMenu->addAction(actUndo);
    editMenu->addAction(actRedo);
    editMenu->addSeparator();
    editMenu->addAction(actCut);
    editMenu->addAction(actCopy);
    editMenu->addAction(actPaste);
    editMenu->addSeparator();
    editMenu->addAction(actDelete);
}

void NodeEditorWindow::onScenePosChanged(int x, int y)
{
    statusMousePos->setText(QString("Scene Pos: [%1, %2]").arg(x).arg(y));
}

NodeEditorWidget* NodeEditorWindow::getCurrentNodeEditorWidget() const{
    return dynamic_cast<NodeEditorWidget*>(centralWidget());
}

void NodeEditorWindow::onFileNew()
{
    if (maybeSave()){
        getCurrentNodeEditorWidget()->getScene()->clearScene();
        setTitle();
    }
}

void NodeEditorWindow::onFileOpen()
{
    if (maybeSave()){
        QString fname = QFileDialog::getOpenFileName(this, "Open graph from file");
        if (fname.isEmpty()) return;

        QFile file(fname);
        if (file.exists()) {
            getCurrentNodeEditorWidget()->fileLoad(fname);
            setTitle();
        }
    }
}

bool NodeEditorWindow::onFileSave()
{
    if (getCurrentNodeEditorWidget()->getFilename().isEmpty()) {
        return onFileSaveAs();
    }
    getCurrentNodeEditorWidget()->fileSave();
    statusBar()->showMessage(QString("Successfully saved %1🧩").arg(getCurrentNodeEditorWidget()->getFilename()));
    setTitle();
    return true;
}

bool NodeEditorWindow::onFileSaveAs()
{
    QString fname = QFileDialog::getSaveFileName(this, "Save graph to file");
    if (fname.isEmpty()) return false;
    getCurrentNodeEditorWidget()->fileSave(fname);
    statusBar()->showMessage(QString("Successfully saved %1🧩").arg(getCurrentNodeEditorWidget()->getFilename()));
    return true;
}

void NodeEditorWindow::onEditUndo()
{
    getCurrentNodeEditorWidget()->getScene()->getHistory()->undo();
}

void NodeEditorWindow::onEditRedo()
{
    getCurrentNodeEditorWidget()->getScene()->getHistory()->redo();
}

void NodeEditorWindow::onEditDelete()
{
    NodeEditorGraphicsView* view = getCurrentNodeEditorWidget()->getGraphicsView();
    if (view) {
        // Assuming NodeEditorGraphicsView has deleteSelected()
        view->deleteSelected();
    }
}

void NodeEditorWindow::onEditCut()
{
    QJsonObject data = getCurrentNodeEditorWidget()->getScene()->serializeSelected(true);
    QJsonDocument doc(data);
    QString strData = doc.toJson(QJsonDocument::Indented);
    qWarning() << "copying: "<< strData;

    QApplication::clipboard()->setText(strData);
}

void NodeEditorWindow::onEditCopy()
{
    QJsonObject data = getCurrentNodeEditorWidget()->getScene()->serializeSelected(false);
    QJsonDocument doc(data);
    QString strData = doc.toJson(QJsonDocument::Indented);

    QApplication::clipboard()->setText(strData);
}

void NodeEditorWindow::onEditPaste()
{
    QString rawData = QApplication::clipboard()->text();

    QJsonParseError parseError;
    QJsonDocument doc = QJsonDocument::fromJson(rawData.toUtf8(), &parseError);

    if (parseError.error != QJsonParseError::NoError) {
        qWarning() << "Pasting invalid JSON data!" << parseError.errorString();
        return;
    }

    if (!doc.isObject()) {
        qWarning() << "JSON root is not an object!";
        return;
    }

    QJsonObject data = doc.object();

    if (!data.contains("nodes")) {
        qWarning() << "JSON does not contain any nodes!";
        return;
    }

    getCurrentNodeEditorWidget()->getScene()->deserializeFromClipboard(data);
}


void NodeEditorWindow::setTitle()
{
    QString title = "Node Editor - ";
    title += getCurrentNodeEditorWidget()->getUserFriendlyFilename();
    setWindowTitle(title);

}

void NodeEditorWindow::closeEvent(QCloseEvent* event)
{
    if (maybeSave()) {
        event->accept();
    } else {
        event->ignore();
    }
}

bool NodeEditorWindow::isModified() const
{
    return getCurrentNodeEditorWidget()->getScene()->hasBeenModified();
}

bool NodeEditorWindow::maybeSave()
{
    if (!isModified())
        return true;

    QMessageBox::StandardButton res = QMessageBox::warning(
        this,
        QString("Scene Modified"),
        QString("The scene has been modified.\nDo you want to save your changes?"),
        QMessageBox::Save | QMessageBox::Discard | QMessageBox::Cancel
        );

    if (res == QMessageBox::Save) {
        return onFileSave();
    } else if (res == QMessageBox::Cancel) {
        return false;
    }

    return true;
}
