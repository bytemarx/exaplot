#include "internal.hpp"

#include <cstdarg>
#include <cstdlib>
#include <limits>
#include <string>
#include <utility>
#include <vector>
#include <iostream>

namespace zeta {


static PyMethodDef
moduleMethods[] =
{
    {
        ZETA_INIT,
        (PyCFunction)zeta_init,
        METH_FASTCALL | METH_KEYWORDS,
        NULL
    },
    {
        ZETA_STOP,
        (PyCFunction)zeta_stop,
        METH_NOARGS,
        NULL
    },
    {
        ZETA_MSG,
        (PyCFunction)zeta_msg,
        METH_VARARGS | METH_KEYWORDS,
        NULL
    },
    {
        ZETA_PLOT,
        (PyCFunction)zeta_plot,
        METH_FASTCALL,
        NULL
    },
    {
        ZETA_SET_PLOT,
        (PyCFunction)zeta__set_plot_property,
        METH_VARARGS,
        NULL
    },
    {
        ZETA_GET_PLOT,
        (PyCFunction)zeta__get_plot_property,
        METH_VARARGS,
        NULL
    },
    {
        ZETA_SHOW_PLOT,
        (PyCFunction)zeta__show_plot,
        METH_VARARGS,
        NULL
    },
    {NULL, NULL}
};


static PyModuleDef_Slot
moduleSlots[] =
{
    {Py_mod_exec, (void*)moduleSlot_initInterface},
    {Py_mod_exec, (void*)moduleSlot_initTypes},
    {Py_mod_exec, (void*)moduleSlot_initExceptions},
    {Py_mod_multiple_interpreters, Py_MOD_PER_INTERPRETER_GIL_SUPPORTED},
    {0, NULL},
};


static PyModuleDef
moduleDef =
{
    .m_base = PyModuleDef_HEAD_INIT,
    .m_name = ZETA_MODULE,
    .m_doc = NULL,
    .m_size = sizeof(zeta_state),
    .m_methods = moduleMethods,
    .m_slots = moduleSlots,
    .m_traverse = module_traverse,
    .m_clear = module_clear,
    .m_free = NULL,
};


zeta_state*
getModuleStateFromObject(PyObject* object)
{
    auto pyBorrowed_type = Py_TYPE(object);
    if (!PyType_HasFeature(pyBorrowed_type, Py_TPFLAGS_HEAPTYPE))
        return NULL;

    auto heapType = reinterpret_cast<PyHeapTypeObject*>(pyBorrowed_type);
    auto module = heapType->ht_module;
    if (module && PyModule_GetDef(module) == &moduleDef)
        return static_cast<zeta_state*>(PyModule_GetState(module));

    return NULL;
}


PyMODINIT_FUNC
PyInit__zetaplot(void)
{
    return PyModuleDef_Init(&moduleDef);
}


static PyStatus
preinit(void)
{
    PyPreConfig preConfig;

    PyPreConfig_InitIsolatedConfig(&preConfig);
    return Py_PreInitialize(&preConfig);
}


/**
 * @brief Obtains the string representation of a Python object. If the conversion was successful,
 * returns `true` and sets the given `std::string` object to the result, otherwise returns `false`
 * (Python's error indicator may be set on failure).
 * 
 * @param object 
 * @param str 
 * @return true 
 * @return false 
 */
static bool
pyStrToStr(PyObject* pyBorrowed_object, std::string& str)
{
    PyObject* pyOwned_msg = PyObject_Str(pyBorrowed_object);
    if (pyOwned_msg == NULL)
        return false;

    Py_ssize_t size;
    const char* msg = PyUnicode_AsUTF8AndSize(pyOwned_msg, &size);
    if (msg == NULL) {
        Py_DECREF(pyOwned_msg);
        return false;
    }

    str = std::string{msg, static_cast<std::string::size_type>(size)};
    Py_DECREF(pyOwned_msg);
    return true;
}


/**
 * @brief Obtains a formatted traceback string from an exception object. Returns `false` if an
 * error occurred (Python's error indicator may be set on failure), otherwise returns `true` and
 * sets the given `std::string` object to the result (if the exception doesn't have a traceback,
 * then the result is set to an empty string).
 * 
 * @param exception 
 * @param str 
 * @return true 
 * @return false 
 */
static bool
pyTracebackToStr(PyObject* pyBorrowed_exception, std::string& str)
{
    PyObject* pyOwned_traceback = NULL;
    PyObject* pyOwned_module = NULL;
    PyObject* pyOwned_formatTb = NULL;
    PyObject* pyOwned_tracebackList = NULL;
    PyObject* pyOwned_emptyStr = NULL;
    PyObject* pyOwned_tracebackStr = NULL;
    const char* tracebackStr = NULL;
    Py_ssize_t size = 0;

    if (pyBorrowed_exception == NULL)
        return false;

    pyOwned_traceback = PyException_GetTraceback(pyBorrowed_exception);
    if (pyOwned_traceback == NULL) {
        str = std::string{};
        return true;
    }

    pyOwned_module = PyImport_ImportModule("traceback");
    if (pyOwned_module == NULL) goto error;

    pyOwned_formatTb = PyObject_GetAttrString(pyOwned_module, "format_tb");
    if (pyOwned_formatTb == NULL) goto error;

    pyOwned_tracebackList = PyObject_CallFunctionObjArgs(pyOwned_formatTb, pyOwned_traceback, NULL);
    if (pyOwned_tracebackList == NULL) goto error;

    pyOwned_emptyStr = PyUnicode_FromString("");
    if (pyOwned_emptyStr == NULL) goto error;

    pyOwned_tracebackStr = PyUnicode_Join(pyOwned_emptyStr, pyOwned_tracebackList);
    if (pyOwned_tracebackStr == NULL) goto error;

    tracebackStr = PyUnicode_AsUTF8AndSize(pyOwned_tracebackStr, &size);
    if (tracebackStr == NULL) goto error;

    str = std::string{tracebackStr, static_cast<std::string::size_type>(size)};

    Py_DECREF(pyOwned_tracebackStr);
    Py_DECREF(pyOwned_emptyStr);
    Py_DECREF(pyOwned_tracebackList);
    Py_DECREF(pyOwned_formatTb);
    Py_DECREF(pyOwned_module);
    Py_DECREF(pyOwned_traceback);
    return true;

error:
    Py_XDECREF(pyOwned_tracebackStr);
    Py_XDECREF(pyOwned_emptyStr);
    Py_XDECREF(pyOwned_tracebackList);
    Py_XDECREF(pyOwned_formatTb);
    Py_XDECREF(pyOwned_module);
    Py_DECREF(pyOwned_traceback);
    return false;
}


static bool
isInterrupt(PyObject* object)
{
    // TODO: PyErr_GivenExceptionMatches(object, mState->obj_InterruptException)
    if (strcmp(object->ob_type->tp_name, ZETA_INTERRUPT) != 0)
        return false;

    auto pyDict = PyType_GetDict(Py_TYPE(object));
    auto pyModule = PyDict_GetItemString(pyDict, "__module__");
    if (pyModule && PyUnicode_Check(pyModule)) {
        auto module = PyUnicode_AsUTF8(pyModule);
        if (module != NULL)
            return strcmp(module, ZETA_MODULE) == 0;
        if (PyErr_Occurred())
            PyErr_Clear();
    }
    return false;
}


static std::map<std::string, PlotProperty::Type> propertyMap{
    {"title", PlotProperty::TITLE},
    {"x_axis", PlotProperty::XAXIS},
    {"y_axis", PlotProperty::YAXIS},
    {"min_size", PlotProperty::MINSIZE},
    {"min_size.w", PlotProperty::MINSIZE_W},
    {"min_size.h", PlotProperty::MINSIZE_H},
    {"two_dimen.x_range.min", PlotProperty::TWODIMEN_XRANGE_MIN},
    {"two_dimen.x_range.max", PlotProperty::TWODIMEN_XRANGE_MAX},
    {"two_dimen.y_range.min", PlotProperty::TWODIMEN_YRANGE_MIN},
    {"two_dimen.y_range.max", PlotProperty::TWODIMEN_YRANGE_MAX},
    {"two_dimen.line.type", PlotProperty::TWODIMEN_LINE_TYPE},
    {"two_dimen.line.color", PlotProperty::TWODIMEN_LINE_COLOR},
    {"two_dimen.line.style", PlotProperty::TWODIMEN_LINE_STYLE},
    {"two_dimen.points.shape", PlotProperty::TWODIMEN_POINTS_SHAPE},
    {"two_dimen.points.color", PlotProperty::TWODIMEN_POINTS_COLOR},
    {"two_dimen.points.size", PlotProperty::TWODIMEN_POINTS_SIZE},
    {"two_dimen.autorescale_axes", PlotProperty::TWODIMEN_AUTORS_AXES},
    {"color_map.x_range.min", PlotProperty::COLORMAP_XRANGE_MIN},
    {"color_map.x_range.max", PlotProperty::COLORMAP_XRANGE_MAX},
    {"color_map.y_range.min", PlotProperty::COLORMAP_YRANGE_MIN},
    {"color_map.y_range.max", PlotProperty::COLORMAP_YRANGE_MAX},
    {"color_map.z_range.min", PlotProperty::COLORMAP_ZRANGE_MIN},
    {"color_map.z_range.max", PlotProperty::COLORMAP_ZRANGE_MAX},
    {"color_map.data_size.x", PlotProperty::COLORMAP_DATASIZE_X},
    {"color_map.data_size.y", PlotProperty::COLORMAP_DATASIZE_Y},
    {"color_map.color.min", PlotProperty::COLORMAP_COLOR_MIN},
    {"color_map.color.max", PlotProperty::COLORMAP_COLOR_MAX},
    {"color_map.autorescale_axes", PlotProperty::COLORMAP_AUTORS_AXES},
    {"color_map.autorescale_data", PlotProperty::COLORMAP_AUTORS_DATA},
};


const char*
PlotProperty::toStr(Type t)
{
    for (const auto& [key, value] : propertyMap)
        if (value == t) return key.c_str();
    throw std::out_of_range{"Plot property has no mapping"};
}


PlotProperty::PlotProperty(const std::string& property)
    : m_property{propertyMap.at(property)}
    , m_str{property}
{}


Error
Error::pyerror(Type type)
{
    Error error{type};
    PyObject* pyOwned_exception = NULL;

    pyOwned_exception = PyErr_GetRaisedException();
    if (pyOwned_exception == NULL)
        return Error{NONE};

    if (isInterrupt(pyOwned_exception))
        error.m_type = INTERRUPT;

    if (!pyStrToStr(pyOwned_exception, error.m_message)) goto error;
    if (!pyTracebackToStr(pyOwned_exception, error.m_traceback)) goto error;
    Py_DECREF(pyOwned_exception);

    assert(!PyErr_Occurred());
    return error;
error:
    Py_DECREF(pyOwned_exception);
    if (PyErr_Occurred())
        PyErr_Print();
    throw std::runtime_error{"Fatal error parsing Python error"};
}


Error::Error(
    const Type type,
    const std::string& msg,
    const std::string& tb)
    : m_type{type}
    , m_message{msg}
    , m_traceback{tb}
{}


std::size_t Core::coreCount = 0;
PyThreadState* Core::mainThreadState = NULL;


/**
 * @brief Initializes the isolated embedded Python interpreter. The given prefix value must be the 
 * directory path for the `lib`, `lib-dynload`, and `site-packages` directories.
 * 
 * @param executable absolute path of the executable binary for the Python interpreter: 
 * `sys.executable`
 * @param prefix library path
 * @return PyStatus 
 */
PyStatus
Core::init(
    const std::filesystem::path& executable,
    const std::filesystem::path& prefix)
{
    PyStatus status;
    PyConfig config;

    if (PyImport_AppendInittab(ZETA_MODULE, PyInit__zetaplot) == -1) {
        return PyStatus_Error("Failed to append " ZETA_MODULE " to the built-in modules.");
    }

    status = preinit();
    if (PyStatus_Exception(status)) return status;

    PyConfig_InitIsolatedConfig(&config);

    status = PyConfig_Read(&config);
    if (PyStatus_Exception(status)) goto done;

    config.module_search_paths_set = 1;

    // Python interpreter (Look at me, I am the interpreter now)
    status = PyConfig_SetBytesString(
        &config,
        &config.program_name,
        executable.string().c_str()
    );
    if (PyStatus_Exception(status)) goto done;

    // stdlib modules
    status = PyWideStringList_Append(
        &config.module_search_paths,
        (prefix / "lib").wstring().c_str()
    );
    if (PyStatus_Exception(status)) goto done;

    // stdlib extension modules
    status = PyWideStringList_Append(
        &config.module_search_paths,
        (prefix / "lib-dynload").wstring().c_str()
    );
    if (PyStatus_Exception(status)) goto done;

    // third-party modules
    if (std::getenv("PYTHONPATH")) {
        status = PyWideStringList_Append(
            &config.module_search_paths,
            std::filesystem::path{std::getenv("PYTHONPATH")}.wstring().c_str()
        );
        if (PyStatus_Exception(status)) goto done;
    }

    // Python home
    status = PyConfig_SetBytesString(
        &config,
        &config.home,
        prefix.string().c_str()
    );
    if (PyStatus_Exception(status)) goto done;

    status = Py_InitializeFromConfig(&config);
    if (PyStatus_Exception(status)) goto done;

    mainThreadState = PyThreadState_Get();

done:
    PyConfig_Clear(&config);
    return status;
}


int
Core::deinit()
{
    if (mainThreadState != _PyThreadState_UncheckedGet())
        PyThreadState_Swap(mainThreadState);
    return Py_FinalizeEx();
}


Interface::~Interface() = default;


Core::Core(Interface* interface)
    : m_interface{interface}
    , m_tState{NULL}
{
    assert(Py_IsInitialized());
    if (this->coreCount == 0) {
        this->m_tState = Py_NewInterpreter();
    } else {
        PyInterpreterConfig config = {
            .use_main_obmalloc = 0,
            .allow_fork = 0,
            .allow_exec = 0,
            .allow_threads = 1,
            .allow_daemon_threads = 0,
            .check_multi_interp_extensions = 1,
            .gil = PyInterpreterConfig_OWN_GIL
        };
        PyStatus status = Py_NewInterpreterFromConfig(&this->m_tState, &config);
        if (PyStatus_Exception(status)) {
            throw std::runtime_error{"Failed to initialize interpreter from config"};
        }
    }
    this->m_tState->interp->passthrough = static_cast<void*>(interface);
    this->coreCount++;
}


Core::~Core()
{
    if (this->m_tState != _PyThreadState_UncheckedGet())
        PyThreadState_Swap(this->m_tState);
    for (const auto& script : this->m_scripts) {
        if (auto h_script = script.lock()) {
            // If the core is deleted while a script is still alive,
            //   manually decrement its reference and nullify its
            //   contents (use of a script with a destroyed core is
            //   a fatal error).
            if (h_script.get()->m_pyOwned_module) {
                assert(h_script.get()->m_pyOwned_module->ob_refcnt > 0);
                Py_DECREF(h_script.get()->m_pyOwned_module);
                h_script.get()->m_pyOwned_module = NULL;
            }
            h_script.get()->m_tState = NULL;
        }
    }
    Py_EndInterpreter(this->m_tState);
    assert(PyThreadState_Swap(this->mainThreadState) == NULL);
    this->coreCount--;
}


Error
Core::load(const std::filesystem::path& file, std::shared_ptr<ScriptModule>& module)
{
    module = std::shared_ptr<ScriptModule>(new ScriptModule{this->m_tState, file});
    this->m_scripts.push_back(module);
    return module->load();
}


}
