#include "appinterface.hpp"
#include "config.h"

#include <iostream>


Interface::Interface(QObject* parent)
    : QObject{parent}
    , core{nullptr}
{
}


PyObject*
Interface::init(
    const std::vector<orbital::RunParam>& params,
    const std::vector<orbital::GridPoint>& plots)
{
    this->params = params;
    emit this->module_init(params, plots);
    Py_RETURN_NONE;
}


PyObject*
Interface::msg(const std::string& message, bool append)
{
    emit this->module_msg(message, append);
    Py_RETURN_NONE;
}


PyObject*
Interface::plot2D(std::size_t plotID, double x, double y)
{
    emit this->module_plot2D(plotID - 1, x, y);
    Py_RETURN_NONE;
}


PyObject*
Interface::plot2DVec(std::size_t plotID, const std::vector<double>& x, const std::vector<double>& y)
{
    emit this->module_plot2DVec(plotID - 1, x, y);
    Py_RETURN_NONE;
}


PyObject*
Interface::plotCM(std::size_t plotID, int col, int row, double value)
{
    auto plot = this->plots.at(plotID - 1);
    if (col >= plot.attributes.colorMap.dataSize.x) {
        PyErr_SetString(PyExc_ValueError, ORBITAL_PLOT "() 'col' argument out of bounds");
        return NULL;
    }
    if (row >= plot.attributes.colorMap.dataSize.y) {
        PyErr_SetString(PyExc_ValueError, ORBITAL_PLOT "() 'row' argument out of bounds");
        return NULL;
    }
    emit this->module_plotCM(plotID - 1, col, row, value);
    Py_RETURN_NONE;
}


PyObject*
Interface::plotCMVec(std::size_t plotID, int row, const std::vector<double>& values)
{
    auto plot = this->plots.at(plotID - 1);
    if (row >= plot.attributes.colorMap.dataSize.y) {
        PyErr_SetString(PyExc_ValueError, ORBITAL_PLOT "() 'row' argument out of bounds");
        return NULL;
    }
    if (values.size() > static_cast<std::size_t>(plot.attributes.colorMap.dataSize.x)) {
        PyErr_SetString(PyExc_ValueError, ORBITAL_PLOT "() 'values' argument contains too many values");
        return NULL;
    }
    emit this->module_plotCMVec(plotID - 1, row, values);
    Py_RETURN_NONE;
}


PyObject*
Interface::plotCMFrame(std::size_t plotID, const std::vector<std::vector<double>>& frame)
{
    auto plot = this->plots.at(plotID - 1);
    if (frame.size() > static_cast<std::size_t>(plot.attributes.colorMap.dataSize.y)) {
        PyErr_SetString(PyExc_ValueError, ORBITAL_PLOT "() 'frame' argument contains too many rows");
        return NULL;
    }
    std::size_t i = 0;
    for (const auto& row : frame) {
        if (row.size() > static_cast<std::size_t>(plot.attributes.colorMap.dataSize.x)) {
            PyErr_Format(PyExc_ValueError, ORBITAL_PLOT "() frame[%zd] contains too many values", i);
            return NULL;
        }
        i += 1;
    }
    emit this->module_plotCMFrame(plotID - 1, frame);
    Py_RETURN_NONE;
}


PyObject*
Interface::clear(std::size_t plotID)
{
    auto plotIdx = plotID - 1;
    if (plotIdx >= this->plots.size()) {
        PyErr_SetString(PyExc_IndexError, "plot ID out of range");
        return NULL;
    }
    emit this->module_clear(plotIdx);
    Py_RETURN_NONE;
}


