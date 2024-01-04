#include "mainwindow.hpp"


MainWindow::MainWindow()
    : QMainWindow{nullptr}
{
    this->ui.setupUi(this);
}


QAction*
MainWindow::actionAbout()
{
    return this->ui.actionAbout;
}


QAction*
MainWindow::actionPlotEditor()
{
    return this->ui.actionPlotEditor;
}
