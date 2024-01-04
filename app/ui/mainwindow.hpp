#pragma once

#include <QAction>

#include "ui_mainwindow.h"


class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    MainWindow();

    QAction* actionAbout();
    QAction* actionPlotEditor();

private:
    Ui::MainWindow ui;
};
