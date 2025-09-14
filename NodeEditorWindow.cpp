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
    QMenuBar *menubar = menuBar();

    // File menu
    QMenu *fileMenu = menubar->addMenu("&File");
    fileMenu->addAction(createAct("&New", "Ctrl+N", "Create new graph", this, SLOT(onFileNew())));
    fileMenu->addSeparator();
    fileMenu->addAction(createAct("&Open", "Ctrl+O", "Open file", this, SLOT(onFileOpen())));
    fileMenu->addAction(createAct("&Save", "Ctrl+S", "Save file", this, SLOT(onFileSave())));
    fileMenu->addAction(createAct("Save &As...", "Ctrl+Shift+S", "Save file as...", this, SLOT(onFileSaveAs())));
    fileMenu->addSeparator();
    fileMenu->addAction(createAct("E&xit", "Ctrl+Q", "Exit application", this, SLOT(close())));

    // Edit menu
    QMenu *editMenu = menubar->addMenu("&Edit");
    editMenu->addAction(createAct("Cu&t", "Ctrl+X", "Cut to clipboard", this, SLOT(onEditCut())));
    editMenu->addAction(createAct("&Copy", "Ctrl+C", "Copy to clipboard", this, SLOT(onEditCopy())));
    editMenu->addAction(createAct("&Paste", "Ctrl+V", "Paste from clipboard", this, SLOT(onEditPaste())));
    editMenu->addSeparator();
    editMenu->addAction(createAct("&Redo", "Ctrl+Shift+Z", "Redo last operation", this, SLOT(onEditRedo())));
    editMenu->addAction(createAct("&Undo", "Ctrl+Z", "Undo last operation", this, SLOT(onEditUndo())));
    editMenu->addSeparator();
    editMenu->addAction(createAct("&Delete", "Del", "Delete selected items", this, SLOT(onEditDelete())));

    // Node editor widget
    nodeEditorWidget = new NodeEditorWidget(this);
    setCentralWidget(nodeEditorWidget);

    // Status bar
    statusBar()->showMessage("");
    statusMousePos = new QLabel("");
    statusBar()->addPermanentWidget(statusMousePos);

    connect(nodeEditorWidget->getGraphicsView(), &NodeEditorGraphicsView::scenePosChanged, this, &NodeEditorWindow::onScenePosChanged);

    // Window properties
    setGeometry(200, 200, 800, 600);
    setWindowTitle("Node Editor");
    show();
}

void NodeEditorWindow::onScenePosChanged(int x, int y)
{
    statusMousePos->setText(QString("Scene Pos: [%1, %2]").arg(x).arg(y));
}

void NodeEditorWindow::onFileNew()
{
    nodeEditorWidget->getScene()->clearScene();
}

void NodeEditorWindow::onFileOpen()
{
    QString fname = QFileDialog::getOpenFileName(this, "Open graph from file");
    if (fname.isEmpty()) return;

    QFile file(fname);
    if (file.exists()) {
        nodeEditorWidget->getScene()->loadFromFile(fname);
    }
}

void NodeEditorWindow::onFileSave()
{
    if (filename.isEmpty()) {
        onFileSaveAs();
        return;
    }
    nodeEditorWidget->getScene()->saveToFile(filename);
    statusBar()->showMessage(QString("Successfully saved %1🧩").arg(filename));
}

void NodeEditorWindow::onFileSaveAs()
{
    QString fname = QFileDialog::getSaveFileName(this, "Save graph to file");
    if (fname.isEmpty()) return;
    filename = fname;
    onFileSave();
}

void NodeEditorWindow::onEditUndo()
{
    nodeEditorWidget->getScene()->getHistory()->undo();
}

void NodeEditorWindow::onEditRedo()
{
    nodeEditorWidget->getScene()->getHistory()->redo();
}

void NodeEditorWindow::onEditDelete()
{
    NodeEditorGraphicsView* view = nodeEditorWidget->getGraphicsView();
    if (view) {
        // Assuming NodeEditorGraphicsView has deleteSelected()
        view->deleteSelected();
    }
}

void NodeEditorWindow::onEditCut()
{
    QJsonObject data = nodeEditorWidget->getScene()->serializeSelected(true);
    QJsonDocument doc(data);
    QString strData = doc.toJson(QJsonDocument::Indented);
    qWarning() << "copying: "<< strData;

    QApplication::clipboard()->setText(strData);
}

void NodeEditorWindow::onEditCopy()
{
    QJsonObject data = nodeEditorWidget->getScene()->serializeSelected(false);
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

    nodeEditorWidget->getScene()->deserializeFromClipboard(data);
}
