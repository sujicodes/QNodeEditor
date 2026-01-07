#ifndef CALCULATORWINDOW_H
#define CALCULATORWINDOW_H

#include "../../NodeEditorWindow.h" // your existing base window
#include "NodeEditorWidget.h"
#include <QMdiArea>
#include <QDockWidget>
#include <QListWidget>
#include <QAction>
#include <QMenu>
#include <QSignalMapper>
#include <qgraphicsscene.h>

class CalculatorWindow : public NodeEditorWindow {
    Q_OBJECT

public:
    explicit CalculatorWindow(QWidget *parent = nullptr);
    ~CalculatorWindow() override = default;

protected:
    void closeEvent(QCloseEvent *event) override;

private slots:
    void updateMenus();
    void updateWindowMenu();
    void about();
    void setActiveSubWindow(QWidget* window);
    void onFileNew() override;
    NodeEditorWidget* getCurrentNodeEditorWidget() const override;
    void onFileOpen() override;
    void hookEditorSignals(NodeEditorWidget* editor);
    void onSubWndClose(NodeEditorWidget* widget, QCloseEvent* event);

private:
    void initUI();
    void createActions();
    void createMenus();
    void createToolBars();
    void createStatusBar();
    void createNodesDock();
    void updateEditMenu();
    QMdiSubWindow* createMdiChild();
    QWidget* activeMdiChild();
    QMdiSubWindow* findMdiChild(const QString& filename);

    QMdiArea *mdiArea = nullptr;
    QDockWidget *itemsDock = nullptr;
    QListWidget *listWidget = nullptr;

    // Actions
    QAction *newAct = nullptr;
    QAction *closeAct = nullptr;
    QAction *closeAllAct = nullptr;
    QAction *tileAct = nullptr;
    QAction *cascadeAct = nullptr;
    QAction *nextAct = nullptr;
    QAction *previousAct = nullptr;
    QAction *separatorAct = nullptr;
    QAction *aboutAct = nullptr;

    // Menus
    QMenu *windowMenu = nullptr;
    QMenu *helpMenu = nullptr;
    QSignalMapper *windowMapper;
    QMdiSubWindow* lastActiveSubWindow = nullptr;
    QIcon emptyIcon = QIcon(".");
};

#endif // CALCULATORWINDOW_H
