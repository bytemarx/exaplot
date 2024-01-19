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

    PyObject* init(
        const std::vector<std::string>& params,
        const std::vector<orbital::GridPoint>& plots) const;
    PyObject* msg(const std::string& message, bool append) const;
    PyObject* plot(long dataSet, const std::vector<double>& data) const;
    PyObject* plotVec(long dataSet, const std::vector<std::vector<double>>& data) const;
    PyObject* clear(long dataSet) const;

Q_SIGNALS:
    void fatalError(int);
    void runErrored(const QString&, const QString&);
    void runCompleted();
    void module_init(const std::vector<std::string>&, const std::vector<orbital::GridPoint>&) const;
    void module_msg(const std::string&, bool) const;
    void module_plot(long, const std::vector<double>&) const;
    void module_plotVec(long, const std::vector<std::vector<double>>&) const;
    void module_clear(long) const;

public Q_SLOTS:
    void pythonInit();
    void pythonDeInit();
    void loadScript(const QString&);
    void runScript(const std::map<std::string, std::string>&);

private:
    QMutex mutex;
    orbital::OrbitalCore* core;
    std::shared_ptr<orbital::ScriptModule> module;
};


class AppMain : public QObject
{
    Q_OBJECT

public:
    AppMain(int& argc, char* argv[], Interface* iface, QThread* ifaceThread);
    ~AppMain();

    int exec();

Q_SIGNALS:
    void scriptLoaded(const QString&);
    void scriptRan(const std::map<std::string, std::string>&);

public Q_SLOTS:
    void shutdown(int = 0);
    void load(const QString&);
    void run(const std::map<std::string, std::string>&);
    void runError(const QString&, const QString&);
    void runComplete();
    void module_init(const std::vector<std::string>&, const std::vector<orbital::GridPoint>&);
    void module_msg(const std::string&, bool);
    void module_plot(long, const std::vector<double>&);
    void module_plotVec(long, const std::vector<std::vector<double>>&);
    void module_clear(long);

private:
    QApplication a;
    AppUI ui;
    QThread* ifaceThread;
    bool scriptRunning;
};
