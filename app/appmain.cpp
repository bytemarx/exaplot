#include "appmain.hpp"
#include "config.h"

#include <iostream>


Interface::Interface(QObject* parent)
    : QObject{parent}
    , core{nullptr}
{
}


PyObject*
Interface::init(
    const std::vector<std::string>& params,
    const std::vector<orbital::GridPoint>& plots) const
{
    emit this->signal_init(params, plots);
    Py_RETURN_NONE;
}


PyObject*
Interface::msg(const std::string& message, bool append) const
{
    emit this->signal_msg(message, append);
    Py_RETURN_NONE;
}


PyObject*
Interface::plot(long dataSet, const std::vector<double>& data) const
{
    emit this->signal_plot(dataSet, data);
    Py_RETURN_NONE;
}


PyObject*
Interface::plotVec(long dataSet, const std::vector<std::vector<double>>& data) const
{
    emit this->signal_plotVec(dataSet, data);
    Py_RETURN_NONE;
}


PyObject*
Interface::clear(long dataSet) const
{
    emit this->signal_clear(dataSet);
    Py_RETURN_NONE;
}


void
Interface::pythonInit()
{
    std::filesystem::path prefix{ORBITAL_LIBRARY_PATH "/python"};

    // TODO: For uninstalled applications, find a better way to locate library
    if (!std::filesystem::exists(prefix))
        prefix = std::filesystem::canonical("/proc/self/exe").parent_path() / "python";

    PyStatus status = orbital::OrbitalCore::init(
        std::filesystem::canonical("/proc/self/exe"),
        prefix
    );
    if (PyStatus_Exception(status)) {
        if (PyStatus_IsError(status))
            std::cerr << "FATAL PYTHON INITIALIZATION ERROR:\n" << status.func << ": " << status.err_msg << '\n';
        emit this->fatalError(status.exitcode);
        return;
    }

    this->core = new orbital::OrbitalCore{this};
}


void
Interface::pythonDeInit()
{
    delete this->core;
    orbital::OrbitalCore::deinit();
}


void
Interface::loadScript(const QString& file)
{
    assert(this->core != nullptr);
    QMutexLocker locker{&this->mutex};
    std::cerr << "Loading: " << file.toStdString() << '\n';
    auto status = this->core->load(std::filesystem::path{file.toStdString()}, this->module);
    if (status) {
        auto message = status.message();
        if (!status.traceback().empty())
            message.append("\n").append(status.traceback());
        emit this->signal_error(message.c_str(), QString{"ERROR::"}.append(status.type()));
    }
}


void
Interface::runScript(const std::map<std::string, std::string>& kwargs)
{
    QMutexLocker locker{&this->mutex};
    auto status = this->module.get()->run(kwargs);
    if (status) {
        auto message = status.message();
        if (!status.traceback().empty())
            message.append("\n").append(status.traceback());
        emit this->signal_error(message.c_str(), QString{"ERROR::"}.append(status.type()));
    }
}


AppMain::AppMain(int& argc, char* argv[], Interface* iface, QThread* ifaceThread)
    : QObject{Q_NULLPTR}
    , a{argc, argv}
    , ui{this}
    , ifaceThread{ifaceThread}
{
    QObject::connect(&this->a, &QApplication::aboutToQuit, [this]() { this->shutdown(0); });
    QObject::connect(&this->ui, &AppUI::scriptLoad, iface, &Interface::loadScript, Qt::QueuedConnection);
    QObject::connect(&this->ui, &AppUI::scriptRun, iface, &Interface::runScript, Qt::QueuedConnection);
    QObject::connect(iface, &Interface::fatalError, this, &AppMain::shutdown, Qt::QueuedConnection);
    QObject::connect(iface, &Interface::signal_init, this, &AppMain::slot_init, Qt::QueuedConnection);
    QObject::connect(iface, &Interface::signal_msg, this, &AppMain::slot_msg, Qt::QueuedConnection);
    QObject::connect(iface, &Interface::signal_plot, this, &AppMain::slot_plot, Qt::QueuedConnection);
    QObject::connect(iface, &Interface::signal_plotVec, this, &AppMain::slot_plotVec, Qt::QueuedConnection);
    QObject::connect(iface, &Interface::signal_clear, this, &AppMain::slot_clear, Qt::QueuedConnection);
    QObject::connect(iface, &Interface::signal_error, this, &AppMain::slot_error, Qt::QueuedConnection);
}


AppMain::~AppMain()
{
}


int
AppMain::exec()
{
    this->ui.show();
    return this->a.exec();
}


void
AppMain::shutdown(int status)
{
    ifaceThread->quit();
    ifaceThread->wait();
    this->a.exit(status);
}


void
AppMain::slot_init(
    const std::vector<std::string>& params,
    const std::vector<orbital::GridPoint>& plots)
{
    this->ui.initArgs(params);
}


void
AppMain::slot_msg(const std::string& message, bool append)
{
    this->ui.setMessage(QString::fromStdString(message));
}


void
AppMain::slot_plot(long dataSet, const std::vector<double>& data)
{
    auto n = static_cast<std::size_t>(dataSet);
    if (n > this->ui.plotCount())
        return this->ui.displayError("Invalid data set");
    if (n == 0) return;
    auto plot = this->ui.plot(n - 1);
    if (data.size() >= 2)
        plot->plot2D()->addData(data[0], data[1]);
    plot->queue();
}


void
AppMain::slot_plotVec(long dataSet, const std::vector<std::vector<double>>& data)
{
}


void
AppMain::slot_clear(long dataSet)
{
}


void
AppMain::slot_error(const QString& message, const QString& title)
{
    this->ui.displayError(message, title);
}
