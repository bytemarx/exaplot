#include <QMessageBox>

#include "appui.hpp"


AppUI::AppUI(QObject* parent)
    : QObject{parent}
    , mainWindow{new MainWindow}
    , aboutDialog{new About{this->mainWindow}}
    , plotEditorDialog{new PlotEditor{this->mainWindow}}
{
    this->mainWindow->setPlots(this->plotEditorDialog->plots());

    QObject::connect(
        this->mainWindow->actionLoad(), &QAction::triggered,
        this, &AppUI::loadScript
    );
    QObject::connect(
        this->mainWindow->actionAbout(), &QAction::triggered,
        this->aboutDialog, &About::open
    );
    QObject::connect(
        this->mainWindow->actionPlotEditor(), &QAction::triggered,
        this->plotEditorDialog, &PlotEditor::open
    );
    QObject::connect(
        this->plotEditorDialog, &QDialog::accepted,
        [=] { this->mainWindow->setPlots(this->plotEditorDialog->plots()); }
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
AppUI::displayError(const QString& msg, const QString& title)
{
    QMessageBox::critical(this->mainWindow, title, msg);
}


std::map<std::string, std::string>
AppUI::scriptArgs() const
{
    std::map<std::string, std::string> kwargs;
    return kwargs;
}


void
AppUI::setMessage(const QString& message)
{
    this->mainWindow->setMessage(message);
}


void
AppUI::loadScript()
{
    auto file = QFileDialog::getOpenFileName(
        this->mainWindow,
        tr("Load Script File"),
        QString{},
        tr("Python Files (*.py *.opy);;All Files (*)")
    );
    if (file.isEmpty()) return;
    emit this->scriptLoaded(file);
}
