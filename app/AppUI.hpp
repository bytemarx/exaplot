#pragma once

#include <QObject>
#include <QString>

#include "MainWindow.hpp"
#include "About.hpp"


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
};