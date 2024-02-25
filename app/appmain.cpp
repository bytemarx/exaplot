#include <QMessageBox>

#include "appmain.hpp"
#include "config.h"


AppMain::AppMain(int& argc, char* argv[])
    : QObject{Q_NULLPTR}
    , ifaceThread{}
    , iface{}
    , a{argc, argv}
    , ui{this}
    , scriptRunning{false}
{
    QObject::connect(&this->ifaceThread, &QThread::started, &this->iface, &Interface::pythonInit);
    QObject::connect(&this->ifaceThread, &QThread::finished, &this->iface, &Interface::pythonDeInit);
    this->iface.moveToThread(&this->ifaceThread);
    this->ifaceThread.start();

    QObject::connect(&this->a, &QApplication::aboutToQuit, [this] { this->shutdown(0); });
    QObject::connect(&this->ui, &AppUI::closed, [this] { this->shutdown(0); });
    QObject::connect(&this->ui, &AppUI::scriptLoad, this, &AppMain::load);
    QObject::connect(&this->ui, &AppUI::scriptRun, this, &AppMain::run);
    QObject::connect(&this->ui, &AppUI::plotsSet, &this->iface, &Interface::updatePlotProperties, Qt::QueuedConnection);
    QObject::connect(this, &AppMain::scriptLoaded, &this->iface, &Interface::loadScript, Qt::QueuedConnection);
    QObject::connect(this, &AppMain::scriptRan, &this->iface, &Interface::runScript, Qt::QueuedConnection);
    QObject::connect(&this->iface, &Interface::fatalError, this, &AppMain::shutdown, Qt::QueuedConnection);
    QObject::connect(&this->iface, &Interface::scriptErrored, this, &AppMain::scriptError, Qt::QueuedConnection);
    QObject::connect(&this->iface, &Interface::runCompleted, this, &AppMain::runComplete, Qt::QueuedConnection);
    QObject::connect(&this->iface, &Interface::module_init, this, &AppMain::module_init, Qt::QueuedConnection);
    QObject::connect(&this->iface, &Interface::module_msg, this, &AppMain::module_msg, Qt::QueuedConnection);
    QObject::connect(&this->iface, &Interface::module_plot2D, this, &AppMain::module_plot2D, Qt::QueuedConnection);
    QObject::connect(&this->iface, &Interface::module_plot2DVec, this, &AppMain::module_plot2DVec, Qt::QueuedConnection);
    QObject::connect(&this->iface, &Interface::module_plotCM, this, &AppMain::module_plotCM, Qt::QueuedConnection);
    QObject::connect(&this->iface, &Interface::module_plotCMVec, this, &AppMain::module_plotCMVec, Qt::QueuedConnection);
    QObject::connect(&this->iface, &Interface::module_clear, this, &AppMain::module_clear, Qt::QueuedConnection);
    QObject::connect(&this->iface, &Interface::module_setPlotProperty, this, &AppMain::module_setPlotProperty, Qt::QueuedConnection);
    QObject::connect(&this->iface, &Interface::module_showPlot, this, &AppMain::module_showPlot, Qt::QueuedConnection);
}


AppMain::~AppMain()
{
}


int
AppMain::exec()
{
    this->ui.show();
    return this->a.exec();
}


void
AppMain::shutdown(int status)
{
    bool terminate = false;
    if (this->scriptRunning) {
        QMessageBox msgBox{
            QMessageBox::Warning,
            "",
            "Script is running. Force quit?",
            QMessageBox::Yes | QMessageBox::No
        };
        switch (msgBox.exec()) {
            default: return;
            case QMessageBox::Yes: terminate = true;
        }
    }
    if (this->scriptRunning && terminate) {
        this->ifaceThread.terminate();
        this->scriptRunning = false;
    } else {
        this->ifaceThread.quit();
        this->ifaceThread.wait();
    }
    this->ui.close();
    this->a.exit(status);
}


void
AppMain::load(const QString& file)
{
    if (this->scriptRunning) {
        this->scriptError(
            "Cannot load new script while current script is running.",
            "Load Error"
        );
        return;
    }
    this->ui.setScriptStatus();
    this->reset();
    emit this->scriptLoaded(file);
}


void
AppMain::run(const std::map<std::string, std::string>& kwargs)
{
    if (this->scriptRunning)
        return;
    this->ui.enableRun(false);
    this->ui.enableStop(true);
    this->ui.clear();
    this->ui.setScriptStatus("Running...");
    emit this->scriptRan(kwargs);
    this->scriptRunning = true;
}


void
AppMain::runComplete(const QString& message)
{
    this->scriptRunning = false;
    this->ui.setScriptStatus(message);
    this->ui.enableStop(false);
    this->ui.enableRun(true);
}


void
AppMain::module_init(
    const std::vector<std::string>& params,
    const std::vector<orbital::GridPoint>& plots)
{
    this->ui.initArgs(params);
}


void
AppMain::module_msg(const std::string& message, bool append)
{
    this->ui.setMessage(QString::fromStdString(message));
}


void
AppMain::module_plot2D(std::size_t plotIdx, double x, double y)
{
    auto plot = this->ui.plot(plotIdx);
    plot->plot2D()->addData(x, y);
    plot->queue();
}


void
AppMain::module_plot2DVec(
    std::size_t plotIdx,
    const std::vector<double>& x,
    const std::vector<double>& y)
{
    auto plot = this->ui.plot(plotIdx);
    plot->plot2D()->addData({x.cbegin(), x.cend()}, {y.cbegin(), y.cend()});
    plot->queue();
}


void
AppMain::module_plotCM(std::size_t plotIdx, int x, int y, double value)
{
    auto plot = this->ui.plot(plotIdx);
    plot->plotColorMap()->setCell(x, y, value);
    plot->queue();
}


void
AppMain::module_plotCMVec(std::size_t plotIdx, int y, const std::vector<double>& values)
{
    auto plot = this->ui.plot(plotIdx);
    auto x_end = static_cast<std::size_t>(plot->plotColorMap()->getDataSizeX()) < values.size()
        ? static_cast<std::size_t>(plot->plotColorMap()->getDataSizeX())
        : values.size();
    for (decltype(x_end) x = 0; x < x_end; ++x)
        plot->plotColorMap()->setCell(x, y, values[x]);
    plot->queue();
}


void
AppMain::module_clear(std::size_t plotIdx)
{
    auto plot = this->ui.plot(plotIdx);
    plot->plot2D()->clear();
    plot->plotColorMap()->clear();
    plot->queue();
}


void
AppMain::module_setPlotProperty(
    std::size_t plotIdx,
    const orbital::PlotProperty& property,
    const QPlotTab::Cache& properties)
{
    this->ui.setPlotProperty(plotIdx, property, properties);
}


void
AppMain::module_showPlot(std::size_t plotIdx, QPlot::Type plotType)
{
    this->ui.showPlot(plotIdx, plotType);
}


void
AppMain::reset()
{
    this->ui.reset();
}


void
AppMain::scriptError(const QString& message, const QString& title)
{
    this->ui.displayError(message, title);
}
