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
    void displayError(const QString&, const QString& = "ERROR");
    std::map<std::string, std::string> scriptArgs() const;
    void setMessage(const QString&);

Q_SIGNALS:
    void scriptLoaded(const QString&);

private Q_SLOTS:
    void loadScript();

private:
    MainWindow* mainWindow;
    About* aboutDialog;
    PlotEditor* plotEditorDialog;
};
