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
    std::map<std::string, std::string> scriptArgs() const;
    void setMessage(const QString&);
    void initArgs(const std::vector<std::string>&);
    void setScriptStatus(const QString& = "");
    QPlot* plot(std::size_t);
    std::size_t plotCount() const;
    void enableRun(bool);
    void enableStop(bool);
    void setPlotProperty(std::size_t, const orbital::PlotProperty&, const QPlotTab::Cache&);

public Q_SLOTS:
    void displayError(const QString&, const QString& = "ERROR");

Q_SIGNALS:
    void closed();
    void scriptLoad(const QString&);
    void scriptRun(const std::map<std::string, std::string>&);
    void plotsSet(const std::vector<PlotEditor::PlotInfo>&);

private Q_SLOTS:
    void loadScript();

private:
    void setPlots(const std::vector<PlotEditor::PlotInfo>&);

    MainWindow* mainWindow;
    About* aboutDialog;
    PlotEditor* plotEditorDialog;
};
