#pragma once

#include <QAction>
#include <QTimer>

#include "ui_mainwindow.h"
#include "ploteditor.hpp"
#include "qplot.hpp"

#include <utility>
#include <vector>


class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    MainWindow();

    bool close();
    QAction* actionLoad();
    QAction* actionAbout();
    QAction* actionPlotEditor();
    QPushButton* buttonRun();
    void setPlots(const std::vector<PlotEditor::PlotInfo>&);
    void setMessage(const QString&);
    void initArgs(const std::vector<std::pair<std::string, std::string>>&);
    std::vector<std::string> scriptArgs() const;
    void setScriptStatus(const QString&);
    QPlot* plot(std::size_t);
    std::size_t plotCount() const;
    void enableRun(bool);
    void enableStop(bool);

Q_SIGNALS:
    void closed();
    void plotsSet(const std::vector<PlotEditor::PlotInfo>&);

protected:
    void closeEvent(QCloseEvent*) override;

private Q_SLOTS:
    void redraw();

private:
    Ui::MainWindow m_ui;
    QTimer m_timer;
    std::vector<QPlot*> m_plots;
    bool m_programmaticClose;
};
