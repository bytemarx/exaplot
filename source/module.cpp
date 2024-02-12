#include "internal.hpp"

#include <limits>


namespace orbital {


static orbital_state*
getModuleState(PyObject* module)
{
    return static_cast<orbital_state*>(PyModule_GetState(module));
}


extern "C" {


int
orbitalExec(PyObject* module)
{
    orbital_state* mState = getModuleState(module);
    PyThreadState* tState = PyThreadState_Get();
    PyInterpreterState* interp = tState->interp;

    assert(interp->orb_passthrough);
    if (interp->orb_passthrough == NULL) {
        goto error;
    }

    mState->iface = static_cast<OrbitalInterface*>(interp->orb_passthrough);

    return 0;
error:
    return -1;
}


PyObject*
orbital_init(PyObject *module, PyObject *const *args, Py_ssize_t nargs, PyObject *kwnames)
{
    std::vector<std::string> params;
    std::vector<GridPoint> plots{{.x=0, .dx=1, .y=0, .dy=1}};

    for (decltype(nargs) i = 0; i < nargs; ++i) {
        if (!PyUnicode_Check(args[i])) {
            PyErr_Format(PyExc_TypeError,
                ORBITAL_INIT "() parameter #%zd must be type 'string'", i + 1);
            return NULL;
        }
        params.push_back(PyUnicode_AsUTF8(args[i]));
    }

    if (kwnames == NULL)
        goto done;

    for (decltype(PyTuple_GET_SIZE(kwnames)) i = 0; i < PyTuple_GET_SIZE(kwnames); ++i) {
        PyObject* pyBorrowed_kwname = PyTuple_GET_ITEM(kwnames, i);

        if (strcmp("plots", PyUnicode_AsUTF8(pyBorrowed_kwname)) == 0) {
            plots.clear();
            PyObject* pyBorrowed_plots = args[nargs + i];
            if (PyLong_Check(pyBorrowed_plots)) {
                auto n_plots = PyLong_AsLong(pyBorrowed_plots);
                if (n_plots <= 0) {
                    if (!PyErr_Occurred())
                        PyErr_Format(PyExc_ValueError,
                            ORBITAL_INIT "() 'plots' keyword must be an integer greater than zero");
                    return NULL;
                }
                for (decltype(n_plots) i_plot = 0; i_plot < n_plots; ++i_plot) {
                    plots.push_back({
                        .x = static_cast<GridPoint_t>(i_plot),
                        .dx = 1,
                        .y = 0,
                        .dy = 1,
                    });
                }
            } else if (PyList_Check(pyBorrowed_plots)) {
                auto n_plots = PyList_GET_SIZE(pyBorrowed_plots);
                if (n_plots == 0) {
                    PyErr_SetString(PyExc_ValueError,
                        ORBITAL_INIT "() plots list is missing entries");
                    return NULL;
                }
                if (n_plots > 64)
                    n_plots = 64;
                for (decltype(n_plots) i_plot = 0; i_plot < n_plots; ++i_plot) {
                    PyObject* pyBorrowed_plot = PyList_GET_ITEM(pyBorrowed_plots, i_plot);
                    if (!PyTuple_Check(pyBorrowed_plot)) {
                        PyErr_Format(PyExc_TypeError,
                            ORBITAL_INIT "() plot #%zd must be type 'tuple'", i_plot + 1);
                        return NULL;
                    }
                    if (PyTuple_GET_SIZE(pyBorrowed_plot) != 4) {
                        PyErr_Format(PyExc_TypeError,
                            ORBITAL_INIT "() plot #%zd must be a 4-tuple", i_plot + 1);
                        return NULL;
                    }
                    long p[4];
                    for (decltype(std::size(p)) i_plotPoint = 0; i_plotPoint < std::size(p); ++i_plotPoint) {
                        PyObject* pyBorrowed_plotPoint = PyTuple_GET_ITEM(pyBorrowed_plot, i_plotPoint);
                        if (!PyLong_Check(pyBorrowed_plotPoint)) {
                            PyErr_Format(PyExc_TypeError,
                                ORBITAL_INIT "() entry #%zd of plot #%zd must be type 'int'",
                                i_plotPoint + 1, i_plot + 1);
                            return NULL;
                        }
                        auto plotPoint = PyLong_AsLong(pyBorrowed_plotPoint);
                        if (plotPoint < 0) {
                            if (!PyErr_Occurred())
                                PyErr_Format(PyExc_ValueError,
                                    ORBITAL_INIT "() entry #%zd of plot #%zd is invalid: %ld",
                                    i_plotPoint + 1, i_plot + 1, plotPoint);
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
                PyErr_SetString(PyExc_TypeError, ORBITAL_INIT "() plots must be either an 'int' or 'list' type");
                return NULL;
            }
        } else {
            PyErr_Format(PyExc_TypeError, ORBITAL_INIT "() got an unexpected keyword argument '%S'", pyBorrowed_kwname);
            return NULL;
        }
    }

done:
    orbital_state* state = getModuleState(module);
    return state->iface->init(params, plots);
}


static char*
msg_keywords[] = {
    (char*)"",
    (char*)"append",
    NULL
};


PyObject*
orbital_msg(PyObject* module, PyObject* args, PyObject* kwargs)
{
    const char* c_message = NULL;
    int c_append = 0;
    if (!PyArg_ParseTupleAndKeywords(args, kwargs, "s|p:" ORBITAL_MSG, msg_keywords, &c_message, &c_append))
        return NULL;

    std::string message{c_message};
    bool append = c_append != 0;

    orbital_state* state = getModuleState(module);
    return state->iface->msg(message, append);
}


PyObject*
orbital_plot(PyObject* module, PyObject* const* args, Py_ssize_t nargs)
{
    orbital_state* state = getModuleState(module);

    if (nargs == 0) {
        PyErr_SetString(PyExc_TypeError, ORBITAL_PLOT "() missing 1 required positional argument: 'data_set'");
        return NULL;
    }

    PyObject* pyBorrowed_dataSet = args[0];
    if (!PyLong_Check(pyBorrowed_dataSet)) {
        PyErr_SetString(PyExc_TypeError, ORBITAL_PLOT "() data_set parameter must be type 'int'");
        return NULL;
    }
    long dataSet = PyLong_AsLong(pyBorrowed_dataSet);
    if (dataSet < 0) {
        if (!PyErr_Occurred())
            PyErr_SetString(PyExc_ValueError, "data_set parameter must be positive");
        return NULL;
    }

    if (nargs == 1) {
        return state->iface->clear(dataSet);
    }

    // detect overload
    if (PyList_Check(args[1])) {
        std::vector<std::vector<double>> vectorData(nargs - 1);
        for (decltype(nargs) i = 1; i < nargs; ++i) {
            PyObject* pyBorrowed_vectorData = args[i];
            if (!PyList_Check(pyBorrowed_vectorData)) {
                PyErr_Format(PyExc_TypeError, ORBITAL_PLOT "() data argument #%zd must be type 'list'", i);
                return NULL;
            }
            auto n_data = PyList_GET_SIZE(pyBorrowed_vectorData);
            std::vector<double> data(n_data);
            for (decltype(n_data) i_data = 0; i_data < n_data; ++i_data) {
                double dataVal = PyFloat_AsDouble(PyList_GET_ITEM(pyBorrowed_vectorData, i_data));
                if (PyErr_Occurred()) {
                    return NULL;
                }
                data[i_data] = dataVal;
            }
            vectorData[i - 1] = std::move(data);
        }
        return state->iface->plotVec(dataSet, vectorData);
    } else {
        std::vector<double> data(nargs - 1);
        for (decltype(nargs) i = 1; i < nargs; ++i) {
            double dataVal = PyFloat_AsDouble(args[i]);
            if (PyErr_Occurred()) {
                return NULL;
            }
            data[i - 1] = dataVal;
        }
        return state->iface->plot(dataSet, data);
    }
}


PyObject*
orbital__set_plot_property(PyObject* module, PyObject* args)
{
    orbital_state* state = getModuleState(module);

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
                    PyErr_Format(PyExc_OverflowError, "Value must not exceed %d", std::numeric_limits<int>::max());
                    return NULL;
                }
                value = static_cast<int>(long_value);
            }
            break;
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


PyObject*
orbital__get_plot_property(PyObject* module, PyObject* args)
{
    orbital_state* state = getModuleState(module);

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


PyObject*
orbital__show_plot(PyObject* module, PyObject* args)
{
    orbital_state* state = getModuleState(module);

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


}
}
