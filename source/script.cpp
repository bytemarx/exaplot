#include "orbital.hpp"

#include <cstdio>


namespace orbital {


ScriptModule::ScriptModule(PyThreadState* tState, const std::filesystem::path& file)
    : m_tState{tState}
    , m_file{file}
    , m_pyOwned_module{NULL}
{
}


ScriptModule::~ScriptModule()
{
    Py_XDECREF(this->m_pyOwned_module);
}


void
ScriptModule::ensureThreadState()
{
    assert(this->m_tState);
    if (this->m_tState != PyThreadState_Get())
        PyThreadState_Swap(this->m_tState);
}


OrbitalError
ScriptModule::load()
{
    FILE* fp = fopen(this->m_file.c_str(), "r");
    if (fp == NULL)
        return OrbitalError{OrbitalError::IMPORT, "Failed to open file"};
    fseek(fp, 0, SEEK_END);
    size_t size = ftell(fp);
    fseek(fp, 0, SEEK_SET);
    char* code = (char*)malloc(size + 1);
    if (code == NULL) {
        fclose(fp);
        return OrbitalError{OrbitalError::SYSTEM, "Memory allocation failed"};
    }
    fread(code, 1, size, fp);
    code[size] = 0;
    fclose(fp);

    this->ensureThreadState();
    PyObject* codeObject = Py_CompileString(code, this->m_file.c_str(), Py_file_input);
    free(code);
    if (codeObject == NULL)
        return OrbitalError::pyerror(OrbitalError::IMPORT);

    PyObject* pyOwned_module = PyImport_ExecCodeModule("__orbital__", codeObject);
    Py_DECREF(codeObject);
    if (pyOwned_module == NULL)
        return OrbitalError::pyerror(OrbitalError::IMPORT);
    if (this->m_pyOwned_module != NULL)
        Py_DECREF(this->m_pyOwned_module);
    this->m_pyOwned_module = pyOwned_module;

    return OrbitalError{OrbitalError::NONE};
}


OrbitalError
ScriptModule::reload()
{
    // this->ensureThreadState();
    // PyObject* pyOwned_newModule = PyImport_ReloadModule(this->m_pyOwned_module);
    // if (pyOwned_newModule == NULL)
    //     return OrbitalError::pyerror(OrbitalError::RELOAD);
    // Py_DECREF(this->m_pyOwned_module);
    // this->m_pyOwned_module = pyOwned_newModule;
    // return OrbitalError{OrbitalError::NONE};
    // TODO: `PyImport_ReloadModule` doesn't re-execute the module for some reason
    return this->load();
}


OrbitalError
ScriptModule::run(const std::map<std::string, std::string>& kwargs)
{
    this->ensureThreadState();
    PyObject* pyOwned_runFn = NULL;
    PyObject* pyOwned_args = NULL;
    PyObject* pyOwned_kwargs = NULL;
    PyObject* pyOwned_result = NULL;

    pyOwned_runFn = PyObject_GetAttrString(this->m_pyOwned_module, ORBITAL_SCRIPT_RUN);
    if (pyOwned_runFn == NULL) goto error;

    pyOwned_args = PyTuple_New(0);
    if (pyOwned_args == NULL) goto error;

    pyOwned_kwargs = PyDict_New();
    if (pyOwned_kwargs == NULL) goto error;

    for (const auto& entry : kwargs) {
        int result;
        PyObject* pyOwned_val = PyUnicode_FromString(entry.second.c_str());
        if (pyOwned_val == NULL) goto error;
        result = PyDict_SetItemString(pyOwned_kwargs, entry.first.c_str(), pyOwned_val);
        Py_DECREF(pyOwned_val);
        if (result != 0) goto error;
    }
    pyOwned_result = PyObject_Call(pyOwned_runFn, pyOwned_args, pyOwned_kwargs);
    if (pyOwned_result == NULL) goto error;

    Py_DECREF(pyOwned_result);
    Py_DECREF(pyOwned_kwargs);
    Py_DECREF(pyOwned_args);
    Py_DECREF(pyOwned_runFn);
    return OrbitalError{OrbitalError::NONE};

error:
    Py_XDECREF(pyOwned_kwargs);
    Py_XDECREF(pyOwned_args);
    Py_XDECREF(pyOwned_runFn);
    return OrbitalError::pyerror(OrbitalError::RUNTIME);
}


}
