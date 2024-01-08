#pragma once

#include <QtWidgets/QApplication>
#include <QObject>
#include <QString>

#include "appui.hpp"
#include "orbital.hpp"


class Interface : public QObject, public orbital::OrbitalInterface
{
    Q_OBJECT

public:
    Interface(QObject* parent) : QObject{parent}
    {
    }

    PyObject* init(
        const std::vector<std::string>& params,
        const std::vector<orbital::GridPoint>& plots) const
    {
        emit this->signal_init(params, plots);
        Py_RETURN_NONE;
    }

    PyObject* msg(const std::string& message, bool append) const
    {
        emit this->signal_msg(message, append);
        Py_RETURN_NONE;
    }

    PyObject* plot(long dataSet, const std::vector<double>& data) const
    {
        emit this->signal_plot(dataSet, data);
        Py_RETURN_NONE;
    }

    PyObject* plotVec(long dataSet, const std::vector<std::vector<double>>& data) const
    {
        emit this->signal_plotVec(dataSet, data);
        Py_RETURN_NONE;
    }

    PyObject* clear(long dataSet) const
    {
        emit this->signal_clear(dataSet);
        Py_RETURN_NONE;
    }

Q_SIGNALS:
    void signal_init(const std::vector<std::string>&, const std::vector<orbital::GridPoint>&) const;
    void signal_msg(const std::string&, bool) const;
    void signal_plot(long, const std::vector<double>&) const;
    void signal_plotVec(long, const std::vector<std::vector<double>>&) const;
    void signal_clear(long) const;
};


class AppMain : public QObject
{
    Q_OBJECT

public:
    AppMain(int& argc, char* argv[]);
    ~AppMain();

    int exec();

public Q_SLOTS:
    void load(const QString&);
    void run();
    void slot_init(const std::vector<std::string>&, const std::vector<orbital::GridPoint>&);
    void slot_msg(const std::string&, bool);
    void slot_plot(long, const std::vector<double>&);
    void slot_plotVec(long, const std::vector<std::vector<double>>&);
    void slot_clear(long);

private:
    QApplication a;
    AppUI ui;
    Interface interface;
    orbital::OrbitalCore core;
    std::shared_ptr<orbital::ScriptModule> module;
};
