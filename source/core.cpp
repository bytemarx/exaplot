#include "orbital.hpp"

// #ifndef Py_BUILD_CORE
// #define Py_BUILD_CORE
// #include <pycore_interp.h>
// #undef Py_BUILD_CORE
// #endif

#include <cstdarg>
#include <limits>
#include <string>
#include <utility>
#include <vector>


// typedef struct _is {

//     PyInterpreterState *next;

//     int64_t id;
//     int64_t id_refcount;
//     int requires_idref;
//     PyThread_type_lock id_mutex;

//     /* Has been initialized to a safe state.

//        In order to be effective, this must be set to 0 during or right
//        after allocation. */
//     int _initialized;
//     int finalizing;

//     uint64_t monitoring_version;
//     uint64_t last_restart_version;
//     struct pythreads {
//         uint64_t next_unique_id;
//         /* The linked list of threads, newest first. */
//         PyThreadState *head;
//         /* Used in Modules/_threadmodule.c. */
//         long count;
//         /* Support for runtime thread stack size tuning.
//            A value of 0 means using the platform's default stack size
//            or the size specified by the THREAD_STACK_SIZE macro. */
//         /* Used in Python/thread.c. */
//         size_t stacksize;
//     } threads;

//     // CUSTOM ORBITAL MODIFICATION
//     const void *orb_passthrough;

// } PyInterpreterState;


namespace orbital {


typedef struct s_orbital_state {
    const OrbitalInterface* iface;
} orbital_state;


static orbital_state*
getModuleState(PyObject* module)
{
    return static_cast<orbital_state*>(PyModule_GetState(module));
}


extern "C" {
    static PyObject* orbital_init(PyObject*, PyObject* const*, Py_ssize_t, PyObject*);
    static PyObject* orbital_msg(PyObject*, PyObject*, PyObject*);
    static PyObject* orbital_plot(PyObject*, PyObject* const*, Py_ssize_t);
}


static PyMethodDef
moduleMethods[] =
{
    {
        ORBITAL_INIT,
        (PyCFunction)orbital_init,
        METH_FASTCALL | METH_KEYWORDS,
        NULL
    },
    {
        ORBITAL_MSG,
        (PyCFunction)orbital_msg,
        METH_VARARGS | METH_KEYWORDS,
        NULL
    },
    {
        ORBITAL_PLOT,
        (PyCFunction)orbital_plot,
        METH_FASTCALL,
        NULL
    },
    {NULL, NULL}
};


static int
orbitalExec(PyObject* module)
{
    orbital_state* mState = getModuleState(module);
    PyThreadState* tState = PyThreadState_Get();
    PyInterpreterState* interp = tState->interp;
    // sizeof(PyInterpreterState);

    // assert(interp->orb_passthrough);

    // if (interp->orb_passthrough == NULL) {
    //     goto error;
    // }

    // mState->iface = static_cast<const OrbitalInterface*>(interp->orb_passthrough);

    return 0;
// error:
//     return -1;
}


static PyModuleDef_Slot
moduleSlots[] =
{
    {Py_mod_exec, (void*)orbitalExec},
    {Py_mod_multiple_interpreters, Py_MOD_PER_INTERPRETER_GIL_SUPPORTED},
    {0, NULL},
};


PyModuleDef
moduleDef =
{
    .m_base = PyModuleDef_HEAD_INIT,
    .m_name = ORBITAL_MODULE,
    .m_doc = NULL,
    .m_size = sizeof(orbital_state),
    .m_methods = moduleMethods,
    .m_slots = moduleSlots,
    .m_traverse = NULL,
    .m_clear = NULL,
    .m_free = NULL,
};


PyMODINIT_FUNC
PyInit__orbital(void)
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


static PyObject*
getModuleAttr(PyObject* pyModule, const char* attrName)
{
    PyObject* pyModuleAttr = PyObject_GetAttrString(pyModule, attrName);
    if (pyModuleAttr == NULL || !PyCallable_Check(pyModuleAttr)) {
        Py_XDECREF(pyModuleAttr);
        return NULL;
    }
    return pyModuleAttr;
}


/**
 * @brief Returns the `traceback.format_tb` function. Throws a runtime error on failure.
 * 
 * @return PyObject* 
 */
static PyObject*
pyErrBootstrap()
{
    PyObject* tracebackModule = PyImport_ImportModule("traceback");
    if (tracebackModule == NULL) {
        throw std::runtime_error("Failed to import traceback module");
    }

    PyObject* format_tb = getModuleAttr(tracebackModule, "format_tb");
    Py_DECREF(tracebackModule);
    if (format_tb == NULL) {
        throw std::runtime_error("Failed to load traceback.format_tb function");
    }

    return format_tb;
}


ssize_t OrbitalCore::coreCount = 0;


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
OrbitalCore::init(
    const std::wstring& executable,
    const std::filesystem::path& prefix)
{
    PyStatus status;
    PyConfig config;

    // assert(orbital::core == nullptr);

    if (PyImport_AppendInittab(ORBITAL_MODULE, PyInit__orbital) == -1) {
        return PyStatus_Error("Failed to append _orbital to the built-in modules.");
    }

    status = preinit();
    if (PyStatus_Exception(status)) return status;

    PyConfig_InitIsolatedConfig(&config);

    status = PyConfig_Read(&config);
    if (PyStatus_Exception(status)) goto exit;

    config.module_search_paths_set = 1;

    // Python interpreter (Look at me, I am the interpreter now)
    status = PyConfig_SetString(
        &config,
        &config.executable,
        executable.c_str()
    );
    if (PyStatus_Exception(status)) goto exit;

    // stdlib modules
    status = PyWideStringList_Append(
        &config.module_search_paths,
        (prefix / "lib").wstring().c_str()
    );
    if (PyStatus_Exception(status)) goto exit;

    // stdlib extension modules
    status = PyWideStringList_Append(
        &config.module_search_paths,
        (prefix / "lib-dynload").wstring().c_str()
    );
    if (PyStatus_Exception(status)) goto exit;

    // third-party modules
    status = PyWideStringList_Append(
        &config.module_search_paths,
        (prefix / "site-packages").wstring().c_str()
    );
    if (PyStatus_Exception(status)) goto exit;

    status = Py_InitializeFromConfig(&config);
    if (PyStatus_Exception(status)) goto exit;

    // TODO: Save me PEP 703
    // orbital::core = new OrbitalCore{interface};
    // *core = orbital::core;

exit:
    PyConfig_Clear(&config);
    return status;
}


int
OrbitalCore::deinit()
{
    return Py_FinalizeEx();
}


OrbitalCore::OrbitalCore(const OrbitalInterface* interface)
    : m_interface{interface}
    // , m_pyOwned_formatTraceback{pyErrBootstrap()}
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
            throw std::runtime_error("Failed to initialize interpreter from config");
        }
    }
    // this->m_tState->interp->orb_passthrough = static_cast<const void*>(interface);
    this->coreCount++;
}


