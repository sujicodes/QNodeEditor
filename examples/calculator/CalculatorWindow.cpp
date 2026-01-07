#include "CalculatorWindow.h"
#include "CalculatorSubWindow.h"
#include "NodeGraphicsScene.h"
#include "Scene.h"

#include <QMenuBar>
#include <QStatusBar>
#include <QMessageBox>
#include <QCloseEvent>
#include <QKeySequence>
#include <QMdiSubWindow>
#include <QFileDialog>

CalculatorWindow::CalculatorWindow(QWidget *parent)
{
    initUI();
}

void CalculatorWindow::initUI()
{
    // window title
    setWindowTitle(tr("Calculator NodeEditor Example"));

    // central MDI area
    mdiArea = new QMdiArea(this);
    mdiArea->setHorizontalScrollBarPolicy(Qt::ScrollBarAsNeeded);
    mdiArea->setVerticalScrollBarPolicy(Qt::ScrollBarAsNeeded);
    mdiArea->setViewMode(QMdiArea::TabbedView);
    mdiArea->setDocumentMode(true);
    mdiArea->setTabsClosable(true);
    mdiArea->setTabsMovable(true);
    setCentralWidget(mdiArea);

    // Connect MDI signals
    connect(mdiArea, &QMdiArea::subWindowActivated, this, [this](QMdiSubWindow* sub){
        lastActiveSubWindow = sub;
        if (auto* editor = getCurrentNodeEditorWidget()){
                hookEditorSignals(editor);
                connect(editor->getScene()->getHistory(), &QUndoStack::canUndoChanged, this, &CalculatorWindow::updateEditMenu);
                connect(editor->getScene()->getHistory(), &QUndoStack::canRedoChanged, this, &CalculatorWindow::updateEditMenu);
        }
        updateMenus();
    });

    windowMapper = new QSignalMapper(this);
    connect(windowMapper, SIGNAL(mapped(QWidget*)),
            this, SLOT(setActiveSubWindow(QWidget*)));


    createActions();
    createMenus();
    createToolBars();
    createStatusBar();
    createNodesDock();
    updateMenus();

    setGeometry(200, 200, 800, 600);
    setWindowTitle("Node Editor");
    show();
}

void CalculatorWindow::createActions()
{

    //NodeEditorWindow::createActions();   // IMPORTANT

    // --- now subclass actions ---
    closeAct = new QAction(tr("Cl&ose"), this);
    connect(closeAct, &QAction::triggered, mdiArea, &QMdiArea::closeActiveSubWindow);

    closeAllAct = new QAction(tr("Close &All"), this);
    connect(closeAllAct, &QAction::triggered, mdiArea, &QMdiArea::closeAllSubWindows);

    tileAct = new QAction(tr("&Tile"), this);
    connect(tileAct, &QAction::triggered, mdiArea, &QMdiArea::tileSubWindows);

    cascadeAct = new QAction(tr("&Cascade"), this);
    connect(cascadeAct, &QAction::triggered, mdiArea, &QMdiArea::cascadeSubWindows);

    nextAct = new QAction(tr("Ne&xt"), this);
    nextAct->setShortcut(QKeySequence::NextChild);
    connect(nextAct, &QAction::triggered, mdiArea, &QMdiArea::activateNextSubWindow);

    previousAct = new QAction(tr("Pre&vious"), this);
    previousAct->setShortcut(QKeySequence::PreviousChild);
    connect(previousAct, &QAction::triggered, mdiArea, &QMdiArea::activatePreviousSubWindow);

    separatorAct = new QAction(this);
    separatorAct->setSeparator(true);

    aboutAct = new QAction(tr("&About"), this);
    connect(aboutAct, &QAction::triggered, this, &CalculatorWindow::about);


}

void CalculatorWindow::createMenus()
{
    // Window menu
    windowMenu = menuBar()->addMenu(tr("&Window"));
    updateWindowMenu();
    connect(windowMenu, &QMenu::aboutToShow, this, &CalculatorWindow::updateWindowMenu);

    menuBar()->addSeparator();

    // Help menu
    helpMenu = menuBar()->addMenu(tr("&Help"));
    helpMenu->addAction(aboutAct);
}

