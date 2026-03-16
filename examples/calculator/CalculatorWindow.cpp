#include "CalculatorWindow.h"
#include "CalculatorSubWindow.h"
#include "NodeEditorGraphicsScene.h"

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
    setWindowTitle("Calculator NodeEditor Example");

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
        if(auto* editor = getCurrentNodeEditorWidget()){
                hookEditorSignals(editor);
                connect(editor->getScene()->getHistory(), &QUndoStack::canUndoChanged, this, &CalculatorWindow::updateEditMenu);
                connect(editor->getScene()->getHistory(), &QUndoStack::canRedoChanged, this, &CalculatorWindow::updateEditMenu);
        }
        updateMenus();
    });

    windowMapper = new QSignalMapper(this);
    connect(windowMapper, SIGNAL(mapped(QWidget*)),
            this, SLOT(setActiveSubWindow(QWidget*)));


    createNodesDock();
    createActions();
    createMenus();
    createToolBars();
    createStatusBar();
    updateMenus();

    setGeometry(200, 200, 800, 600);
    setWindowTitle("Node Editor");
    show();
}

void CalculatorWindow::createActions()
{
    //NodeEditorWindow::createActions();   // IMPORTANT

    // --- now subclass actions ---
    closeAct = new QAction("Close", this);
    connect(closeAct, &QAction::triggered, mdiArea, &QMdiArea::closeActiveSubWindow);

    closeAllAct = new QAction("Close All", this);
    connect(closeAllAct, &QAction::triggered, mdiArea, &QMdiArea::closeAllSubWindows);

    tileAct = new QAction("Tile", this);
    connect(tileAct, &QAction::triggered, mdiArea, &QMdiArea::tileSubWindows);

    cascadeAct = new QAction("Cascade", this);
    connect(cascadeAct, &QAction::triggered, mdiArea, &QMdiArea::cascadeSubWindows);

    nextAct = new QAction("Next", this);
    nextAct->setShortcut(QKeySequence::NextChild);
    connect(nextAct, &QAction::triggered, mdiArea, &QMdiArea::activateNextSubWindow);

    previousAct = new QAction("Previous", this);
    previousAct->setShortcut(QKeySequence::PreviousChild);
    connect(previousAct, &QAction::triggered, mdiArea, &QMdiArea::activatePreviousSubWindow);

    separatorAct = new QAction(this);
    separatorAct->setSeparator(true);

    aboutAct = new QAction("About", this);
    connect(aboutAct, &QAction::triggered, this, &CalculatorWindow::about);
}

void CalculatorWindow::createMenus()
{
    // Window menu
    windowMenu = menuBar()->addMenu("Window");
    updateWindowMenu();
    connect(windowMenu, &QMenu::aboutToShow, this, &CalculatorWindow::updateWindowMenu);

    menuBar()->addSeparator();

    // Help menu
    helpMenu = menuBar()->addMenu("Help");
    helpMenu->addAction(aboutAct);
}

void CalculatorWindow::updateMenus()
{
    qDebug() << "CalculatorWindow::updateMenu";

    QWidget *active = getCurrentNodeEditorWidget();
    bool hasMdiChild = active;

    if(closeAct)      closeAct->setEnabled(hasMdiChild);
    if(closeAllAct)   closeAllAct->setEnabled(hasMdiChild);
    if(tileAct)       tileAct->setEnabled(hasMdiChild);
    if(cascadeAct)    cascadeAct->setEnabled(hasMdiChild);
    if(nextAct)       nextAct->setEnabled(true);
    if(previousAct)   previousAct->setEnabled(hasMdiChild);
    m_actSave->setEnabled(hasMdiChild);
    m_actSaveAs->setEnabled(hasMdiChild);
    qDebug() << "NodeEditorWindow::updateMenus -> save:" << m_actSave->isEnabled();
    if(separatorAct)  separatorAct->setVisible(hasMdiChild);
    updateEditMenu();
}

void CalculatorWindow::createToolBars()
{
    // Not used in your example, leave empty (placeholder)
}

void CalculatorWindow::createStatusBar()
{
    statusBar()->showMessage("Ready");
}

void CalculatorWindow::createNodesDock()
{
    QStringList keys = {"Input", "Output", "Add", "Subtract", "Multiply", "Divide"};
    QStringList paths = {"://icons/in.png", "://icons/out.png", "://icons/plus.png", "://icons/substract.png", "://icons/multiply.png", "://icons/divide.png"};
    QStringList classNames = {"InputNode", "OutputNode", "AddNode", "SubtractNode", "MultiplyNode", "DivideNode"};

    dragListWidget = new DragListWidget();
    dragListWidget->addCustomItems(keys, paths, classNames);
    nodesDock = new QDockWidget("Nodes", this);
    nodesDock->setWidget(dragListWidget);
    nodesDock->setFloating(false);

    addDockWidget(Qt::RightDockWidgetArea, nodesDock);
}

