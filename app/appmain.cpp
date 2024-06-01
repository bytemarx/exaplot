/*
 * ExaPlot
 * app main component
 * 
 * SPDX-License-Identifier: GPL-3.0
 * Copyright (C) 2024 bytemarx
 */

#include <QMessageBox>

#include "appmain.hpp"
#include "config.h"
#include "toml.hpp"

#include <cstdlib>
#include <iostream>


Config::Config() {
    const char* configPath = std::getenv("EXACONFIG");
    if (configPath == NULL)
        return;

    try {
        auto config = toml::parse_file(configPath);

        const auto& config_searchPaths = config.at_path("python.search_paths");
        if (config_searchPaths && config_searchPaths.is_array()) {
            for (auto&& entry : *config_searchPaths.as_array())
                if (auto path = entry.value<std::string>()) this->m_searchPaths.push_back(*path);
        }
    } catch (const toml::parse_error& e) {
        std::cerr << "Failed to read config (" << configPath << "):\n" << e << '\n';
    }
}


AppMain::AppMain(int& argc, char* argv[], const Config& config)
    : QObject{Q_NULLPTR}
    , ifaceThread{}
    , iface{config.searchPaths()}
    , dmThread{}
    , dm{}
    , a{argc, argv}
    , ui{this}
    , promptBeforeRun{false}
    , scriptRunning{false}
{
    QObject::connect(&this->ifaceThread, &QThread::started, &this->iface, &Interface::pythonInit);
    QObject::connect(&this->ifaceThread, &QThread::finished, &this->iface, &Interface::pythonDeInit);
    this->iface.moveToThread(&this->ifaceThread);

    this->dm.moveToThread(&this->dmThread);

    QObject::connect(&this->a, &QApplication::aboutToQuit, [this] { this->shutdown(0); });
    QObject::connect(&this->dm, &DataManager::error, this, &AppMain::dmError);
    QObject::connect(&this->ui, &AppUI::closed, [this] { this->shutdown(0); });
    QObject::connect(&this->ui, &AppUI::scriptLoad, this, &AppMain::load);
    QObject::connect(&this->ui, &AppUI::scriptRun, this, &AppMain::run);
    QObject::connect(&this->ui, &AppUI::scriptStop, this, &AppMain::stop);
    QObject::connect(&this->ui, &AppUI::plotsSet, &this->iface, &Interface::updatePlotProperties, Qt::QueuedConnection);
    QObject::connect(this, &AppMain::scriptLoaded, &this->iface, &Interface::loadScript, Qt::QueuedConnection);
    QObject::connect(this, &AppMain::scriptRan, &this->iface, &Interface::runScript, Qt::QueuedConnection);
    QObject::connect(this, &AppMain::scriptStopped, &this->iface, &Interface::requestStop);
    QObject::connect(this, &AppMain::dmConfigure, &this->dm, &DataManager::configure, Qt::QueuedConnection);
    QObject::connect(this, &AppMain::dmOpen, &this->dm, &DataManager::open, Qt::QueuedConnection);
    QObject::connect(this, &AppMain::dmClose, &this->dm, &DataManager::close, Qt::QueuedConnection);
    QObject::connect(this, &AppMain::dmWrite2D, &this->dm, &DataManager::write2D, Qt::QueuedConnection);
    QObject::connect(this, &AppMain::dmWrite2DVec, &this->dm, &DataManager::write2DVec, Qt::QueuedConnection);
    QObject::connect(this, &AppMain::dmWriteCM, &this->dm, &DataManager::writeCM, Qt::QueuedConnection);
    QObject::connect(this, &AppMain::dmWriteCMVec, &this->dm, &DataManager::writeCMVec, Qt::QueuedConnection);
    QObject::connect(this, &AppMain::dmWriteCMFrame, &this->dm, &DataManager::writeCMFrame, Qt::QueuedConnection);
    QObject::connect(this, &AppMain::dmFlush, &this->dm, &DataManager::flush, Qt::QueuedConnection);
    QObject::connect(&this->iface, &Interface::fatalError, this, &AppMain::shutdown, Qt::QueuedConnection);
    QObject::connect(&this->iface, &Interface::initializeDatafile, this, &AppMain::initializeDatafile, Qt::QueuedConnection);
    QObject::connect(&this->iface, &Interface::scriptErrored, this, &AppMain::scriptError, Qt::QueuedConnection);
    QObject::connect(&this->iface, &Interface::scriptStatusUpdated, this, &AppMain::updateScriptStatus, Qt::QueuedConnection);
    QObject::connect(&this->iface, &Interface::runCompleted, this, &AppMain::runComplete, Qt::QueuedConnection);
    QObject::connect(&this->iface, &Interface::module_init, this, &AppMain::module_init, Qt::QueuedConnection);
    QObject::connect(&this->iface, &Interface::module_msg, this, &AppMain::module_msg, Qt::QueuedConnection);
    QObject::connect(&this->iface, &Interface::module_datafile, this, &AppMain::module_datafile, Qt::QueuedConnection);
    QObject::connect(&this->iface, &Interface::module_plot2D, this, &AppMain::module_plot2D, Qt::QueuedConnection);
    QObject::connect(&this->iface, &Interface::module_plot2DVec, this, &AppMain::module_plot2DVec, Qt::QueuedConnection);
    QObject::connect(&this->iface, &Interface::module_plotCM, this, &AppMain::module_plotCM, Qt::QueuedConnection);
    QObject::connect(&this->iface, &Interface::module_plotCMVec, this, &AppMain::module_plotCMVec, Qt::QueuedConnection);
    QObject::connect(&this->iface, &Interface::module_plotCMFrame, this, &AppMain::module_plotCMFrame, Qt::QueuedConnection);
    QObject::connect(&this->iface, &Interface::module_clear, this, &AppMain::module_clear, Qt::QueuedConnection);
    QObject::connect(&this->iface, &Interface::module_setPlotProperty, this, &AppMain::module_setPlotProperty, Qt::QueuedConnection);
    QObject::connect(&this->iface, &Interface::module_showPlot, this, &AppMain::module_showPlot, Qt::QueuedConnection);

    this->ifaceThread.start();
    this->dmThread.start();
    this->a.setStyle(QStyleFactory::create("fusion"));
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
    this->dmThread.quit();
    if (this->scriptRunning && terminate) {
        this->ifaceThread.terminate();
        this->scriptRunning = false;
    } else {
        this->ifaceThread.quit();
        this->ifaceThread.wait();
    }
    this->dmThread.wait();
    this->ui.close();
    this->a.exit(status);
}


