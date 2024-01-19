#include <QMessageBox>

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
    emit this->module_init(params, plots);
    Py_RETURN_NONE;
}


PyObject*
Interface::msg(const std::string& message, bool append) const
{
    emit this->module_msg(message, append);
    Py_RETURN_NONE;
}


PyObject*
Interface::plot(long dataSet, const std::vector<double>& data) const
{
    emit this->module_plot(dataSet, data);
    Py_RETURN_NONE;
}


PyObject*
Interface::plotVec(long dataSet, const std::vector<std::vector<double>>& data) const
{
    emit this->module_plotVec(dataSet, data);
    Py_RETURN_NONE;
}


PyObject*
Interface::clear(long dataSet) const
{
    emit this->module_clear(dataSet);
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
        emit this->runErrored(message.c_str(), QString{"ERROR::"}.append(status.type()));
    }
}


void
Interface::runScript(const std::map<std::string, std::string>& kwargs)
{
    if (!this->module) return;
    QMutexLocker locker{&this->mutex};
    auto status = this->module.get()->run(kwargs);
    if (status) {
        auto message = status.message();
        if (!status.traceback().empty())
            message.append("\n").append(status.traceback());
        emit this->runErrored(message.c_str(), QString{"ERROR::"}.append(status.type()));
    }
    emit this->runCompleted();
}


AppMain::AppMain(int& argc, char* argv[], Interface* iface, QThread* ifaceThread)
    : QObject{Q_NULLPTR}
    , a{argc, argv}
    , ui{this}
    , ifaceThread{ifaceThread}
    , scriptRunning{false}
{
    QObject::connect(&this->a, &QApplication::aboutToQuit, [this] { this->shutdown(0); });
    QObject::connect(&this->ui, &AppUI::closed, [this] { this->shutdown(0); });
    QObject::connect(&this->ui, &AppUI::scriptLoad, this, &AppMain::load);
    QObject::connect(&this->ui, &AppUI::scriptRun, this, &AppMain::run);
    QObject::connect(this, &AppMain::scriptLoaded, iface, &Interface::loadScript, Qt::QueuedConnection);
    QObject::connect(this, &AppMain::scriptRan, iface, &Interface::runScript, Qt::QueuedConnection);
    QObject::connect(iface, &Interface::fatalError, this, &AppMain::shutdown, Qt::QueuedConnection);
    QObject::connect(iface, &Interface::runErrored, this, &AppMain::runError, Qt::QueuedConnection);
    QObject::connect(iface, &Interface::runCompleted, this, &AppMain::runComplete, Qt::QueuedConnection);
    QObject::connect(iface, &Interface::module_init, this, &AppMain::module_init, Qt::QueuedConnection);
    QObject::connect(iface, &Interface::module_msg, this, &AppMain::module_msg, Qt::QueuedConnection);
    QObject::connect(iface, &Interface::module_plot, this, &AppMain::module_plot, Qt::QueuedConnection);
    QObject::connect(iface, &Interface::module_plotVec, this, &AppMain::module_plotVec, Qt::QueuedConnection);
    QObject::connect(iface, &Interface::module_clear, this, &AppMain::module_clear, Qt::QueuedConnection);
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
    if (this->scriptRunning) {
        QMessageBox msgBox{
            QMessageBox::Warning,
            "",
            "Script is running. Force quit?",
            QMessageBox::Yes | QMessageBox::No
        };
        switch (msgBox.exec()) {
        case QMessageBox::Yes:
            this->ifaceThread->terminate();
            this->scriptRunning = false;
            break;
        default:
            return;
        }
    } else {
        this->ifaceThread->quit();
        this->ifaceThread->wait();
    }
    this->ui.close();
    this->a.exit(status);
}


void
AppMain::load(const QString& file)
{
    this->ui.setScriptStatus();
    emit this->scriptLoaded(file);
}


void
AppMain::run(const std::map<std::string, std::string>& kwargs)
{
    this->ui.setScriptStatus("Running...");
    emit this->scriptRan(kwargs);
    this->scriptRunning = true;
}


void
AppMain::runComplete()
{
    this->scriptRunning = false;
    this->ui.setScriptStatus("Completed");
}


void
AppMain::module_init(
    const std::vector<std::string>& params,
    const std::vector<orbital::GridPoint>& plots)
{
    this->ui.initArgs(params);
}


void
AppMain::module_msg(const std::string& message, bool append)
{
    this->ui.setMessage(QString::fromStdString(message));
}


void
AppMain::module_plot(long dataSet, const std::vector<double>& data)
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
AppMain::module_plotVec(long dataSet, const std::vector<std::vector<double>>& data)
{
}


void
AppMain::module_clear(long dataSet)
{
}


void
AppMain::runError(const QString& message, const QString& title)
{
    this->ui.displayError(message, title);
}
