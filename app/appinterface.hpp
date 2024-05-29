/*
 * ExaPlot
 * app-module interface
 * 
 * SPDX-License-Identifier: GPL-3.0
 * Copyright (C) 2024 bytemarx
 */

#pragma once

#include <QMutex>
#include <QObject>
#include <QString>

#include "exaplot.hpp"
#include "ploteditor.hpp"
#include "qplot.hpp"
#include "qplottab.hpp"

#include <atomic>


class Interface : public QObject, public exa::Interface
{
    Q_OBJECT

public:
    Interface(const std::vector<std::filesystem::path>& searchPaths, QObject* parent = nullptr);

    void setError(bool);

    PyObject* init(const std::vector<exa::RunParam>& params, const std::vector<exa::GridPoint>& plots) override;
    PyObject* stop() override;
    PyObject* msg(const std::string& message, bool append) override;
    PyObject* datafile(const exa::DatafileConfig& config, PyObject* path, bool prompt) override;
    PyObject* plot2D(std::size_t plotID, double x, double y, bool write) override;
    PyObject* plot2DVec(std::size_t plotID, const std::vector<double>& x, const std::vector<double>& y, bool write) override;
    PyObject* plotCM(std::size_t plotID, int col, int row, double value, bool write) override;
    PyObject* plotCMVec(std::size_t plotID, int row, const std::vector<double>& values, bool write) override;
    PyObject* plotCMFrame(std::size_t plotID, const std::vector<std::vector<double>>& frame, bool write) override;
    PyObject* clear(std::size_t plotID) override;
    PyObject* setPlotProperty(std::size_t plotID, const exa::PlotProperty& property, const exa::PlotProperty::Value& value) override;
    PyObject* getPlotProperty(std::size_t plotID, const exa::PlotProperty& property) override;
    PyObject* showPlot(std::size_t plotID, std::size_t plotType) override;
    Py_ssize_t currentPlotType(std::size_t plotID) override;

Q_SIGNALS:
    void fatalError(int);
    void scriptErrored(const QString&, const QString&);
    void initializationCompleted(bool);
    void initializeDatafile(std::filesystem::path);
    void datafileInitializationCompleted(bool);
    void scriptStatusUpdated(const QString&);
    void runCompleted(const QString&);
    void module_init(const std::vector<exa::RunParam>&, const std::vector<exa::GridPoint>&) const;
    void module_msg(const std::string&, bool) const;
    void module_datafile(const exa::DatafileConfig& config, bool prompt) const;
    void module_plot2D(std::size_t plotIdx, double, double, bool) const;
    void module_plot2DVec(std::size_t plotIdx, const std::vector<double>&, const std::vector<double>&, bool) const;
    void module_plotCM(std::size_t plotIdx, int, int, double, bool) const;
    void module_plotCMVec(std::size_t plotIdx, int, const std::vector<double>&, bool) const;
    void module_plotCMFrame(std::size_t plotIdx, const std::vector<std::vector<double>>&, bool) const;
    void module_clear(std::size_t plotIdx) const;
    void module_setPlotProperty(std::size_t plotIdx, const exa::PlotProperty&, const QPlotTab::Cache&) const;
    void module_showPlot(std::size_t plotIdx, QPlot::Type);

public Q_SLOTS:
    void pythonInit();
    void pythonDeInit();
    void loadScript(const QString&);
    void runScript(const std::vector<std::string>&);
    void requestStop();
    void updatePlotProperties(const std::vector<PlotEditor::PlotInfo>&);

private:
    void initDatafileAndRun(const std::vector<exa::RunParam> &args);

    std::atomic_bool error;
    QMutex mutex;
    const std::vector<std::filesystem::path> searchPaths;
    exa::Core* core;
    bool scriptRunning;
    bool stopRequested;
    std::shared_ptr<exa::ScriptModule> module;
    std::vector<PlotEditor::PlotInfo> plots;
    std::vector<exa::RunParam> params;
};
