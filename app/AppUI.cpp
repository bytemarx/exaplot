#include <QMessageBox>

#include "AppUI.hpp"


AppUI::AppUI(QObject* parent)
    : QObject{parent}
{
    this->mainWindow = new MainWindow{};
    this->aboutDialog = new About{this->mainWindow};
    QObject::connect(
        this->mainWindow->actionAbout(), &QAction::triggered,
        this->aboutDialog, &About::open
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
