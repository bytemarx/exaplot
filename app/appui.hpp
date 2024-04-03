/*
 * ExaPlot
 * app ui component
 * 
 * SPDX-License-Identifier: GPL-3.0
 * Copyright (C) 2024 bytemarx
 */

#pragma once

#include <QObject>
#include <QString>

#include "mainwindow.hpp"
#include "about.hpp"
#include "ploteditor.hpp"

#include <map>
#include <string>


class AppUI : public QObject
{
    Q_OBJECT

public:
    AppUI(QObject* parent);
    ~AppUI();

    void show();
    bool close();
    void reset();
    void clear();
    std::vector<std::string> scriptArgs() const;
    void setMessage(const QString&, bool);
    bool init(const std::vector<exa::GridPoint>&, const std::vector<std::pair<std::string, std::string>>&);
    void setScriptStatus(const QString& = "");
    QPlot* plot(std::size_t);
    std::size_t plotCount() const;
    void enableRun(bool);
    void enableStop(bool);
    void setPlotProperty(std::size_t, const exa::PlotProperty&, const QPlotTab::Cache&);
    void showPlot(std::size_t, QPlot::Type);

public Q_SLOTS:
    void displayError(const QString&, const QString& = "ERROR");

Q_SIGNALS:
    void closed();
    void scriptLoad(const QString&);
    void scriptRun(const std::vector<std::string>&);
    void scriptStop();
    void plotsSet(const std::vector<PlotEditor::PlotInfo>&);

private Q_SLOTS:
    void loadScript();

private:
    void setPlots(const std::vector<PlotEditor::PlotInfo>&);

    MainWindow* mainWindow;
    About* aboutDialog;
    PlotEditor* plotEditorDialog;
};