PyObject*
Interface::setPlotProperty(
    std::size_t plotID,
    const orbital::PlotProperty& property,
    const orbital::PlotProperty::Value& value)
{
    if (plotID == 0) {
        PyErr_SetString(PyExc_IndexError, "invalid plot ID");
        return NULL;
    }
    auto plotIdx = plotID - 1;
    if (plotIdx >= this->plots.size()) {
        PyErr_SetString(PyExc_IndexError, "plot ID out of range");
        return NULL;
    }

    auto& properties = this->plots.at(plotIdx).attributes;
    using PlotProperty = orbital::PlotProperty;
    switch (property) {
    case PlotProperty::TITLE: properties.title = QString::fromStdString(std::get<std::string>(value)); break;
    case PlotProperty::XAXIS: properties.xAxis = QString::fromStdString(std::get<std::string>(value)); break;
    case PlotProperty::YAXIS: properties.yAxis = QString::fromStdString(std::get<std::string>(value)); break;
    case PlotProperty::MINSIZE_W: properties.minSize.width = std::get<int>(value); break;
    case PlotProperty::MINSIZE_H: properties.minSize.height = std::get<int>(value); break;
    case PlotProperty::TWODIMEN_XRANGE_MIN: properties.twoDimen.xRange.min = std::get<double>(value); break;
    case PlotProperty::TWODIMEN_XRANGE_MAX: properties.twoDimen.xRange.max = std::get<double>(value); break;
    case PlotProperty::TWODIMEN_YRANGE_MIN: properties.twoDimen.yRange.min = std::get<double>(value); break;
    case PlotProperty::TWODIMEN_YRANGE_MAX: properties.twoDimen.yRange.max = std::get<double>(value); break;
    case PlotProperty::TWODIMEN_LINE_TYPE:
        if (std::get<std::string>(value).compare("none") == 0) {
            properties.twoDimen.line.type = QCPGraph::LineStyle::lsNone;
        } else if (std::get<std::string>(value).compare("line") == 0) {
            properties.twoDimen.line.type = QCPGraph::LineStyle::lsLine;
        } else if (std::get<std::string>(value).compare("step-left") == 0) {
            properties.twoDimen.line.type = QCPGraph::LineStyle::lsStepLeft;
        } else if (std::get<std::string>(value).compare("step-right") == 0) {
            properties.twoDimen.line.type = QCPGraph::LineStyle::lsStepRight;
        } else if (std::get<std::string>(value).compare("step-center") == 0) {
            properties.twoDimen.line.type = QCPGraph::LineStyle::lsStepCenter;
        } else if (std::get<std::string>(value).compare("impulse") == 0) {
            properties.twoDimen.line.type = QCPGraph::LineStyle::lsImpulse;
        } else {
            PyErr_Format(PyExc_ValueError, "invalid line type: %s", std::get<std::string>(value).c_str());
            return NULL;
        }
        break;
    case PlotProperty::TWODIMEN_LINE_COLOR:
        {
            QColor color{QString::fromStdString(std::get<std::string>(value))};
            if (!color.isValid()) {
                PyErr_Format(PyExc_ValueError, "invalid color: %s", std::get<std::string>(value).c_str());
                return NULL;
            }
            properties.twoDimen.line.color = color;
        }
        break;
    case PlotProperty::TWODIMEN_LINE_STYLE:
        if (std::get<std::string>(value).compare("solid") == 0) {
            properties.twoDimen.line.style = Qt::PenStyle::SolidLine;
        } else if (std::get<std::string>(value).compare("dashed") == 0) {
            properties.twoDimen.line.style = Qt::PenStyle::DashLine;
        } else if (std::get<std::string>(value).compare("dotted") == 0) {
            properties.twoDimen.line.style = Qt::PenStyle::DotLine;
        } else if (std::get<std::string>(value).compare("dash-dotted") == 0) {
            properties.twoDimen.line.style = Qt::PenStyle::DashDotLine;
        } else if (std::get<std::string>(value).compare("dash-double-dotted") == 0) {
            properties.twoDimen.line.style = Qt::PenStyle::DashDotDotLine;
        } else {
            PyErr_Format(PyExc_ValueError, "invalid line style: %s", std::get<std::string>(value).c_str());
            return NULL;
        }
        break;
    case PlotProperty::TWODIMEN_POINTS_SHAPE:
        if (std::get<std::string>(value).compare("none") == 0) {
            properties.twoDimen.points.shape = QCPScatterStyle::ScatterShape::ssNone;
        } else if (std::get<std::string>(value).compare("dot") == 0) {
            properties.twoDimen.points.shape = QCPScatterStyle::ScatterShape::ssDot;
        } else if (std::get<std::string>(value).compare("cross") == 0) {
            properties.twoDimen.points.shape = QCPScatterStyle::ScatterShape::ssCross;
        } else if (std::get<std::string>(value).compare("plus") == 0) {
            properties.twoDimen.points.shape = QCPScatterStyle::ScatterShape::ssPlus;
        } else if (std::get<std::string>(value).compare("circle") == 0) {
            properties.twoDimen.points.shape = QCPScatterStyle::ScatterShape::ssCircle;
        } else if (std::get<std::string>(value).compare("disc") == 0) {
            properties.twoDimen.points.shape = QCPScatterStyle::ScatterShape::ssDisc;
        } else if (std::get<std::string>(value).compare("square") == 0) {
            properties.twoDimen.points.shape = QCPScatterStyle::ScatterShape::ssSquare;
        } else if (std::get<std::string>(value).compare("diamond") == 0) {
            properties.twoDimen.points.shape = QCPScatterStyle::ScatterShape::ssDiamond;
        } else if (std::get<std::string>(value).compare("star") == 0) {
            properties.twoDimen.points.shape = QCPScatterStyle::ScatterShape::ssStar;
        } else if (std::get<std::string>(value).compare("triangle") == 0) {
            properties.twoDimen.points.shape = QCPScatterStyle::ScatterShape::ssTriangle;
        } else if (std::get<std::string>(value).compare("triangle-inverted") == 0) {
            properties.twoDimen.points.shape = QCPScatterStyle::ScatterShape::ssTriangleInverted;
        } else if (std::get<std::string>(value).compare("cross-square") == 0) {
            properties.twoDimen.points.shape = QCPScatterStyle::ScatterShape::ssCrossSquare;
        } else if (std::get<std::string>(value).compare("plus-square") == 0) {
            properties.twoDimen.points.shape = QCPScatterStyle::ScatterShape::ssPlusSquare;
        } else if (std::get<std::string>(value).compare("cross-circle") == 0) {
            properties.twoDimen.points.shape = QCPScatterStyle::ScatterShape::ssCrossCircle;
        } else if (std::get<std::string>(value).compare("plus-circle") == 0) {
            properties.twoDimen.points.shape = QCPScatterStyle::ScatterShape::ssPlusCircle;
        } else if (std::get<std::string>(value).compare("peace") == 0) {
            properties.twoDimen.points.shape = QCPScatterStyle::ScatterShape::ssPeace;
        } else {
            PyErr_Format(PyExc_ValueError, "invalid shape: %s", std::get<std::string>(value).c_str());
            return NULL;
        }
        break;
    case PlotProperty::TWODIMEN_POINTS_COLOR:
        {
            QColor color{QString::fromStdString(std::get<std::string>(value))};
            if (!color.isValid()) {
                PyErr_Format(PyExc_ValueError, "invalid color: %s", std::get<std::string>(value).c_str());
                return NULL;
            }
            properties.twoDimen.points.color = color;
        }
        break;
    case PlotProperty::TWODIMEN_POINTS_SIZE: properties.twoDimen.points.size = std::get<double>(value); break;
    case PlotProperty::COLORMAP_XRANGE_MIN: properties.colorMap.xRange.min = std::get<double>(value); break;
    case PlotProperty::COLORMAP_XRANGE_MAX: properties.colorMap.xRange.max = std::get<double>(value); break;
    case PlotProperty::COLORMAP_YRANGE_MIN: properties.colorMap.yRange.min = std::get<double>(value); break;
    case PlotProperty::COLORMAP_YRANGE_MAX: properties.colorMap.yRange.max = std::get<double>(value); break;
    case PlotProperty::COLORMAP_ZRANGE_MIN: properties.colorMap.zRange.min = std::get<double>(value); break;
    case PlotProperty::COLORMAP_ZRANGE_MAX: properties.colorMap.zRange.max = std::get<double>(value); break;
    case PlotProperty::COLORMAP_DATASIZE_X: properties.colorMap.dataSize.x = std::get<int>(value); break;
    case PlotProperty::COLORMAP_DATASIZE_Y: properties.colorMap.dataSize.y = std::get<int>(value); break;
    case PlotProperty::COLORMAP_COLOR_MIN:
        {
            QColor color{QString::fromStdString(std::get<std::string>(value))};
            if (!color.isValid()) {
                PyErr_Format(PyExc_ValueError, "invalid color: %s", std::get<std::string>(value).c_str());
                return NULL;
            }
            properties.colorMap.color.min = color;
        }
        break;
    case PlotProperty::COLORMAP_COLOR_MAX:
        {
            QColor color{QString::fromStdString(std::get<std::string>(value))};
            if (!color.isValid()) {
                PyErr_Format(PyExc_ValueError, "invalid color: %s", std::get<std::string>(value).c_str());
                return NULL;
            }
            properties.colorMap.color.max = color;
        }
        break;
    default:
        PyErr_Format(PyExc_KeyError, "invalid property '%s'", property.c_str());
        return NULL;
    }
    emit this->module_setPlotProperty(plotIdx, property, properties);
    Py_RETURN_NONE;
}


