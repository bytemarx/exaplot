/*
 * ZetaPlot
 * app-module interface
 * 
 * SPDX-License-Identifier: GPL-3.0
 * Copyright (C) 2024 bytemarx
 */

#pragma once

#include <QMutex>
#include <QMutexLocker>
#include <QObject>
#include <QString>

#include "zetaplot.hpp"
#include "ploteditor.hpp"
#include "qplot.hpp"
#include "qplottab.hpp"


class Interface : public QObject, public zeta::Interface
{
    Q_OBJECT

public:
    Interface(QObject* parent = nullptr);

    PyObject* init(const std::vector<zeta::RunParam>& params, const std::vector<zeta::GridPoint>& plots) override;
    PyObject* stop() override;
    PyObject* msg(const std::string& message, bool append) override;
    PyObject* plot2D(std::size_t plotID, double x, double y) override;
    PyObject* plot2DVec(std::size_t plotID, const std::vector<double>& x, const std::vector<double>& y) override;
    PyObject* plotCM(std::size_t plotID, int col, int row, double value) override;
    PyObject* plotCMVec(std::size_t plotID, int row, const std::vector<double>& values) override;
    PyObject* plotCMFrame(std::size_t plotID, const std::vector<std::vector<double>>& frame) override;
    PyObject* clear(std::size_t plotID) override;
    PyObject* setPlotProperty(std::size_t plotID, const zeta::PlotProperty& property, const zeta::PlotProperty::Value& value) override;
    PyObject* getPlotProperty(std::size_t plotID, const zeta::PlotProperty& property) override;
    PyObject* showPlot(std::size_t plotID, std::size_t plotType) override;
    Py_ssize_t currentPlotType(std::size_t plotID) override;

Q_SIGNALS:
    void fatalError(int);
    void scriptErrored(const QString&, const QString&);
    void initializationCompleted(bool);
    void runCompleted(const QString&);
    void module_init(const std::vector<zeta::RunParam>&, const std::vector<zeta::GridPoint>&) const;
    void module_msg(const std::string&, bool) const;
    void module_plot2D(std::size_t plotIdx, double, double) const;
    void module_plot2DVec(std::size_t plotIdx, const std::vector<double>&, const std::vector<double>&) const;
    void module_plotCM(std::size_t plotIdx, int, int, double) const;
    void module_plotCMVec(std::size_t plotIdx, int, const std::vector<double>&) const;
    void module_plotCMFrame(std::size_t plotIdx, const std::vector<std::vector<double>>&) const;
    void module_clear(std::size_t plotIdx) const;
    void module_setPlotProperty(std::size_t plotIdx, const zeta::PlotProperty&, const QPlotTab::Cache&) const;
    void module_showPlot(std::size_t plotIdx, QPlot::Type);

public Q_SLOTS:
    void pythonInit();
    void pythonDeInit();
    void loadScript(const QString&);
    void runScript(const std::vector<std::string>&);
    void requestStop();
    void updatePlotProperties(const std::vector<PlotEditor::PlotInfo>&);

private:
    QMutex mutex;
    zeta::Core* core;
    std::shared_ptr<zeta::ScriptModule> module;
    std::vector<PlotEditor::PlotInfo> plots;
    std::vector<zeta::RunParam> params;
    bool stopRequested;
};
