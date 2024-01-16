#pragma once

#include <QtWidgets/QApplication>
#include <QMutex>
#include <QMutexLocker>
#include <QObject>
#include <QThread>
#include <QString>

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
    void signal_init(const std::vector<std::string>&, const std::vector<orbital::GridPoint>&) const;
    void signal_msg(const std::string&, bool) const;
    void signal_plot(long, const std::vector<double>&) const;
    void signal_plotVec(long, const std::vector<std::vector<double>>&) const;
    void signal_clear(long) const;
    void signal_error(const QString&, const QString&) const;
    void runCompleted() const;

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

public Q_SLOTS:
    void shutdown(int = 0);
    void slot_init(const std::vector<std::string>&, const std::vector<orbital::GridPoint>&);
    void slot_msg(const std::string&, bool);
    void slot_plot(long, const std::vector<double>&);
    void slot_plotVec(long, const std::vector<std::vector<double>>&);
    void slot_clear(long);
    void slot_error(const QString&, const QString&);

private:
    QApplication a;
    AppUI ui;
    QThread* ifaceThread;
};
