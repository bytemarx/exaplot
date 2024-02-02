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
    if (this->plots.size() < static_cast<std::size_t>(dataSet)) {
        PyErr_SetString(PyExc_IndexError, ORBITAL_PLOT " index out of range");
        return NULL;
    }
    emit this->module_plot(dataSet, data);
    Py_RETURN_NONE;
}


PyObject*
Interface::plotVec(long dataSet, const std::vector<std::vector<double>>& data) const
{
    if (this->plots.size() < static_cast<std::size_t>(dataSet)) {
        PyErr_SetString(PyExc_IndexError, ORBITAL_PLOT " index out of range");
        return NULL;
    }
    emit this->module_plotVec(dataSet, data);
    Py_RETURN_NONE;
}


PyObject*
Interface::clear(long dataSet) const
{
    if (this->plots.size() < static_cast<std::size_t>(dataSet)) {
        PyErr_SetString(PyExc_IndexError, ORBITAL_PLOT " index out of range");
        return NULL;
    }
    emit this->module_clear(dataSet);
    Py_RETURN_NONE;
}


PyObject*
Interface::setPlotProperty(
    long plotID,
    const std::string& property,
    const std::variant<int, double, std::string>& value) const
{
    if (plotID < 1) {
        PyErr_SetString(PyExc_IndexError, "Invalid plot index");
        return NULL;
    }
    auto i = static_cast<std::size_t>(plotID - 1);
    if (i >= this->plots.size()) {
        PyErr_SetString(PyExc_IndexError, "Plot index out of range");
        return NULL;
    }

    Py_RETURN_NONE;
}


