#include "CalculatorSubWindow.h"
#include "../../Scene.h"
#include <QDebug>
#include <QEvent>

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
        getScene()->addHasBeenModifiedListener(setTitle());
    }
}

void CalculatorSubWindow::setTitle()
{
    // Assuming NodeEditorWidget provides getUserFriendlyFilename()
    setWindowTitle(getUserFriendlyFilename());
}