void CalculatorWindow::updateMenus()
{
    qDebug() << "update Menus";

    QWidget *active = getCurrentNodeEditorWidget();
    bool hasMdiChild = (active != nullptr);

    // Enable/disable actions
    if (newAct)        newAct->setEnabled(true);        // "New" is usually always enabled
    if (closeAct)      closeAct->setEnabled(hasMdiChild);
    if (closeAllAct)   closeAllAct->setEnabled(hasMdiChild);
    if (tileAct)       tileAct->setEnabled(hasMdiChild);
    if (cascadeAct)    cascadeAct->setEnabled(hasMdiChild);
    if (nextAct)       nextAct->setEnabled(true);
    if (previousAct)   previousAct->setEnabled(hasMdiChild);
    actSave->setEnabled(hasMdiChild);
    actSaveAs->setEnabled(hasMdiChild);
    qDebug() << "NodeEditorWindow::updateMenus -> save:" << actSave->isEnabled();
    if (separatorAct)  separatorAct->setVisible(hasMdiChild);
    updateEditMenu();
}

void CalculatorWindow::createToolBars()
{
    // Not used in your example, leave empty (placeholder)
}

void CalculatorWindow::createStatusBar()
{
    statusBar()->showMessage(tr("Ready"));
}

void CalculatorWindow::createNodesDock()
{
    listWidget = new QListWidget(this);
    listWidget->addItem(tr("Add"));
    listWidget->addItem(tr("Substract"));
    listWidget->addItem(tr("Multiply"));
    listWidget->addItem(tr("Divide"));

    itemsDock = new QDockWidget(tr("Nodes"), this);
    itemsDock->setWidget(listWidget);
    itemsDock->setFloating(false);

    addDockWidget(Qt::RightDockWidgetArea, itemsDock);
}


void CalculatorWindow::updateWindowMenu()
{
    windowMenu->clear();
    windowMenu->addAction(closeAct);
    windowMenu->addAction(closeAllAct);
    windowMenu->addSeparator();
    windowMenu->addAction(tileAct);
    windowMenu->addAction(cascadeAct);
    windowMenu->addSeparator();
    windowMenu->addAction(nextAct);
    windowMenu->addAction(previousAct);
    windowMenu->addAction(separatorAct);

    const auto windows = mdiArea->subWindowList();
    separatorAct->setVisible(!windows.isEmpty());

    // Add each window as a numbered, checkable menu item
    for (int i = 0; i < windows.size(); ++i) {
        QMdiSubWindow *sub = windows.at(i);
        QWidget *child = sub->widget();
        QString text = QString("%1 %2").arg(i + 1).arg(child ? child->windowTitle() : QString());
        if (i < 9) text.prepend('&');

        QAction *action = windowMenu->addAction(text);
        action->setCheckable(true);
        action->setChecked(sub == mdiArea->currentSubWindow());

        // capture sub pointer in lambda
        connect(action, &QAction::triggered, this, [this, sub]() {
            if (sub) mdiArea->setActiveSubWindow(sub);
        });
    }
}


void CalculatorWindow::about()
{
    QMessageBox::about(this,
                       tr("About Calculator NodeEditor Example"),
                       tr("The <b>Calculator NodeEditor</b> example demonstrates how to "
                          "write multiple document interface applications using Qt and your NodeEditor."));

    const QList<QMdiSubWindow *> windows = mdiArea->subWindowList();

    for (int i = 0; i < windows.size(); ++i) {
        QMdiSubWindow *window = windows.at(i);
        QWidget *child = window->widget();

        QString text = QString("%1 %2").arg(i + 1).arg(child->property("userFriendlyFilename").toString());
        if (i < 9)
            text.prepend('&');  // add Alt shortcut accelerator

        QAction *action = windowMenu->addAction(text);
        action->setCheckable(true);
        action->setChecked(window == mdiArea->activeSubWindow());

        connect(action, &QAction::triggered, windowMapper, QOverload<>::of(&QSignalMapper::map));
        windowMapper->setMapping(action, window);
    }
}

void CalculatorWindow::setActiveSubWindow(QWidget* window)
{
    if (!window) return;
    // If user passed an inner widget, find its subwindow
    QMdiSubWindow *sub = qobject_cast<QMdiSubWindow*>(window);
    if (!sub) {
        // maybe they passed the child widget itself -> find containing subwindow
        sub = mdiArea->subWindowList().isEmpty() ? nullptr : mdiArea->subWindowList().first();
        // fallback: try to find the subwindow that contains the widget
        foreach (QMdiSubWindow* s, mdiArea->subWindowList()) {
            if (s->widget() == window) { sub = s; break; }
        }
    }
    if (sub) mdiArea->setActiveSubWindow(sub);
}

void CalculatorWindow::closeEvent(QCloseEvent *event)
{
    // try to close all subwindows
    mdiArea->closeAllSubWindows();
    if (mdiArea->currentSubWindow()) {
        // some subwindow rejected close
        event->ignore();
    } else {
        // OK to close
        event->accept();
    }
}

