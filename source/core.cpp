#include "internal.hpp"

#include <cstdarg>
#include <limits>
#include <string>
#include <utility>
#include <vector>


namespace orbital {


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


// extern int orbitalExec(PyObject*);


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
traceback(PyObject* pyBorrowed_traceback)
{
    PyObject* pyOwned_module = NULL;
    PyObject* pyOwned_formatTb = NULL;
    PyObject* pyOwned_tracebackList = NULL;
    PyObject* pyOwned_emptyStr = NULL;
    PyObject* pyOwned_tracebackStr = NULL;

    if (pyBorrowed_traceback == NULL)
        return NULL;

    pyOwned_module = PyImport_ImportModule("traceback");
    if (pyOwned_module == NULL) goto error;

    pyOwned_formatTb = PyObject_GetAttrString(pyOwned_module, "format_tb");
    if (pyOwned_formatTb == NULL) goto error;

    pyOwned_tracebackList = PyObject_CallFunctionObjArgs(pyOwned_formatTb, pyBorrowed_traceback, NULL);
    if (pyOwned_tracebackList == NULL) goto error;

    pyOwned_emptyStr = PyUnicode_FromString("");
    if (pyOwned_emptyStr == NULL) goto error;

    pyOwned_tracebackStr = PyUnicode_Join(pyOwned_emptyStr, pyOwned_tracebackList);

    Py_DECREF(pyOwned_emptyStr);
    Py_DECREF(pyOwned_tracebackList);
    Py_DECREF(pyOwned_formatTb);
    Py_DECREF(pyOwned_module);
    return pyOwned_tracebackStr;

error:
    Py_XDECREF(pyOwned_emptyStr);
    Py_XDECREF(pyOwned_tracebackList);
    Py_XDECREF(pyOwned_formatTb);
    Py_XDECREF(pyOwned_module);
    return NULL;
}


OrbitalError
OrbitalError::pyerror(OrbitalError::Type type)
{
    PyObject* pyOwned_type = NULL;
    PyObject* pyOwned_value = NULL;
    PyObject* pyOwned_traceback = NULL;
    OrbitalError error{type};

    if (PyErr_Occurred() == NULL)
        return OrbitalError{OrbitalError::NONE};

    PyErr_Fetch(&pyOwned_type, &pyOwned_value, &pyOwned_traceback);
    Py_DECREF(pyOwned_type);

    // msg = str(e)
    // * look what they need to mimic a fraction of our power *
    if (pyOwned_value != NULL) {
        PyObject* pyOwned_msg = PyObject_Str(pyOwned_value);
        if (pyOwned_msg == NULL) {
            Py_DECREF(pyOwned_value);
            goto error;
        }

        Py_ssize_t size;
        const char* msg = PyUnicode_AsUTF8AndSize(pyOwned_msg, &size);
        if (msg == NULL) {
            Py_DECREF(pyOwned_msg);
            Py_DECREF(pyOwned_value);
            goto error;
        }

        error.m_message = std::string{msg, static_cast<std::string::size_type>(size)};
        Py_DECREF(pyOwned_msg);
        Py_DECREF(pyOwned_value);
    }

    if (pyOwned_traceback != NULL) {
        PyObject* pyOwned_tracebackStr = orbital::traceback(pyOwned_traceback);
        if (pyOwned_tracebackStr == NULL) {
            Py_DECREF(pyOwned_traceback);
            goto error;
        }

        Py_ssize_t size;
        const char* tracebackStr = PyUnicode_AsUTF8AndSize(pyOwned_tracebackStr, &size);
        if (tracebackStr == NULL) {
            Py_DECREF(pyOwned_tracebackStr);
            Py_DECREF(pyOwned_traceback);
            goto error;
        }

        error.m_traceback = std::string{tracebackStr, static_cast<std::string::size_type>(size)};
        Py_DECREF(pyOwned_tracebackStr);
        Py_DECREF(pyOwned_traceback);
    }

    assert(!PyErr_Occurred());
    return error;
error:
    if (PyErr_Occurred())
        PyErr_Print();
    throw std::runtime_error{"Fatal error parsing Python error"};
}


OrbitalError::OrbitalError(
    const Type type,
    const std::string& msg,
    const std::string& tb)
    : m_type{type}
    , m_message{msg}
    , m_traceback{tb}
{}


ssize_t OrbitalCore::coreCount = 0;
PyThreadState* OrbitalCore::mainThreadState = NULL;


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
    const std::filesystem::path& executable,
    const std::filesystem::path& prefix)
{
    PyStatus status;
    PyConfig config;

    if (PyImport_AppendInittab(ORBITAL_MODULE, PyInit__orbital) == -1) {
        return PyStatus_Error("Failed to append _orbital to the built-in modules.");
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
        executable.c_str()
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
    status = PyWideStringList_Append(
        &config.module_search_paths,
        (prefix / "site-packages").wstring().c_str()
    );
    if (PyStatus_Exception(status)) goto done;

    // Python home
    status = PyConfig_SetBytesString(
        &config,
        &config.home,
        prefix.c_str()
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
OrbitalCore::deinit()
{
    return Py_FinalizeEx();
}


OrbitalInterface::~OrbitalInterface() = default;


OrbitalCore::OrbitalCore(const OrbitalInterface* interface)
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
            throw std::runtime_error("Failed to initialize interpreter from config");
        }
    }
    this->m_tState->interp->orb_passthrough = static_cast<const void*>(interface);
    this->coreCount++;
}


OrbitalCore::~OrbitalCore()
{
    if (this->m_tState != PyThreadState_Get())
        PyThreadState_Swap(this->m_tState);
    Py_EndInterpreter(this->m_tState);
    assert(PyThreadState_Swap(this->mainThreadState) == NULL);
    this->coreCount--;
}


OrbitalError
OrbitalCore::load(const std::filesystem::path& file, std::unique_ptr<ScriptModule>& module)
{
    module = std::unique_ptr<ScriptModule>(new ScriptModule{this->m_tState, file});
    return module->load();
}


}
