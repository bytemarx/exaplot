#pragma once

#include <QAction>

#include "ui_mainwindow.h"
#include "ploteditor.hpp"
#include "qplot.hpp"

#include <vector>


class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    MainWindow();

    QAction* actionLoad();
    QAction* actionAbout();
    QAction* actionPlotEditor();
    QPushButton* buttonRun();
    void setPlots(const std::vector<PlotEditor::PlotInfo>&);
    void setMessage(const QString&);
    void initArgs(const std::vector<std::string>&);
    QPlot* plot(std::size_t);
    std::size_t plotCount() const;

private:
    Ui::MainWindow ui;
    std::vector<QPlot*> m_plots;
};
