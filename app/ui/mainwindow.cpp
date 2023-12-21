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
