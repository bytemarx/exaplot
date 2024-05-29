/*
 * ExaPlot
 * module implementation
 * 
 * SPDX-License-Identifier: GPL-3.0
 * Copyright (C) 2024 bytemarx
 */

#include "internal.hpp"

#include <cstring>
#include <functional>
#include <limits>


namespace exa {


static exa_state*
getModuleState(PyObject* module)
{
    return static_cast<exa_state*>(PyModule_GetState(module));
}


static int
PyRunParam_Check(PyObject* object)
{
    auto mState = getModuleStateFromObject(object);
    if (mState == NULL)
        return 0;

    return PyObject_TypeCheck(object, mState->type_RunParam);
}


extern "C" {


int
moduleSlot_initInterface(PyObject* module)
{
    auto mState = getModuleState(module);
    auto tState = PyThreadState_Get();
    auto interp = tState->interp;

    assert(interp->passthrough);
    if (interp->passthrough == NULL) {
        goto error;
    }

    mState->iface = static_cast<Interface*>(interp->passthrough);

    return 0;
error:
    return -1;
}


int
module_traverse(PyObject* module, visitproc visit, void* arg)
{
    auto mState = getModuleState(module);
    Py_VISIT(mState->type_RunParam);
    Py_VISIT(mState->obj_InterruptException);
    return 0;
}


int
module_clear(PyObject* module)
{
    auto mState = getModuleState(module);
    Py_CLEAR(mState->type_RunParam);
    Py_CLEAR(mState->obj_InterruptException);
    return 0;
}


/**
 * @brief Module `init` function
 * 
 * @param module 
 * @param args 
 * @param nargs 
 * @param kwnames 
 * @return PyObject* 
 */
PyObject*
exa_init(PyObject* module, PyObject* const* args, Py_ssize_t nargs, PyObject* kwnames)
{
    std::vector<RunParam> params;
    std::vector<GridPoint> plots{{.x=0, .dx=0, .y=0, .dy=0}};

    if (nargs > 1) {
        PyErr_Format(PyExc_TypeError,
                     EXA_INIT "() takes from 0 to 1 positional arguments but %zd were given", nargs);
        return NULL;
    }

    // positional argument 'plots' given
    // - clear current plots
    // - determine overload (num of plots or plot arrangement)
    // - parse
    // - convert to std::vector<GridPoint>
    if (nargs == 1) {
        plots.clear();
        auto pyBorrowed_plots = args[0];
        if (PyLong_Check(pyBorrowed_plots)) {
            auto n_plots = PyLong_AS_LONG(pyBorrowed_plots);
            if (n_plots <= 0) {
                if (!PyErr_Occurred())
                    PyErr_SetString(PyExc_ValueError,
                                    EXA_INIT "() 'plots' argument must be an integer greater than zero");
                return NULL;
            }
            for (decltype(n_plots) i_plot = 0; i_plot < n_plots; ++i_plot) {
                plots.push_back({
                    .x = 0,
                    .dx = 0,
                    .y = static_cast<GridPoint_t>(i_plot),
                    .dy = 0,
                });
            }
        } else if (PyList_Check(pyBorrowed_plots)) {
            auto n_plots = PyList_GET_SIZE(pyBorrowed_plots);
            if (n_plots == 0) {
                PyErr_SetString(PyExc_ValueError, EXA_INIT "() plots list is missing entries");
                return NULL;
            }
            for (decltype(n_plots) i_plot = 0; i_plot < n_plots; ++i_plot) {
                auto pyBorrowed_plot = PyList_GET_ITEM(pyBorrowed_plots, i_plot);
                if (!PyTuple_Check(pyBorrowed_plot) || PyTuple_GET_SIZE(pyBorrowed_plot) != 4) {
                    PyErr_Format(PyExc_TypeError,
                                 EXA_INIT "() 'plots[%zd]' value must be type 'tuple[int, int, int, int]'",
                                 i_plot);
                    return NULL;
                }
                long p[4];
                for (decltype(std::size(p)) i_plotPoint = 0; i_plotPoint < std::size(p); ++i_plotPoint) {
                    auto pyBorrowed_plotPoint = PyTuple_GET_ITEM(pyBorrowed_plot, i_plotPoint);
                    if (!PyLong_Check(pyBorrowed_plotPoint)) {
                        PyErr_Format(PyExc_TypeError,
                                     EXA_INIT "() 'plots[%zd][%zd]' value must be type 'int'",
                                     i_plot, i_plotPoint);
                        return NULL;
                    }
                    auto plotPoint = PyLong_AsLong(pyBorrowed_plotPoint);
                    if (plotPoint < 0) {
                        if (!PyErr_Occurred())
                            PyErr_Format(PyExc_ValueError,
                                         EXA_INIT "() 'plots[%zd][%zd]' value is invalid: %ld",
                                         i_plot, i_plotPoint, plotPoint);
                        return NULL;
                    }
                    p[i_plotPoint] = plotPoint;
                }
                plots.push_back({
                    .x = static_cast<GridPoint_t>(p[0]),
                    .dx = static_cast<GridPoint_t>(p[1]),
                    .y = static_cast<GridPoint_t>(p[2]),
                    .dy = static_cast<GridPoint_t>(p[3]),
                });
            }
        } else {
            PyErr_SetString(PyExc_TypeError,
                            EXA_INIT "() 'plots' argument must be either an 'int' or 'list' type");
            return NULL;
        }
    }

    if (kwnames == NULL)
        goto done;

    // parse script parameters
    // - iterate over keywords
    // - determine type
    // - parse
    // - convert to std::vector<RunParam>
    for (decltype(PyTuple_GET_SIZE(kwnames)) i = 0; i < PyTuple_GET_SIZE(kwnames); ++i) {
        auto pyBorrowed_paramName = PyTuple_GET_ITEM(kwnames, i);
        auto pyBorrowed_paramInfo = args[nargs + i];

        auto identifier = PyUnicode_AsUTF8(pyBorrowed_paramName);
        if (identifier == NULL) return NULL;

        auto type = RunParamType::STRING;
        auto defaultValue = std::string{};
        auto display = std::string{identifier};

        if (PyRunParam_Check(pyBorrowed_paramInfo)) {
            auto pyBorrowed_runParam = reinterpret_cast<PyRunParam*>(pyBorrowed_paramInfo);

            type = pyBorrowed_runParam->type;
            if (pyBorrowed_runParam->value != NULL) {
                auto c_value = PyUnicode_AsUTF8(pyBorrowed_runParam->value);
                if (c_value == NULL) return NULL;
                defaultValue = c_value;
            }
            if (pyBorrowed_runParam->display != NULL) {
                auto c_display = PyUnicode_AsUTF8(pyBorrowed_runParam->display);
                if (c_display == NULL) return NULL;
                display = c_display;
            }
        } else if (PyUnicode_CheckExact(pyBorrowed_paramInfo)) {
            auto c_defaultValue = PyUnicode_AsUTF8(pyBorrowed_paramInfo);
            if (c_defaultValue == NULL) return NULL;
            defaultValue = c_defaultValue;
        } else {
            if (PyLong_CheckExact(pyBorrowed_paramInfo)) {
                type = RunParamType::INT;
            } else if (PyFloat_CheckExact(pyBorrowed_paramInfo)) {
                type = RunParamType::FLOAT;
            } else {
                PyErr_Format(PyExc_ValueError,
                             EXA_INIT "() invalid value for parameter '%U'", pyBorrowed_paramName);
                return NULL;
            }

            auto pyOwned_defaultValue = PyObject_Str(pyBorrowed_paramInfo);
            if (pyOwned_defaultValue == NULL) return NULL;
            auto c_defaultValue = PyUnicode_AsUTF8(pyOwned_defaultValue);
            if (c_defaultValue == NULL) {
                Py_DECREF(pyOwned_defaultValue);
                return NULL;
            }
            defaultValue = c_defaultValue;
            Py_DECREF(pyOwned_defaultValue);
        }

        params.push_back({
            .identifier = identifier,
            .type = type,
            .value = defaultValue,
            .display = display,
        });
    }

done:
    auto state = getModuleState(module);
    return state->iface->init(params, plots);
}


PyObject*
exa_stop(PyObject* module, [[maybe_unused]] PyObject* args)
{
    auto state = getModuleState(module);
    return state->iface->stop();
}


static char*
msg_keywords[] = {
    (char*)"",
    (char*)"append",
    NULL
};


/**
 * @brief Module `msg` function
 * 
 * @param module 
 * @param args 
 * @return PyObject* 
 */
PyObject*
exa_msg(PyObject* module, PyObject* args, PyObject* kwargs)
{
    const char* c_message = NULL;
    int c_append = 0;
    if (!PyArg_ParseTupleAndKeywords(args, kwargs, "s|p:" EXA_MSG, msg_keywords, &c_message, &c_append))
        return NULL;

    std::string message{c_message};
    bool append = c_append != 0;

    auto state = getModuleState(module);
    return state->iface->msg(message, append);
}


static char*
datafile_keywords[] = {
    (char*)"enable",
    (char*)"path",
    (char*)"prompt",
    NULL
};


PyObject*
exa_datafile(PyObject* module, PyObject* args, PyObject* kwargs)
{
    int c_enable = -1;
    PyObject* pyBorrowed_path = NULL;
    int c_prompt = 0;
    if (!PyArg_ParseTupleAndKeywords(
            args, kwargs, "|$pOp:" EXA_DATAFILE, datafile_keywords,
            &c_enable,
            &pyBorrowed_path,
            &c_prompt
        )) return NULL;

    DatafileConfig config;
    if (c_enable != -1)
        config.enable = c_enable != 0;

    auto state = getModuleState(module);
    return state->iface->datafile(config, pyBorrowed_path, c_prompt != 0);
}


static PyObject*
plot2D(
    exa_state* state,
    std::size_t plotID,
    PyObject* const* args,
    Py_ssize_t nargs,
    bool write)
{
    if (nargs != 2) {
        PyErr_Format(PyExc_TypeError, EXA_PLOT "() takes 2 positional arguments but %zd were given", nargs);
        return NULL;
    }
    auto x = PyFloat_AsDouble(args[0]);
    if (PyErr_Occurred()) return NULL;
    auto y = PyFloat_AsDouble(args[1]);
    if (PyErr_Occurred()) return NULL;
    return state->iface->plot2D(plotID, x, y, write);
}


static PyObject*
plot2DVec(
    exa_state* state,
    std::size_t plotID,
    PyObject* const* args,
    Py_ssize_t nargs,
    bool write)
{
    if (nargs != 2) {
        PyErr_Format(PyExc_TypeError, EXA_PLOT "() takes 2 positional arguments but %zd were given", nargs);
        return NULL;
    }

    auto pyBorrowed_xData = PySequence_Fast(args[0], EXA_PLOT "() 'x' argument must be type 'Sequence'");
    if (pyBorrowed_xData == NULL)
        return NULL;
    auto pyBorrowed_yData = PySequence_Fast(args[1], EXA_PLOT "() 'y' argument must be type 'Sequence'");
    if (pyBorrowed_yData == NULL)
        return NULL;

    auto n_xData = PySequence_Fast_GET_SIZE(pyBorrowed_xData);
    std::vector<double> xData(n_xData);
    for (decltype(n_xData) i = 0; i < n_xData; ++i) {
        auto x = PyFloat_AsDouble(PySequence_Fast_GET_ITEM(pyBorrowed_xData, i));
        if (PyErr_Occurred()) return NULL;
        xData[i] = x;
    }

    auto n_yData = PySequence_Fast_GET_SIZE(pyBorrowed_yData);
    std::vector<double> yData(n_yData);
    for (decltype(n_yData) i = 0; i < n_yData; ++i) {
        auto y = PyFloat_AsDouble(PySequence_Fast_GET_ITEM(pyBorrowed_yData, i));
        if (PyErr_Occurred()) return NULL;
        yData[i] = y;
    }

    return state->iface->plot2DVec(plotID, xData, yData, write);
}


static PyObject*
plotCM(
    exa_state* state,
    std::size_t plotID,
    PyObject* const* args,
    Py_ssize_t nargs,
    bool write)
{
    if (nargs != 3) {
        PyErr_Format(PyExc_TypeError, EXA_PLOT "() takes 3 positional arguments but %zd were given", nargs);
        return NULL;
    }
    auto x = PyLong_AsLong(args[0]);
    if (PyErr_Occurred()) return NULL;
    auto y = PyLong_AsLong(args[1]);
    if (PyErr_Occurred()) return NULL;
    auto value = PyFloat_AsDouble(args[2]);
    if (PyErr_Occurred()) return NULL;
    return state->iface->plotCM(plotID, x, y, value, write);
}


static PyObject*
plotCMVec(
    exa_state* state,
    std::size_t plotID,
    PyObject* const* args,
    Py_ssize_t nargs,
    bool write)
{
    if (nargs != 2) {
        PyErr_Format(PyExc_TypeError, EXA_PLOT "() takes 2 positional arguments but %zd were given", nargs);
        return NULL;
    }

    auto y = PyLong_AsLong(args[0]);
    if (PyErr_Occurred()) return NULL;

    auto pyBorrowed_values = PySequence_Fast(args[1], EXA_PLOT "() 'values' argument must be type 'Sequence'");
    if (pyBorrowed_values == NULL)
        return NULL;
    auto n_values = PySequence_Fast_GET_SIZE(pyBorrowed_values);
    // TODO: limit n_values
    std::vector<double> values(n_values);
    for (decltype(n_values) i = 0; i < n_values; ++i) {
        auto value = PyFloat_AsDouble(PySequence_Fast_GET_ITEM(pyBorrowed_values, i));
        if (PyErr_Occurred()) return NULL;
        values[i] = value;
    }

    return state->iface->plotCMVec(plotID, y, values, write);
}


static PyObject*
plotCMFrame(
    exa_state* state,
    std::size_t plotID,
    PyObject* const* args,
    [[maybe_unused]] Py_ssize_t nargs,
    bool write)
{
    auto pyBorrowed_frame = PySequence_Fast(args[0],
                                            EXA_PLOT "() 'frame' argument must be type 'Sequence[Sequence[Real]]'");
    if (pyBorrowed_frame == NULL)
        return NULL;

    auto n_rows = PySequence_Fast_GET_SIZE(pyBorrowed_frame);
    // TODO: limit n_rows
    if (n_rows == 0) {
        Py_RETURN_NONE;
    }
    if (!PySequence_Check(PySequence_Fast_GET_ITEM(pyBorrowed_frame, 0))) {
        PyErr_SetString(PyExc_TypeError, EXA_PLOT "() 'frame' argument must be type 'Sequence[Sequence[Real]]'");
        return NULL;
    }
    auto n_cols = PySequence_Size(PySequence_Fast_GET_ITEM(pyBorrowed_frame, 0));
    // TODO: limit n_cols
    std::vector<std::vector<double>> frame(n_rows, std::vector<double>(n_cols));

    for (decltype(n_rows) i = 0; i < n_rows; ++i) {
        auto pyBorrowed_row = PySequence_Fast(
            PySequence_Fast_GET_ITEM(pyBorrowed_frame, i),
            EXA_PLOT "() 'frame' argument contains non-Sequence type object (frame[%zd])"
        );
        if (pyBorrowed_row == NULL)
            return NULL;
        if (PySequence_Fast_GET_SIZE(pyBorrowed_row) != n_cols) {
            // TODO: consider using std::move to allow vectors of differing lengths without sacrificing performance
            PyErr_SetString(PyExc_ValueError,
                            EXA_PLOT "() 'frame' argument must contain sequences of equal size (frame[%zd])");
            return NULL;
        }
        for (decltype(n_cols) j = 0; j < n_cols; ++j) {
            auto value = PyFloat_AsDouble(PySequence_Fast_GET_ITEM(pyBorrowed_row, j));
            if (PyErr_Occurred()) return NULL;
            frame[i][j] = value;
        }
    }

    return state->iface->plotCMFrame(plotID, frame, write);
}


/**
 * @brief Module `plot` function
 * 
 * @param module 
 * @param args 
 * @param nargs 
 * @return PyObject* 
 */
PyObject*
exa_plot(PyObject* module, PyObject* const* args, Py_ssize_t nargs, PyObject* kwnames)
{
    assert(nargs >= 0);
    auto state = getModuleState(module);

    if (nargs == 0) {
        PyErr_SetString(PyExc_TypeError, EXA_PLOT "() missing 1 required positional argument: 'plot_id'");
        return NULL;
    }

    auto pyBorrowed_plotID = args[0];
    if (!PyLong_Check(pyBorrowed_plotID)) {
        PyErr_SetString(PyExc_TypeError, EXA_PLOT "() 'plot_id' argument must be type 'int'");
        return NULL;
    }
    auto plotID = PyLong_AsSize_t(pyBorrowed_plotID);
    if (plotID == (size_t)-1 && PyErr_Occurred()) {
        return NULL;
    }

    bool write = true;
    if (kwnames != NULL) {
        auto nkwargs = PyTuple_GET_SIZE(kwnames);
        for (decltype(nkwargs) i = 0; i < nkwargs; ++i) {
            auto pyBorrowed_kwname = PyTuple_GET_ITEM(kwnames, i);
            auto pyBorrowed_kwvalue = args[nargs + i];

            auto kwname = PyUnicode_AsUTF8(pyBorrowed_kwname);
            if (kwname == NULL) return NULL;
            if (std::strcmp(kwname, "write") == 0) {
                if (!PyBool_Check(pyBorrowed_kwvalue)) {
                    PyErr_SetString(PyExc_TypeError, EXA_PLOT "() 'write' argument must be type 'bool'");
                    return NULL;
                }
                write = pyBorrowed_kwvalue == Py_True;
            } else {
                PyErr_Format(PyExc_TypeError, EXA_PLOT "() got an unexpected keyword argument '%s'", kwname);
                return NULL;
            }
        }
    }

    if (nargs == 1) {
        return state->iface->clear(plotID);
    }
    nargs -= 1;

    // TODO: The zeroth data set (AKA the "hidden plot")
    if (plotID == 0) {
        Py_RETURN_NONE;
    }

    std::function<PyObject*(exa_state*, std::size_t, PyObject* const*, Py_ssize_t, bool)> plotFn;
    switch (state->iface->currentPlotType(plotID)) {
    case 0: // 2D
        plotFn = PySequence_Check(args[1]) ? plot2DVec : plot2D;
        break;
    case 1: // color map
        plotFn = nargs == 1 ? plotCMFrame : PySequence_Check(args[2]) ? plotCMVec : plotCM;
        break;
    default:
        PyErr_Format(PyExc_SystemError, "invalid plot type: %zd", state->iface->currentPlotType(plotID));
    case -1:
        return NULL;
    }
    return plotFn(state, plotID, &args[1], nargs, write);
}


/**
 * @brief Module `_set_plot_property` function
 * 
 * @param module 
 * @param args 
 * @return PyObject* 
 */
PyObject*
exa__set_plot_property(PyObject* module, PyObject* args)
{
    exa_state* state = getModuleState(module);

    long plotID = 0;
    const char* c_prop = NULL;
    PyObject* pyBorrowed_value = NULL;

    if (!PyArg_ParseTuple(args, "lsO", &plotID, &c_prop, &pyBorrowed_value)) {
        return NULL;
    }

    if (plotID <= 0) {
        if (!PyErr_Occurred())
            PyErr_SetString(PyExc_ValueError, "plot_id must be greater than zero");
        return NULL;
    }

    try {
        PlotProperty prop{c_prop};
        PlotProperty::Value value;

        switch (prop) {

        // 'str' types
        case PlotProperty::TITLE:
        case PlotProperty::XAXIS:
        case PlotProperty::YAXIS:
        case PlotProperty::TWODIMEN_LINE_TYPE:
        case PlotProperty::TWODIMEN_LINE_COLOR:
        case PlotProperty::TWODIMEN_LINE_STYLE:
        case PlotProperty::TWODIMEN_POINTS_SHAPE:
        case PlotProperty::TWODIMEN_POINTS_COLOR:
        case PlotProperty::COLORMAP_COLOR_MIN:
        case PlotProperty::COLORMAP_COLOR_MAX:
            if (!PyUnicode_Check(pyBorrowed_value)) {
                PyErr_Format(PyExc_TypeError, "%s must be type 'str'", c_prop);
                return NULL;
            }
            {
                auto c_value = PyUnicode_AsUTF8(pyBorrowed_value);
                if (c_value == NULL) return NULL;
                value = std::string{c_value};
            }
            break;

        // 'int' types
        case PlotProperty::MINSIZE_W:
        case PlotProperty::MINSIZE_H:
        case PlotProperty::COLORMAP_DATASIZE_X:
        case PlotProperty::COLORMAP_DATASIZE_Y:
            if (!PyLong_Check(pyBorrowed_value)) {
                PyErr_Format(PyExc_TypeError, "%s must be type 'int'", c_prop);
                return NULL;
            }
            {
                auto long_value = PyLong_AsLong(pyBorrowed_value);
                if (long_value <= 0) {
                    if (!PyErr_Occurred())
                        PyErr_Format(PyExc_ValueError, "%s must be greater than zero", c_prop);
                    return NULL;
                }
                if (long_value > std::numeric_limits<int>::max()) {
                    PyErr_Format(PyExc_OverflowError,
                                 "Value must not exceed %d", std::numeric_limits<int>::max());
                    return NULL;
                }
                value = static_cast<int>(long_value);
            }
            break;

        // 'numbers.Real' types
        case PlotProperty::TWODIMEN_XRANGE_MIN:
        case PlotProperty::TWODIMEN_XRANGE_MAX:
        case PlotProperty::TWODIMEN_YRANGE_MIN:
        case PlotProperty::TWODIMEN_YRANGE_MAX:
        case PlotProperty::TWODIMEN_POINTS_SIZE:
        case PlotProperty::COLORMAP_XRANGE_MIN:
        case PlotProperty::COLORMAP_XRANGE_MAX:
        case PlotProperty::COLORMAP_YRANGE_MIN:
        case PlotProperty::COLORMAP_YRANGE_MAX:
        case PlotProperty::COLORMAP_ZRANGE_MIN:
        case PlotProperty::COLORMAP_ZRANGE_MAX:
            if (!PyFloat_Check(pyBorrowed_value) && !PyLong_Check(pyBorrowed_value)) {
                PyErr_Format(PyExc_TypeError, "%s must be type 'numbers.Real'", c_prop);
                return NULL;
            }
            {
                auto double_value = PyFloat_AsDouble(pyBorrowed_value);
                if (PyErr_Occurred())
                    return NULL;
                if (prop == PlotProperty::TWODIMEN_POINTS_SIZE && double_value < 0.) {
                    PyErr_Format(PyExc_ValueError, "%s must be positive", c_prop);
                    return NULL;
                }
                value = double_value;
            }
            break;

        // 'bool' types
        case PlotProperty::TWODIMEN_AUTORS_AXES:
        case PlotProperty::COLORMAP_AUTORS_AXES:
        case PlotProperty::COLORMAP_AUTORS_DATA:
            if (!PyBool_Check(pyBorrowed_value)) {
                PyErr_Format(PyExc_TypeError, "%s must be type 'bool'", c_prop);
                return NULL;
            }
            value = pyBorrowed_value == Py_True;
            break;

        default:
            PyErr_Format(PyExc_KeyError, "Invalid property '%s'", c_prop);
            return NULL;
        }

        return state->iface->setPlotProperty(plotID, prop, value);
    } catch (const std::out_of_range&)
    {
        PyErr_Format(PyExc_KeyError, "Unknown property '%s'", c_prop);
        return NULL;
    }
}


/**
 * @brief Module `_get_plot_property` function
 * 
 * @param module 
 * @param args 
 * @return PyObject* 
 */
PyObject*
exa__get_plot_property(PyObject* module, PyObject* args)
{
    exa_state* state = getModuleState(module);

    long plotID = 0;
    const char* c_prop = NULL;

    if (!PyArg_ParseTuple(args, "ls", &plotID, &c_prop)) {
        return NULL;
    }
    if (plotID <= 0) {
        PyErr_SetString(PyExc_ValueError, "plot_id must be greater than zero");
        return NULL;
    }

    try {
        return state->iface->getPlotProperty(plotID, PlotProperty{c_prop});
    } catch (const std::out_of_range&)
    {
        PyErr_Format(PyExc_KeyError, "Unknown property '%s'", c_prop);
        return NULL;
    }
}


/**
 * @brief Module `_show_plot` function
 * 
 * @param module 
 * @param args 
 * @return PyObject* 
 */
PyObject*
exa__show_plot(PyObject* module, PyObject* args)
{
    exa_state* state = getModuleState(module);

    long plotID = 0;
    Py_ssize_t plotType = 0;

    if (!PyArg_ParseTuple(args, "ln", &plotID, &plotType)) {
        return NULL;
    }
    if (plotID <= 0) {
        PyErr_SetString(PyExc_ValueError, "plot_id must be greater than zero");
        return NULL;
    }
    if (plotType < 0) {
        PyErr_SetString(PyExc_ValueError, "plot_type must be a positive integer");
        return NULL;
    }
    return state->iface->showPlot(plotID, static_cast<std::size_t>(plotType));
}


/**
 * RunParam implementation
 */


static char*
RunParam_keywords[] =
{
    (char*)"",
    (char*)"display",
    NULL
};


static int
PyRunParam_CheckDefaultArg(PyObject* object)
{
    if (PyUnicode_Check(object)) return RunParamType::STRING;
    if (PyLong_Check(object)) return RunParamType::INT;
    if (PyFloat_Check(object)) return RunParamType::FLOAT;
    return -1;
}


static int
PyRunParam_CheckTypeArg(PyObject* object)
{
    if (!PyType_Check(object)) return -1;
    if (PyObject_IsSubclass(object, (PyObject*)&PyUnicode_Type)) return RunParamType::STRING;
    if (PyObject_IsSubclass(object, (PyObject*)&PyLong_Type)) return RunParamType::INT;
    if (PyObject_IsSubclass(object, (PyObject*)&PyFloat_Type)) return RunParamType::FLOAT;
    return -1;
}


/**
 * @brief `RunParam.__init__`
 * 
 * @param self 
 * @param args 
 * @param kwargs 
 * @return int 
 */
static int
exaplot_RunParam___init__(PyRunParam* self, PyObject* args, PyObject* kwargs)
{
    PyObject* pyBorrowed_defaultOrType = NULL;
    const char* pyBorrowed_display = NULL;

    if (!PyArg_ParseTupleAndKeywords(args, kwargs, "O|z:RunParam.__init__", RunParam_keywords,
                                     &pyBorrowed_defaultOrType,
                                     &pyBorrowed_display)) {
        return -1;
    }

    self->value = NULL;
    self->display = pyBorrowed_display != NULL ? PyUnicode_FromString(pyBorrowed_display) : NULL;

    auto type = PyRunParam_CheckDefaultArg(pyBorrowed_defaultOrType);
    switch (type) {
    case RunParamType::STRING:
        self->value = pyBorrowed_defaultOrType;
        Py_INCREF(pyBorrowed_defaultOrType);
        break;
    case RunParamType::INT:
    case RunParamType::FLOAT:
        {
            auto pyOwned_defaultValue = PyObject_Str(pyBorrowed_defaultOrType);
            if (pyOwned_defaultValue == NULL) return -1;
            self->value = pyOwned_defaultValue;
        }
        break;
    default:
        type = PyRunParam_CheckTypeArg(pyBorrowed_defaultOrType);
        if (type < 0) {
            PyErr_SetString(PyExc_TypeError,
                            "RunParam.__init__() first argument must be type 'RunParamType' or 'type[RunParamType]'");
            return -1;
        }
    }
    self->type = static_cast<RunParamType>(type);

    return 0;
}


static int
PyRunParam_clear(PyRunParam* self)
{
    Py_CLEAR(self->value);
    Py_CLEAR(self->display);
    return 0;
}


static void
PyRunParam_dealloc(PyRunParam* self)
{
    PyObject_GC_UnTrack(self);
    PyRunParam_clear(self);
    auto tp = Py_TYPE(self);
    tp->tp_free(self);
    Py_DECREF(tp);
}


static int
PyRunParam_traverse(PyRunParam* self, visitproc visit, void* arg)
{
    Py_VISIT(self->value);
    Py_VISIT(self->display);
    Py_VISIT(Py_TYPE(self));
    return 0;
}


static PyType_Slot
PyRunParam_slots[] =
{
    {Py_tp_init, (void*)exaplot_RunParam___init__},
    {Py_tp_dealloc, (void*)PyRunParam_dealloc},
    {Py_tp_traverse, (void*)PyRunParam_traverse},
    {Py_tp_clear, (void*)PyRunParam_clear},
    {0, NULL}
};


static PyType_Spec
PyRunParam_spec =
{
    .name = EXA_MODULE "." EXA_RUNPARAM,
    .basicsize = sizeof(PyRunParam),
    .flags = Py_TPFLAGS_DEFAULT | Py_TPFLAGS_HAVE_GC | Py_TPFLAGS_IMMUTABLETYPE,
    .slots = PyRunParam_slots,
};


/**
 * RunParam implementation end
 */


int
moduleSlot_initTypes(PyObject* module)
{
    auto mState = getModuleState(module);

    mState->type_RunParam = reinterpret_cast<PyTypeObject*>(
        PyType_FromModuleAndSpec(module, &PyRunParam_spec, NULL)
    );
    if (mState->type_RunParam == NULL) return -1;

    if (PyModule_AddType(module, mState->type_RunParam)) return -1;

    return 0;
}


int
moduleSlot_initExceptions(PyObject* module)
{
    auto mState = getModuleState(module);

    mState->obj_InterruptException = PyErr_NewException(
        EXA_MODULE "." EXA_INTERRUPT, PyExc_BaseException, NULL
    );
    if (mState->obj_InterruptException == NULL) return -1;

    if (PyModule_AddObjectRef(module, EXA_INTERRUPT, mState->obj_InterruptException) < 0) return -1;

    return 0;
}


}
}