void
AppMain::dmError(const QString& msg)
{
    this->iface.setError(true);
    this->scriptError(msg, "Data Error");
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
AppMain::run(const std::vector<std::string>& args)
{
    if (this->scriptRunning)
        return;
    this->ui.enableRun(false);
    this->ui.enableStop(true);
    this->ui.clear();
    emit this->scriptRan(args);
    this->scriptRunning = true;
}


void
AppMain::stop()
{
    if (!this->scriptRunning)
        return;
    this->ui.enableStop(false);
    this->iface.requestStop();
}


void
AppMain::initializeDatafile(std::filesystem::path datafile)
{
    if (this->promptBeforeRun)
        datafile = this->ui.promptDatafile(datafile);

    QEventLoop waitLoop;
    bool datafileError = false;
    QString datafileErrorMsg;
    QObject::connect(
        &this->dm,
        &DataManager::opened,
        &waitLoop,
        [&](bool error, const QString& message) {
            datafileError = error;
            datafileErrorMsg = message;
            waitLoop.quit();
        },
        Qt::QueuedConnection
    );
    emit this->dmOpen(datafile, this->ui.plotCount() + 1);
    waitLoop.exec();

    emit this->iface.datafileInitializationCompleted(datafileError);
    if (datafileError)
        this->scriptError(datafileErrorMsg, "Data Error");
}


void
AppMain::updateScriptStatus(const QString& scriptStatus)
{
    this->ui.setScriptStatus(scriptStatus);
}


void
AppMain::runComplete(const QString& scriptStatus)
{
    QEventLoop waitLoop;
    bool datafileError = false;
    QString datafileErrorMsg;
    QObject::connect(
        &this->dm,
        &DataManager::closed,
        &waitLoop,
        [&](bool error, const QString& message) {
            datafileError = error;
            datafileErrorMsg = message;
            waitLoop.quit();
        },
        Qt::QueuedConnection
    );
    emit this->dmClose();
    waitLoop.exec();

    this->scriptRunning = false;
    this->ui.setScriptStatus(scriptStatus);
    this->ui.enableStop(false);
    this->ui.enableRun(true);

    if (datafileError)
        this->scriptError(datafileErrorMsg, "Data Error");
}


void
AppMain::module_init(
    const std::vector<exa::RunParam>& params,
    const std::vector<exa::GridPoint>& plots)
{
    std::vector<std::pair<std::string, std::string>> paramDisplays;
    for (const auto& param : params) {
        paramDisplays.push_back({param.display, param.value});
    }
    auto result = this->ui.init(plots, paramDisplays);
    emit this->iface.initializationCompleted(result);
}


void
AppMain::module_msg(const std::string& message, bool append)
{
    this->ui.setMessage(QString::fromStdString(message), append);
}


void
AppMain::module_datafile(const exa::DatafileConfig& config, bool prompt)
{
    this->promptBeforeRun = prompt;
    emit this->dmConfigure(config);
}


void
AppMain::module_plot2D(std::size_t plotIdx, double x, double y, bool write)
{
    auto plot = this->ui.plot(plotIdx);
    plot->plot2D()->addData(x, y);
    plot->queue();
    if (write)
        emit this->dmWrite2D(plotIdx, x, y);
}


void
AppMain::module_plot2DVec(std::size_t plotIdx, const std::vector<double>& x, const std::vector<double>& y, bool write)
{
    auto plot = this->ui.plot(plotIdx);
    plot->plot2D()->addData({x.cbegin(), x.cend()}, {y.cbegin(), y.cend()});
    plot->queue();
    if (write)
        emit this->dmWrite2DVec(plotIdx, x, y);
}


void
AppMain::module_plotCM(std::size_t plotIdx, int x, int y, double value, bool write)
{
    auto plot = this->ui.plot(plotIdx);
    plot->plotColorMap()->setCell(x, y, value);
    plot->queue();
    if (write)
        emit this->dmWriteCM(plotIdx, x, y, value);
}


void
AppMain::module_plotCMVec(std::size_t plotIdx, int y, const std::vector<double>& values, bool write)
{
    auto plot = this->ui.plot(plotIdx);
    auto x_end = plot->plotColorMap()->getDataSizeX() < static_cast<int>(values.size())
        ? plot->plotColorMap()->getDataSizeX()
        : static_cast<int>(values.size());
    for (int x = 0; x < x_end; ++x)
        plot->plotColorMap()->setCell(x, y, values[x]);
    plot->queue();
    if (write)
        emit this->dmWriteCMVec(plotIdx, y, values);
}


void
AppMain::module_plotCMFrame(std::size_t plotIdx, const std::vector<std::vector<double>>& frame, bool write)
{
    auto plot = this->ui.plot(plotIdx);
    int y = 0;
    for (const auto& row : frame) {
        auto x_end = plot->plotColorMap()->getDataSizeX() < static_cast<int>(row.size())
            ? plot->plotColorMap()->getDataSizeX()
            : static_cast<int>(row.size());
        for (int x = 0; x < x_end; ++x)
            plot->plotColorMap()->setCell(x, y, row[x]);
        y += 1;
    }
    plot->queue();
    if (write)
        emit this->dmWriteCMFrame(plotIdx, frame);
}


void
AppMain::module_clear(std::size_t plotIdx)
{
    auto plot = this->ui.plot(plotIdx);
    switch (plot->type()) {
    default:
    case QPlot::Type::TWODIMEN: return plot->plot2D()->clear();
    case QPlot::Type::COLORMAP: return plot->plotColorMap()->clear();
    }
}


void
AppMain::module_setPlotProperty(
    std::size_t plotIdx,
    const exa::PlotProperty& property,
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
    this->promptBeforeRun = false;
    this->ui.reset();
}


void
AppMain::scriptError(const QString& message, const QString& title)
{
    this->ui.displayError(message, title);
}