PyObject*
Interface::getPlotProperty(std::size_t plotID, const orbital::PlotProperty& property)
{
    if (plotID == 0) {
        PyErr_SetString(PyExc_IndexError, "invalid plot ID");
        return NULL;
    }
    auto plotIdx = plotID - 1;
    if (plotIdx >= this->plots.size()) {
        PyErr_SetString(PyExc_IndexError, "plot ID out of range");
        return NULL;
    }

    const auto& attributes = this->plots.at(plotIdx).attributes;
    PyObject* pyOwned_value = NULL;

    using PlotProperty = orbital::PlotProperty;
    switch (property) {
    case PlotProperty::TITLE: pyOwned_value = PyUnicode_FromString(attributes.title.toStdString().c_str()); break;
    case PlotProperty::XAXIS: pyOwned_value = PyUnicode_FromString(attributes.xAxis.toStdString().c_str()); break;
    case PlotProperty::YAXIS: pyOwned_value = PyUnicode_FromString(attributes.yAxis.toStdString().c_str()); break;
    case PlotProperty::MINSIZE_W: pyOwned_value = PyLong_FromLong(static_cast<long>(attributes.minSize.width)); break;
    case PlotProperty::MINSIZE_H: pyOwned_value = PyLong_FromLong(static_cast<long>(attributes.minSize.height)); break;
    case PlotProperty::TWODIMEN_XRANGE_MIN: pyOwned_value = PyFloat_FromDouble(attributes.twoDimen.xRange.min); break;
    case PlotProperty::TWODIMEN_XRANGE_MAX: pyOwned_value = PyFloat_FromDouble(attributes.twoDimen.xRange.max); break;
    case PlotProperty::TWODIMEN_YRANGE_MIN: pyOwned_value = PyFloat_FromDouble(attributes.twoDimen.yRange.min); break;
    case PlotProperty::TWODIMEN_YRANGE_MAX: pyOwned_value = PyFloat_FromDouble(attributes.twoDimen.yRange.max); break;
    case PlotProperty::TWODIMEN_LINE_TYPE:
        switch (attributes.twoDimen.line.type) {
        case QCPGraph::LineStyle::lsNone: pyOwned_value = PyUnicode_FromString("none"); break;
        case QCPGraph::LineStyle::lsLine: pyOwned_value = PyUnicode_FromString("line"); break;
        case QCPGraph::LineStyle::lsStepLeft: pyOwned_value = PyUnicode_FromString("step-left"); break;
        case QCPGraph::LineStyle::lsStepRight: pyOwned_value = PyUnicode_FromString("step-right"); break;
        case QCPGraph::LineStyle::lsStepCenter: pyOwned_value = PyUnicode_FromString("step-center"); break;
        case QCPGraph::LineStyle::lsImpulse: pyOwned_value = PyUnicode_FromString("impulse"); break;
        default: pyOwned_value = Py_None;
        }
        break;
    case PlotProperty::TWODIMEN_LINE_COLOR: pyOwned_value = PyUnicode_FromString(attributes.twoDimen.line.color.name().toStdString().c_str()); break;
    case PlotProperty::TWODIMEN_LINE_STYLE:
        switch (attributes.twoDimen.line.style) {
        case Qt::PenStyle::SolidLine: pyOwned_value = PyUnicode_FromString("solid"); break;
        case Qt::PenStyle::DashLine: pyOwned_value = PyUnicode_FromString("dashed"); break;
        case Qt::PenStyle::DotLine: pyOwned_value = PyUnicode_FromString("dotted"); break;
        case Qt::PenStyle::DashDotLine: pyOwned_value = PyUnicode_FromString("dash-dotted"); break;
        case Qt::PenStyle::DashDotDotLine: pyOwned_value = PyUnicode_FromString("dash-double-dotted"); break;
        default: pyOwned_value = Py_None;
        }
        break;
    case PlotProperty::TWODIMEN_POINTS_SHAPE:
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
        break;
    case PlotProperty::TWODIMEN_POINTS_COLOR: pyOwned_value = PyUnicode_FromString(attributes.twoDimen.points.color.name().toStdString().c_str()); break;
    case PlotProperty::TWODIMEN_POINTS_SIZE: pyOwned_value = PyFloat_FromDouble(attributes.twoDimen.points.size); break;
    case PlotProperty::COLORMAP_XRANGE_MIN: pyOwned_value = PyFloat_FromDouble(attributes.colorMap.xRange.min); break;
    case PlotProperty::COLORMAP_XRANGE_MAX: pyOwned_value = PyFloat_FromDouble(attributes.colorMap.xRange.max); break;
    case PlotProperty::COLORMAP_YRANGE_MIN: pyOwned_value = PyFloat_FromDouble(attributes.colorMap.yRange.min); break;
    case PlotProperty::COLORMAP_YRANGE_MAX: pyOwned_value = PyFloat_FromDouble(attributes.colorMap.yRange.max); break;
    case PlotProperty::COLORMAP_ZRANGE_MIN: pyOwned_value = PyFloat_FromDouble(attributes.colorMap.zRange.min); break;
    case PlotProperty::COLORMAP_ZRANGE_MAX: pyOwned_value = PyFloat_FromDouble(attributes.colorMap.zRange.max); break;
    case PlotProperty::COLORMAP_DATASIZE_X: pyOwned_value = PyLong_FromLong(static_cast<long>(attributes.colorMap.dataSize.x)); break;
    case PlotProperty::COLORMAP_DATASIZE_Y: pyOwned_value = PyLong_FromLong(static_cast<long>(attributes.colorMap.dataSize.y)); break;
    case PlotProperty::COLORMAP_COLOR_MIN: pyOwned_value = PyUnicode_FromString(attributes.colorMap.color.min.name().toStdString().c_str()); break;
    case PlotProperty::COLORMAP_COLOR_MAX: pyOwned_value = PyUnicode_FromString(attributes.colorMap.color.max.name().toStdString().c_str()); break;
    default:
        PyErr_Format(PyExc_KeyError, "invalid property '%s'", property.c_str());
        return NULL;
    }

    return pyOwned_value;
}


