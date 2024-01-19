#pragma once

#include <QAction>
#include <QTimer>

#include "ui_mainwindow.h"
#include "ploteditor.hpp"
#include "qplot.hpp"

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
    void initArgs(const std::vector<std::string>&);
    void setScriptStatus(const QString&);
    QPlot* plot(std::size_t);
    std::size_t plotCount() const;

Q_SIGNALS:
    void closed();

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