QMdiSubWindow* CalculatorWindow::createMdiChild()
{
    auto *nodeeditor = new CalculatorSubWindow(this);  // your custom widget
    QMdiSubWindow *subWnd = mdiArea->addSubWindow(nodeeditor);
    subWnd->setWindowIcon(emptyIcon);

    connect(
        nodeeditor,
        &CalculatorSubWindow::closeRequested,
        this,
        &CalculatorWindow::onSubWndClose
        );

    return subWnd;

}

NodeEditorWidget* CalculatorWindow::getCurrentNodeEditorWidget() const
{
    qDebug() << "im sub:";
    QMdiSubWindow* sub = mdiArea->activeSubWindow();
    if (!sub) sub = lastActiveSubWindow;  // fallback to last active
    if (sub)
        return dynamic_cast<NodeEditorWidget*>(sub->widget());

    return nullptr;
}

void CalculatorWindow::onFileNew()
{
    try {
        QMdiSubWindow* subwnd = createMdiChild();
        dynamic_cast<CalculatorSubWindow*>(subwnd->widget())->fileNew();
        subwnd->show();
    } catch (std::exception &e) {
        qWarning() << "Exception while creating new MDI child:" << e.what();
    } catch (...) {
        qWarning() << "Unknown exception while creating new MDI child.";
    }
}

void CalculatorWindow::onFileOpen()
{
    QStringList fnames = QFileDialog::getOpenFileNames(this, tr("Open graph from file"));

    for (const QString& fname : fnames)
    {
        if (fname.isEmpty())
            continue;

        // Check if already open
        QMdiSubWindow* existing = findMdiChild(fname);
        if (existing)
        {
            mdiArea->setActiveSubWindow(existing);
        }
        else
        {
            // Create new subwindow and load the file
            auto* subwnd = createMdiChild();
            CalculatorSubWindow* nodeEditor = qobject_cast<CalculatorSubWindow*>(subwnd->widget());

            if (nodeEditor->fileLoad(fname))
            {
                statusBar()->showMessage(tr("File %1 loaded").arg(fname), 5000);
                nodeEditor->setTitle();
                subwnd->show();
            }
            else
            {
                nodeEditor->close();
                delete nodeEditor;
            }
        }
    }
}

QMdiSubWindow* CalculatorWindow::findMdiChild(const QString& filename)
{
    // Loop through all subwindows in the MDI area
    const QList<QMdiSubWindow*> subWindows = mdiArea->subWindowList();

    for (QMdiSubWindow* window : subWindows)
    {
        QWidget* childWidget = window->widget();
        auto nodeEditor = qobject_cast<NodeEditorWidget*>(childWidget); // example type
        if (nodeEditor && nodeEditor->getFilename() == filename)
            return window;

    }

    return nullptr; // None in Python
}

void CalculatorWindow::updateEditMenu()
{
    qDebug() << "update Edit Menu";

    NodeEditorWidget* active = getCurrentNodeEditorWidget();
    bool hasMdiChild = (active != nullptr);

    // Paste only depends on whether an editor exists
    actPaste->setEnabled(hasMdiChild);

    // These depend on active editor + selection
    bool hasSelection = hasMdiChild && active->hasSelectedItems();

    actCut->setEnabled(hasSelection);
    actCopy->setEnabled(hasSelection);
    actDelete->setEnabled(hasSelection);

    // Undo / Redo depend on the editor's history
    actUndo->setEnabled(hasMdiChild && active->canUndo());
    actRedo->setEnabled(hasMdiChild && active->canRedo());
}

void CalculatorWindow::hookEditorSignals(NodeEditorWidget* editor)
{
    if (!editor) return;

    NodeGraphicsScene* scene = getCurrentNodeEditorWidget()->getScene()->graphicsScene();

    connect(scene, &NodeGraphicsScene::selectionChanged,
            this, &CalculatorWindow::updateMenus,
            Qt::UniqueConnection);
}

void CalculatorWindow::onSubWndClose(NodeEditorWidget* widget, QCloseEvent* event)
{
    qDebug() << "LOOOLOLLOL";
    if (!widget) {
        event->accept();
        return;
    }

    // Activate the subwindow owning this widget
    for (QMdiSubWindow* sub : mdiArea->subWindowList()) {
        if (sub->widget() == widget) {
            mdiArea->setActiveSubWindow(sub);
            break;
        }
    }

    // Ask user
    if (maybeSave()) {
        event->accept();
    } else {
        event->ignore();
    }
}