PyObject*
Interface::showPlot(std::size_t plotID, std::size_t plotType)
{
    if (plotID == 0) {
        PyErr_SetString(PyExc_IndexError, "invalid plot ID");
        return NULL;
    }
    auto plotIdx = plotID - 1;
    if (plotIdx >= this->plots.size()) {
        PyErr_SetString(PyExc_IndexError, "plot ID out of range");
        return NULL;
    }

    auto& plot = this->plots.at(plotIdx);
    switch (plotType) {
    case 0: plot.selected = QPlot::Type::TWODIMEN; break;
    case 1: plot.selected = QPlot::Type::COLORMAP; break;
    default:
        PyErr_Format(PyExc_SystemError, "invalid plot type: %zu", plotType);
        return NULL;
    }
    emit this->module_showPlot(plotIdx, plot.selected);
    Py_RETURN_NONE;
}


Py_ssize_t
Interface::currentPlotType(std::size_t plotID)
{
    if (plotID == 0) {
        PyErr_SetString(PyExc_IndexError, "invalid plot ID");
        return -1;
    }
    auto plotIdx = plotID - 1;
    if (plotIdx >= this->plots.size()) {
        PyErr_SetString(PyExc_IndexError, "plot ID out of range");
        return -1;
    }
    return static_cast<Py_ssize_t>(this->plots.at(plotIdx).selected);
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
Interface::runScript(const std::vector<std::string>& args)
{
    if (!this->module) {
        emit this->runCompleted("No script loaded");
        return;
    }
    QMutexLocker locker{&this->mutex};
    assert(args.size() == this->params.size());
    auto params = this->params;
    for (std::size_t i = 0; i < args.size(); ++i)
        params[i].value = args[i];
    auto status = this->module.get()->run(params);
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
}