OrbitalCore::~OrbitalCore()
{
    Py_EndInterpreter(this->m_tState);
    // Py_DECREF(this->m_pyOwned_formatTraceback);
    this->coreCount--;
}


OrbitalError
OrbitalCore::load(const std::filesystem::path& file, std::unique_ptr<ScriptModule>& module)
{
    module = std::unique_ptr<ScriptModule>(new ScriptModule{file});
    return module->load();
}


static PyObject*
orbital_init(PyObject *module, PyObject *const *args, Py_ssize_t nargs, PyObject *kwnames)
{
    std::vector<std::string> params;
    std::vector<GridPoint> plots{{.x=0, .dx=1, .y=0, .dy=1}};

    for (decltype(nargs) i = 0; i < nargs; ++i) {
        if (!PyUnicode_Check(args[i])) {
            PyErr_Format(PyExc_TypeError, ORBITAL_INIT "() parameter #%zd must be type 'string'", i + 1);
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
                if (n_plots <= 0 || n_plots > ORBITAL_MAX_PLOTS) {
                    if (!PyErr_Occurred()) {
                        PyErr_Format(PyExc_ValueError, ORBITAL_INIT "() 'plots' keyword must be an integer from 1 to %d", ORBITAL_MAX_PLOTS);
                    }
                    return NULL;
                }
                for (decltype(n_plots) i_plot = 0; i_plot < n_plots; ++i_plot) {
                    plots.push_back({
                        .x = (int)i_plot,
                        .dx = 1,
                        .y = 0,
                        .dy = 1,
                    });
                }
            } else if (PyList_Check(pyBorrowed_plots)) {
                auto n_plots = PyList_GET_SIZE(pyBorrowed_plots);
                if (n_plots == 0) {
                    PyErr_SetString(PyExc_ValueError, ORBITAL_INIT "() plots list is missing entries");
                    return NULL;
                }
                if (n_plots > 64)
                    n_plots = 64;
                for (decltype(n_plots) i_plot = 0; i_plot < n_plots; ++i_plot) {
                    PyObject* pyBorrowed_plot = PyList_GET_ITEM(pyBorrowed_plots, i_plot);
                    if (!PyTuple_Check(pyBorrowed_plot)) {
                        PyErr_Format(PyExc_TypeError, ORBITAL_INIT "() plot #%zd must be type 'tuple'", i_plot + 1);
                        return NULL;
                    }
                    if (PyTuple_GET_SIZE(pyBorrowed_plot) != 4) {
                        PyErr_Format(PyExc_TypeError, ORBITAL_INIT "() plot #%zd must be a 4-tuple", i_plot + 1);
                        return NULL;
                    }
                    int p[4];
                    for (decltype(std::size(p)) i_plotPoint = 0; i_plotPoint < std::size(p); ++i_plotPoint) {
                        PyObject* pyBorrowed_plotPoint = PyTuple_GET_ITEM(pyBorrowed_plot, i_plotPoint);
                        if (!PyLong_Check(pyBorrowed_plotPoint)) {
                            PyErr_Format(PyExc_TypeError, ORBITAL_INIT "() entry #%zd of plot #%zd must be type 'int'", i_plotPoint + 1, i_plot + 1);
                            return NULL;
                        }
                        auto plotPoint = PyLong_AsLong(pyBorrowed_plotPoint);
                        if (plotPoint < 0 || plotPoint > (decltype(plotPoint))std::numeric_limits<int>::max()) {
                            if (!PyErr_Occurred()) {
                                PyErr_Format(PyExc_ValueError, ORBITAL_INIT "() entry #%zd of plot #%zd is invalid: %ld", i_plotPoint + 1, i_plot + 1, plotPoint);
                            }
                            return NULL;
                        }
                        p[i_plotPoint] = (int)plotPoint;
                    }
                    plots.push_back({
                        .x = p[0],
                        .dx = p[1],
                        .y = p[2],
                        .dy = p[3],
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


static PyObject*
orbital_msg(PyObject* module, PyObject* args, PyObject* kwargs)
{
    const char *c_message = NULL;
    int c_append = 0;
    if (!PyArg_ParseTupleAndKeywords(args, kwargs, "s|p:" ORBITAL_MSG, msg_keywords, &c_message, &c_append))
        return NULL;

    std::string message{c_message};
    bool append = c_append != 0;

    orbital_state* state = getModuleState(module);
    return state->iface->msg(message, append);
}


static PyObject*
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


}