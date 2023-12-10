#include "orbital.hpp"
#include "internal.hpp"

#include <cstdarg>
#include <limits>
#include <string>
#include <utility>
#include <vector>


namespace orbital {


extern "C" {
    PyObject* orbital_init(PyObject*, PyObject* const*, Py_ssize_t, PyObject*);
    PyObject* orbital_msg(PyObject*, PyObject*, PyObject*);
    PyObject* orbital_plot(PyObject*, PyObject* const*, Py_ssize_t);
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


extern int orbitalExec(PyObject*);


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
    if (PyStatus_Exception(status)) goto exit;

    config.module_search_paths_set = 1;

    // Python interpreter (Look at me, I am the interpreter now)
    status = PyConfig_SetBytesString(
        &config,
        &config.program_name,
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

    // Python home
    status = PyConfig_SetBytesString(
        &config,
        &config.home,
        prefix.c_str()
    );
    if (PyStatus_Exception(status)) goto exit;

    status = Py_InitializeFromConfig(&config);
    if (PyStatus_Exception(status)) goto exit;

exit:
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
    this->m_tState->interp->orb_passthrough = static_cast<const void*>(interface);
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


}
