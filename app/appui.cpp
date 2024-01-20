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
        this->mainWindow, &MainWindow::closed,
        [this] { emit this->closed(); }
    );
    QObject::connect(
        this->mainWindow->actionLoad(), &QAction::triggered,
        this, &AppUI::loadScript
    );
    QObject::connect(
        this->mainWindow->buttonRun(), &QPushButton::clicked,
        [this] { emit this->scriptRun(this->scriptArgs()); }
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


bool
AppUI::close()
{
    return this->mainWindow->close();
}


void
AppUI::reset()
{
    this->plotEditorDialog->reset();
    this->mainWindow->setPlots(this->plotEditorDialog->plots());
    this->mainWindow->plot(0)->clear();
}


void
AppUI::clear()
{
    auto n = this->mainWindow->plotCount();
    for (decltype(n) i = 0; i < n; ++i)
        this->mainWindow->plot(i)->clear();
}


std::map<std::string, std::string>
AppUI::scriptArgs() const
{
    return this->mainWindow->scriptArgs();
}


void
AppUI::setMessage(const QString& message)
{
    this->mainWindow->setMessage(message);
}


void
AppUI::initArgs(const std::vector<std::string>& params)
{
    this->mainWindow->initArgs(params);
}


void
AppUI::setScriptStatus(const QString& message)
{
    this->mainWindow->setScriptStatus(message);
}


QPlot*
AppUI::plot(std::size_t n)
{
    return this->mainWindow->plot(n);
}


std::size_t
AppUI::plotCount() const
{
    return this->mainWindow->plotCount();
}


void
AppUI::displayError(const QString& msg, const QString& title)
{
    QMessageBox::critical(this->mainWindow, title, msg);
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
    emit this->scriptLoad(file);
}
