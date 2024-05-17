/*
 * ExaPlot
 * app main component
 * 
 * SPDX-License-Identifier: GPL-3.0
 * Copyright (C) 2024 bytemarx
 */

#pragma once

#include <QtWidgets/QApplication>
#include <QObject>
#include <QString>
#include <QThread>

#include "appinterface.hpp"
#include "appui.hpp"
#include "datamanager.hpp"


struct Config
{
    Config();

    const std::vector<std::filesystem::path>& searchPaths() const { return this->m_searchPaths; }

private:
    std::vector<std::filesystem::path> m_searchPaths;
};


class AppMain : public QObject
{
    Q_OBJECT

public:
    AppMain(int& argc, char* argv[], const Config&);
    ~AppMain();

    int exec();

Q_SIGNALS:
    void setError(bool);
    void scriptLoaded(const QString&);
    void scriptRan(const std::vector<std::string>&);
    void scriptStopped();

public Q_SLOTS:
    void shutdown(int = 0);
    void load(const QString&);
    void run(const std::vector<std::string>&);
    void stop();
    void scriptError(const QString&, const QString&);
    void runComplete(const QString&);
    void module_init(const std::vector<exa::RunParam>&, const std::vector<exa::GridPoint>&);
    void module_msg(const std::string&, bool);
    void module_plot2D(std::size_t plotIdx, double, double, bool);
    void module_plot2DVec(std::size_t plotIdx, const std::vector<double>&, const std::vector<double>&, bool);
    void module_plotCM(std::size_t plotIdx, int, int, double, bool);
    void module_plotCMVec(std::size_t plotIdx, int, const std::vector<double>&, bool);
    void module_plotCMFrame(std::size_t plotIdx, const std::vector<std::vector<double>>&, bool);
    void module_clear(std::size_t plotIdx);
    void module_setPlotProperty(std::size_t plotIdx, const exa::PlotProperty&, const QPlotTab::Cache&);
    void module_showPlot(std::size_t plotIdx, QPlot::Type);

private:
    void reset();

    QThread ifaceThread;
    Interface iface;
    QApplication a;
    AppUI ui;
    DataManager dm;
    bool scriptRunning;
};