PyObject*
Interface::getPlotProperty(long plotID, const std::string& property) const
{
    if (plotID < 1) {
        PyErr_SetString(PyExc_IndexError, "Invalid plot index");
        return NULL;
    }
    auto i = static_cast<std::size_t>(plotID - 1);
    if (i >= this->plots.size()) {
        PyErr_SetString(PyExc_IndexError, "Plot index out of range");
        return NULL;
    }

    const auto& attributes = this->plots.at(i).attributes;
    PyObject* pyOwned_value = NULL;

    std::cout << "get: " << attributes.title.toStdString() << '\n';
    if (property.compare(ORBITAL_PLOT_PROPERTY_TITLE) == 0) {
        std::cout << "hi\n";
        pyOwned_value = PyUnicode_FromString(attributes.title.toStdString().c_str());
    } else if (property.compare(ORBITAL_PLOT_PROPERTY_XAXIS) == 0) {
        pyOwned_value = PyUnicode_FromString(attributes.xAxis.toStdString().c_str());
    } else if (property.compare(ORBITAL_PLOT_PROPERTY_YAXIS) == 0) {
        pyOwned_value = PyUnicode_FromString(attributes.yAxis.toStdString().c_str());
    } else if (property.compare(ORBITAL_PLOT_PROPERTY_MINSIZE_W) == 0) {
        pyOwned_value = PyLong_FromLong(static_cast<long>(attributes.minSize.width));
    } else if (property.compare(ORBITAL_PLOT_PROPERTY_MINSIZE_H) == 0) {
        pyOwned_value = PyLong_FromLong(static_cast<long>(attributes.minSize.height));
    } else if (property.compare(ORBITAL_PLOT_PROPERTY_TWODIMEN_XRANGE_MIN) == 0) {
        pyOwned_value = PyFloat_FromDouble(attributes.twoDimen.xRange.min);
    } else if (property.compare(ORBITAL_PLOT_PROPERTY_TWODIMEN_XRANGE_MAX) == 0) {
        pyOwned_value = PyFloat_FromDouble(attributes.twoDimen.xRange.max);
    } else if (property.compare(ORBITAL_PLOT_PROPERTY_TWODIMEN_YRANGE_MIN) == 0) {
        pyOwned_value = PyFloat_FromDouble(attributes.twoDimen.yRange.min);
    } else if (property.compare(ORBITAL_PLOT_PROPERTY_TWODIMEN_YRANGE_MAX) == 0) {
        pyOwned_value = PyFloat_FromDouble(attributes.twoDimen.yRange.max);
    } else if (property.compare(ORBITAL_PLOT_PROPERTY_TWODIMEN_LINE_TYPE) == 0) {
        switch (attributes.twoDimen.line.type) {
            case QCPGraph::LineStyle::lsNone: pyOwned_value = PyUnicode_FromString("none"); break;
            case QCPGraph::LineStyle::lsLine: pyOwned_value = PyUnicode_FromString("line"); break;
            case QCPGraph::LineStyle::lsStepLeft: pyOwned_value = PyUnicode_FromString("step-left"); break;
            case QCPGraph::LineStyle::lsStepRight: pyOwned_value = PyUnicode_FromString("step-right"); break;
            case QCPGraph::LineStyle::lsStepCenter: pyOwned_value = PyUnicode_FromString("step-center"); break;
            case QCPGraph::LineStyle::lsImpulse: pyOwned_value = PyUnicode_FromString("impulse"); break;
            default: pyOwned_value = Py_None;
        }
    } else if (property.compare(ORBITAL_PLOT_PROPERTY_TWODIMEN_LINE_COLOR) == 0) {
        pyOwned_value = PyUnicode_FromString(attributes.twoDimen.line.color.name().toStdString().c_str());
    } else if (property.compare(ORBITAL_PLOT_PROPERTY_TWODIMEN_LINE_STYLE) == 0) {
        switch (attributes.twoDimen.line.style) {
            case Qt::PenStyle::SolidLine: pyOwned_value = PyUnicode_FromString("solid"); break;
            case Qt::PenStyle::DashLine: pyOwned_value = PyUnicode_FromString("dashed"); break;
            case Qt::PenStyle::DotLine: pyOwned_value = PyUnicode_FromString("dotted"); break;
            case Qt::PenStyle::DashDotLine: pyOwned_value = PyUnicode_FromString("dash-dotted"); break;
            case Qt::PenStyle::DashDotDotLine: pyOwned_value = PyUnicode_FromString("dash-double-dotted"); break;
            default: pyOwned_value = Py_None;
        }
    } else if (property.compare(ORBITAL_PLOT_PROPERTY_TWODIMEN_POINTS_SHAPE) == 0) {
        switch (attributes.twoDimen.points.shape) {
            case QCPScatterStyle::ScatterShape::ssNone: pyOwned_value = PyUnicode_FromString("none"); break;
            case QCPScatterStyle::ScatterShape::ssDot: pyOwned_value = PyUnicode_FromString("dot"); break;
            case QCPScatterStyle::ScatterShape::ssCross: pyOwned_value = PyUnicode_FromString("cross"); break;
            case QCPScatterStyle::ScatterShape::ssPlus: pyOwned_value = PyUnicode_FromString("plus"); break;
            case QCPScatterStyle::ScatterShape::ssCircle: pyOwned_value = PyUnicode_FromString("circle"); break;
            case QCPScatterStyle::ScatterShape::ssDisc: pyOwned_value = PyUnicode_FromString("disc"); break;
            case QCPScatterStyle::ScatterShape::ssSquare: pyOwned_value = PyUnicode_FromString("square"); break;
            case QCPScatterStyle::ScatterShape::ssDiamond: pyOwned_value = PyUnicode_FromString("diamond"); break;
            case QCPScatterStyle::ScatterShape::ssStar: pyOwned_value = PyUnicode_FromString("star"); break;
            case QCPScatterStyle::ScatterShape::ssTriangle: pyOwned_value = PyUnicode_FromString("triangle"); break;
            case QCPScatterStyle::ScatterShape::ssTriangleInverted: pyOwned_value = PyUnicode_FromString("triangle-inverted"); break;
            case QCPScatterStyle::ScatterShape::ssCrossSquare: pyOwned_value = PyUnicode_FromString("cross-square"); break;
            case QCPScatterStyle::ScatterShape::ssPlusSquare: pyOwned_value = PyUnicode_FromString("plus-square"); break;
            case QCPScatterStyle::ScatterShape::ssCrossCircle: pyOwned_value = PyUnicode_FromString("cross-circle"); break;
            case QCPScatterStyle::ScatterShape::ssPlusCircle: pyOwned_value = PyUnicode_FromString("plus-circle"); break;
            case QCPScatterStyle::ScatterShape::ssPeace: pyOwned_value = PyUnicode_FromString("peace"); break;
            default: pyOwned_value = Py_None;
        }
    } else if (property.compare(ORBITAL_PLOT_PROPERTY_TWODIMEN_POINTS_COLOR) == 0) {
        pyOwned_value = PyUnicode_FromString(attributes.twoDimen.points.color.name().toStdString().c_str());
    } else if (property.compare(ORBITAL_PLOT_PROPERTY_TWODIMEN_POINTS_SIZE) == 0) {
        pyOwned_value = PyFloat_FromDouble(attributes.twoDimen.points.size);
    } else if (property.compare(ORBITAL_PLOT_PROPERTY_COLORMAP_XRANGE_MIN) == 0) {
        pyOwned_value = PyFloat_FromDouble(attributes.colorMap.xRange.min);
    } else if (property.compare(ORBITAL_PLOT_PROPERTY_COLORMAP_XRANGE_MAX) == 0) {
        pyOwned_value = PyFloat_FromDouble(attributes.colorMap.xRange.max);
    } else if (property.compare(ORBITAL_PLOT_PROPERTY_COLORMAP_YRANGE_MIN) == 0) {
        pyOwned_value = PyFloat_FromDouble(attributes.colorMap.yRange.min);
    } else if (property.compare(ORBITAL_PLOT_PROPERTY_COLORMAP_YRANGE_MAX) == 0) {
        pyOwned_value = PyFloat_FromDouble(attributes.colorMap.yRange.max);
    } else if (property.compare(ORBITAL_PLOT_PROPERTY_COLORMAP_ZRANGE_MIN) == 0) {
        pyOwned_value = PyFloat_FromDouble(attributes.colorMap.zRange.min);
    } else if (property.compare(ORBITAL_PLOT_PROPERTY_COLORMAP_ZRANGE_MAX) == 0) {
        pyOwned_value = PyFloat_FromDouble(attributes.colorMap.zRange.max);
    } else if (property.compare(ORBITAL_PLOT_PROPERTY_COLORMAP_DATASIZE_X) == 0) {
        pyOwned_value = PyLong_FromLong(static_cast<long>(attributes.colorMap.dataSize.x));
    } else if (property.compare(ORBITAL_PLOT_PROPERTY_COLORMAP_DATASIZE_Y) == 0) {
        pyOwned_value = PyLong_FromLong(static_cast<long>(attributes.colorMap.dataSize.y));
    } else if (property.compare(ORBITAL_PLOT_PROPERTY_COLORMAP_COLOR_MIN) == 0) {
        pyOwned_value = PyUnicode_FromString(attributes.colorMap.color.min.name().toStdString().c_str());
    } else if (property.compare(ORBITAL_PLOT_PROPERTY_COLORMAP_COLOR_MAX) == 0) {
        pyOwned_value = PyUnicode_FromString(attributes.colorMap.color.max.name().toStdString().c_str());
    } else {
        PyErr_Format(PyExc_KeyError, "Unknown property '%s'", property.c_str());
        return NULL;
    }

    return pyOwned_value;
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
        emit this->scriptErrored(message.c_str(), QString{"ERROR::"}.append(status.type()));
    }
}


