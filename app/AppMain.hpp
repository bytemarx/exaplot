#pragma once

#include <QtWidgets/QApplication>
#include <QObject>
#include <QString>

#include "AppUI.hpp"
#include "orbital.hpp"


class Interface : public orbital::OrbitalInterface, public QObject
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
        emit this->signal_init();
        Py_RETURN_NONE;
    }

    PyObject* msg(const std::string& message, bool append) const
    {
        emit this->signal_msg();
        Py_RETURN_NONE;
    }

    PyObject* plot(long dataSet, const std::vector<double>& data) const
    {
        emit this->signal_plot();
        Py_RETURN_NONE;
    }

    PyObject* plotVec(long dataSet, const std::vector<std::vector<double>>& data) const
    {
        emit this->signal_plotVec();
        Py_RETURN_NONE;
    }

    PyObject* clear(long dataSet) const
    {
        emit this->signal_clear();
        Py_RETURN_NONE;
    }

Q_SIGNALS:
    void signal_init() const;
    void signal_msg() const;
    void signal_plot() const;
    void signal_plotVec() const;
    void signal_clear() const;
};


class AppMain : public QObject
{
    Q_OBJECT

public:
    AppMain(int& argc, char* argv[]);
    ~AppMain();

    int exec();

public Q_SLOTS:
    void slot_init();
    void slot_msg();
    void slot_plot();
    void slot_plotVec();
    void slot_clear();

private:
    QApplication a;
    AppUI ui;
    Interface interface;
};