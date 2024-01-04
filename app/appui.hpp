#pragma once

#include <QObject>
#include <QString>

#include "mainwindow.hpp"
#include "about.hpp"
#include "ploteditor.hpp"


class AppUI : public QObject
{
    Q_OBJECT

public:
    AppUI(QObject* parent);
    ~AppUI();

    void show();
    void displayError(const QString&);

private:
    MainWindow* mainWindow;
    About* aboutDialog;
    PlotEditor* plotEditorDialog;
};