void CalculatorWindow::updateWindowMenu()
{
    windowMenu->clear();
    QAction* toolbarNodesAct = windowMenu->addAction("Nodes Toolbar");
    toolbarNodesAct->setCheckable(true);

    // Initial checked state matches dock visibility
    toolbarNodesAct->setChecked(nodesDock && nodesDock->isVisible());

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
        if(i < 9) text.prepend('&');

        QAction *action = windowMenu->addAction(text);
        action->setCheckable(true);
        action->setChecked(sub == mdiArea->currentSubWindow());

        // capture sub pointer in lambda
        connect(action, &QAction::triggered, this, [this, sub]() {
            if(sub) mdiArea->setActiveSubWindow(sub);
        });
    }
}

void CalculatorWindow::about()
{
    QMessageBox::about(this,
                        "About Calculator NodeEditor Example",
                        "The <b>Calculator NodeEditor</b> example demonstrates how to "
                        "write multiple document interface applications using Qt and your NodeEditor.");

    const QList<QMdiSubWindow *> windows = mdiArea->subWindowList();

    for (int i = 0; i < windows.size(); ++i) {
        QMdiSubWindow *window = windows.at(i);
        QWidget *child = window->widget();

        QString text = QString("%1 %2").arg(i + 1).arg(child->property("userFriendlyFilename").toString());
        if(i < 9)
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
    if(!window) return;
    // If user passed an inner widget, find its subwindow
    QMdiSubWindow *sub = qobject_cast<QMdiSubWindow*>(window);
    if(!sub) {
        // maybe they passed the child widget itself -> find containing subwindow
        sub = mdiArea->subWindowList().isEmpty() ? nullptr : mdiArea->subWindowList().first();
        // fallback: try to find the subwindow that contains the widget
        foreach (QMdiSubWindow* s, mdiArea->subWindowList()) {
            if(s->widget() == window) { sub = s; break; }
        }
    }
    if(sub) mdiArea->setActiveSubWindow(sub);
}

void CalculatorWindow::closeEvent(QCloseEvent *event)
{
    // try to close all subwindows
    mdiArea->closeAllSubWindows();
    if(mdiArea->currentSubWindow()) {
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
    QMdiSubWindow* sub = mdiArea->activeSubWindow();
    if(!sub) sub = lastActiveSubWindow;  // fallback to last active
    if(sub)
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
    QStringList fnames = QFileDialog::getOpenFileNames(this, "Open graph from file");

    for (const QString& fname : fnames)
    {
        if(fname.isEmpty())
            continue;

        // Check if already open
        QMdiSubWindow* existing = findMdiChild(fname);
        if(existing)
        {
            mdiArea->setActiveSubWindow(existing);
        }
        else
        {
            // Create new subwindow and load the file
            auto* subwnd = createMdiChild();
            CalculatorSubWindow* nodeEditor = qobject_cast<CalculatorSubWindow*>(subwnd->widget());

            if(nodeEditor->fileLoad(fname))
            {
                statusBar()->showMessage(QString("File %1 loaded").arg(fname), 5000);
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

    for(QMdiSubWindow* window : subWindows)
    {
        QWidget* childWidget = window->widget();
        auto nodeEditor = qobject_cast<NodeEditorWidget*>(childWidget); // example type
        if(nodeEditor && nodeEditor->getFilename() == filename) return window;
    }

    return nullptr; // None in Python
}

void CalculatorWindow::updateEditMenu()
{
    qDebug() << "CalculatorWindow::updateEditMenu";

    NodeEditorWidget* active = getCurrentNodeEditorWidget();
    bool hasMdiChild = active;

    // Paste only depends on whether an editor exists
    m_actPaste->setEnabled(hasMdiChild);

    // These depend on active editor + selection
    bool hasSelection = hasMdiChild && active->hasSelectedItems();

    m_actCut->setEnabled(hasSelection);
    m_actCopy->setEnabled(hasSelection);
    m_actDelete->setEnabled(hasSelection);

    // Undo / Redo depend on the editor's m_history
    m_actUndo->setEnabled(hasMdiChild && active->canUndo());
    m_actRedo->setEnabled(hasMdiChild && active->canRedo());
}

void CalculatorWindow::hookEditorSignals(NodeEditorWidget* editor)
{
    if(!editor) return;

    NodeEditorGraphicsScene* scene = getCurrentNodeEditorWidget()->getScene();

    connect(scene, &NodeEditorGraphicsScene::selectionChanged,
            this, &CalculatorWindow::updateMenus,
            Qt::UniqueConnection);
}

void CalculatorWindow::onSubWndClose(NodeEditorWidget* widget, QCloseEvent* event)
{
    if(!widget)
    {
        event->accept();
        return;
    }

    // Activate the subwindow owning this widget
    for(QMdiSubWindow* sub : mdiArea->subWindowList())
    {
        if(sub->widget() == widget)
        {
            mdiArea->setActiveSubWindow(sub);
            break;
        }
    }

    // Ask user
    if(maybeSave())
    {
        event->accept();
    }
    else
    {
        event->ignore();
    }
}
