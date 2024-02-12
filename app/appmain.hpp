#pragma once

#include <QtWidgets/QApplication>
#include <QMutex>
#include <QMutexLocker>
#include <QObject>
#include <QString>
#include <QThread>

#include "appui.hpp"
#include "orbital.hpp"


class Interface : public QObject, public orbital::OrbitalInterface
{
    Q_OBJECT

public:
    Interface(QObject* parent = nullptr);

    PyObject* init(const std::vector<std::string>& params, const std::vector<orbital::GridPoint>& plots) override;
    PyObject* msg(const std::string& message, bool append) override;
    PyObject* plot(long dataSet, const std::vector<double>& data) override;
    PyObject* plotVec(long dataSet, const std::vector<std::vector<double>>& data) override;
    PyObject* clear(long dataSet) override;
    PyObject* setPlotProperty(long plotID, const orbital::PlotProperty& property, const orbital::PlotProperty::Value& value) override;
    PyObject* getPlotProperty(long plotID, const orbital::PlotProperty& property) override;
    PyObject* showPlot(long plotID, std::size_t plotType) override;

Q_SIGNALS:
    void fatalError(int);
    void scriptErrored(const QString&, const QString&);
    void runCompleted(const QString&);
    void module_init(const std::vector<std::string>&, const std::vector<orbital::GridPoint>&) const;
    void module_msg(const std::string&, bool) const;
    void module_plot(long, const std::vector<double>&) const;
    void module_plotVec(long, const std::vector<std::vector<double>>&) const;
    void module_clear(long) const;
    void module_setPlotProperty(std::size_t plotIdx, const orbital::PlotProperty& property, const QPlotTab::Cache& properties) const;
    void module_showPlot(std::size_t plotIdx, QPlot::Type plotType);

public Q_SLOTS:
    void pythonInit();
    void pythonDeInit();
    void loadScript(const QString&);
    void runScript(const std::map<std::string, std::string>&);
    void updatePlotProperties(const std::vector<PlotEditor::PlotInfo>&);

private:
    QMutex mutex;
    orbital::OrbitalCore* core;
    std::shared_ptr<orbital::ScriptModule> module;
    std::vector<PlotEditor::PlotInfo> plots;
};


class AppMain : public QObject
{
    Q_OBJECT

public:
    AppMain(int& argc, char* argv[]);
    ~AppMain();

    int exec();

Q_SIGNALS:
    void scriptLoaded(const QString&);
    void scriptRan(const std::map<std::string, std::string>&);

public Q_SLOTS:
    void shutdown(int = 0);
    void load(const QString&);
    void run(const std::map<std::string, std::string>&);
    void scriptError(const QString&, const QString&);
    void runComplete(const QString&);
    void module_init(const std::vector<std::string>&, const std::vector<orbital::GridPoint>&);
    void module_msg(const std::string&, bool);
    void module_plot(long, const std::vector<double>&);
    void module_plotVec(long, const std::vector<std::vector<double>>&);
    void module_clear(long);
    void module_setPlotProperty(std::size_t, const orbital::PlotProperty&, const QPlotTab::Cache&);
    void module_showPlot(std::size_t, QPlot::Type plotType);

private:
    void reset();

    QThread ifaceThread;
    Interface iface;
    QApplication a;
    AppUI ui;
    bool scriptRunning;
};
