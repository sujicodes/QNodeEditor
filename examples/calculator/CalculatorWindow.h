#ifndef CALCULATORWINDOW_H
#define CALCULATORWINDOW_H

#include "../../NodeEditorWindow.h" // your existing base window
#include <QMdiArea>
#include <QDockWidget>
#include <QListWidget>
#include <QAction>
#include <QMenu>
#include <QSignalMapper>

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

private:
    void initUI();
    void createActions();
    void createMenus();
    void createToolBars();
    void createStatusBar();
    void createNodesDock();
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
};

#endif // CALCULATORWINDOW_H
