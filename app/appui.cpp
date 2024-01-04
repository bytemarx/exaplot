#include <QMessageBox>

#include "appui.hpp"


AppUI::AppUI(QObject* parent)
    : QObject{parent}
    , mainWindow{new MainWindow}
    , aboutDialog{new About{this->mainWindow}}
    , plotEditorDialog{new PlotEditor{this->mainWindow}}
{
    QObject::connect(
        this->mainWindow->actionAbout(), &QAction::triggered,
        this->aboutDialog, &About::open
    );
    QObject::connect(
        this->mainWindow->actionPlotEditor(), &QAction::triggered,
        this->plotEditorDialog, &PlotEditor::open
    );
}


AppUI::~AppUI()
{
    delete this->mainWindow;
}


void
AppUI::show()
{
    this->mainWindow->show();
}


void
AppUI::displayError(const QString& msg)
{
    QMessageBox::critical(this->mainWindow, "ERROR", msg);
}