void
Interface::runScript(const std::map<std::string, std::string>& kwargs)
{
    if (!this->module) {
        emit this->runCompleted("No script loaded");
        return;
    }
    QMutexLocker locker{&this->mutex};
    std::cerr << "kwargs = {";
    for (const auto& kwarg : kwargs)
        std::cerr << "\n  " << kwarg.first << ": " << kwarg.second;
    std::cerr << "\n}\n";
    auto status = this->module.get()->run(kwargs);
    if (status) {
        auto message = status.message();
        if (!status.traceback().empty())
            message.append("\n").append(status.traceback());
        emit this->scriptErrored(message.c_str(), QString{"ERROR::"}.append(status.type()));
    }
    emit this->runCompleted("Completed");
}


void
Interface::updatePlotProperties(const std::vector<PlotEditor::PlotInfo>& plots)
{
    this->plots = plots;
    std::cout << "updating: " << this->plots.at(0).attributes.title.toStdString() << '\n';
}


AppMain::AppMain(int& argc, char* argv[])
    : QObject{Q_NULLPTR}
    , ifaceThread{}
    , iface{}
    , a{argc, argv}
    , ui{this}
    , scriptRunning{false}
{
    QObject::connect(&this->ifaceThread, &QThread::started, &this->iface, &Interface::pythonInit);
    QObject::connect(&this->ifaceThread, &QThread::finished, &this->iface, &Interface::pythonDeInit);
    this->iface.moveToThread(&this->ifaceThread);
    this->ifaceThread.start();

    QObject::connect(&this->a, &QApplication::aboutToQuit, [this] { this->shutdown(0); });
    QObject::connect(&this->ui, &AppUI::closed, [this] { this->shutdown(0); });
    QObject::connect(&this->ui, &AppUI::scriptLoad, this, &AppMain::load);
    QObject::connect(&this->ui, &AppUI::scriptRun, this, &AppMain::run);
    QObject::connect(&this->ui, &AppUI::plotsSet, &this->iface, &Interface::updatePlotProperties, Qt::QueuedConnection);
    QObject::connect(this, &AppMain::scriptLoaded, &this->iface, &Interface::loadScript, Qt::QueuedConnection);
    QObject::connect(this, &AppMain::scriptRan, &this->iface, &Interface::runScript, Qt::QueuedConnection);
    QObject::connect(&this->iface, &Interface::fatalError, this, &AppMain::shutdown, Qt::QueuedConnection);
    QObject::connect(&this->iface, &Interface::scriptErrored, this, &AppMain::scriptError, Qt::QueuedConnection);
    QObject::connect(&this->iface, &Interface::runCompleted, this, &AppMain::runComplete, Qt::QueuedConnection);
    QObject::connect(&this->iface, &Interface::module_init, this, &AppMain::module_init, Qt::QueuedConnection);
    QObject::connect(&this->iface, &Interface::module_msg, this, &AppMain::module_msg, Qt::QueuedConnection);
    QObject::connect(&this->iface, &Interface::module_plot, this, &AppMain::module_plot, Qt::QueuedConnection);
    QObject::connect(&this->iface, &Interface::module_plotVec, this, &AppMain::module_plotVec, Qt::QueuedConnection);
    QObject::connect(&this->iface, &Interface::module_clear, this, &AppMain::module_clear, Qt::QueuedConnection);
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
    bool terminate = false;
    if (this->scriptRunning) {
        QMessageBox msgBox{
            QMessageBox::Warning,
            "",
            "Script is running. Force quit?",
            QMessageBox::Yes | QMessageBox::No
        };
        switch (msgBox.exec()) {
            default: return;
            case QMessageBox::Yes: terminate = true;
        }
    }
    if (this->scriptRunning && terminate) {
        this->ifaceThread.terminate();
        this->scriptRunning = false;
    } else {
        this->ifaceThread.quit();
        this->ifaceThread.wait();
    }
    this->ui.close();
    this->a.exit(status);
}


void
AppMain::load(const QString& file)
{
    if (this->scriptRunning) {
        this->scriptError(
            "Cannot load new script while current script is running.",
            "Load Error"
        );
        return;
    }
    this->ui.setScriptStatus();
    this->reset();
    emit this->scriptLoaded(file);
}


void
AppMain::run(const std::map<std::string, std::string>& kwargs)
{
    if (this->scriptRunning)
        return;
    this->ui.enableRun(false);
    this->ui.enableStop(true);
    this->ui.clear();
    this->ui.setScriptStatus("Running...");
    emit this->scriptRan(kwargs);
    this->scriptRunning = true;
}


void
AppMain::runComplete(const QString& message)
{
    this->scriptRunning = false;
    this->ui.setScriptStatus(message);
    this->ui.enableStop(false);
    this->ui.enableRun(true);
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
AppMain::reset()
{
    this->ui.reset();
}


void
AppMain::scriptError(const QString& message, const QString& title)
{
    this->ui.displayError(message, title);
}
