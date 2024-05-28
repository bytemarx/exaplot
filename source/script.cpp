/*
 * ExaPlot
 * script handling/management
 * 
 * SPDX-License-Identifier: GPL-3.0
 * Copyright (C) 2024 bytemarx
 */

#include "internal.hpp"

#include <filesystem>
#include <fstream>


namespace exa {


ScriptModule::ScriptModule(PyThreadState* tState, const std::filesystem::path& file)
    : m_tState{tState}
    , m_file{file}
    , m_pyOwned_module{NULL}
    , m_pyOwned_datafile{NULL}
{
}


ScriptModule::~ScriptModule()
{
    Py_XDECREF(this->m_pyOwned_module);
    Py_XDECREF(this->m_pyOwned_datafile);
}


void
ScriptModule::ensureThreadState()
{
    assert(this->m_tState);
    if (this->m_tState != PyThreadState_Get())
        PyThreadState_Swap(this->m_tState);
}


Error
ScriptModule::load()
{
    std::ifstream ifs{this->m_file, std::ios::binary | std::ios::ate};
    if (!ifs.is_open())
        return Error{Error::IMPORT, "Failed to open file"};
    auto end = ifs.tellg();
    ifs.seekg(0, std::ios::beg);
    auto size = static_cast<size_t>(end - ifs.tellg());
    if (size == 0) {
        ifs.close();
        return Error{Error::IMPORT, "File is empty"};
    }
    auto code = static_cast<char*>(malloc(size + 1));
    if (code == NULL) {
        ifs.close();
        return Error{Error::SYSTEM, "Memory allocation failed"};
    }
    ifs.read(code, size);
    code[size] = 0;
    ifs.close();

    this->ensureThreadState();
    PyObject* codeObject = Py_CompileString(code, this->m_file.string().c_str(), Py_file_input);
    free(code);
    if (codeObject == NULL)
        return Error::pyerror(Error::IMPORT);

    PyObject* pyOwned_module = PyImport_ExecCodeModule(EXA_SCRIPT_MODULE, codeObject);
    Py_DECREF(codeObject);
    if (pyOwned_module == NULL)
        return Error::pyerror(Error::IMPORT);
    if (this->m_pyOwned_module != NULL)
        Py_DECREF(this->m_pyOwned_module);
    this->m_pyOwned_module = pyOwned_module;

    return Error{Error::NONE};
}


Error
ScriptModule::reload()
{
    // this->ensureThreadState();
    // PyObject* pyOwned_newModule = PyImport_ReloadModule(this->m_pyOwned_module);
    // if (pyOwned_newModule == NULL)
    //     return Error::pyerror(Error::RELOAD);
    // Py_DECREF(this->m_pyOwned_module);
    // this->m_pyOwned_module = pyOwned_newModule;
    // return Error{Error::NONE};
    // TODO: `PyImport_ReloadModule` doesn't re-execute the module for some reason
    return this->load();
}


Error
ScriptModule::run(const std::vector<RunParam>& args)
{
    this->ensureThreadState();
    Error status{Error::NONE};
    PyObject* pyOwned_runFn = NULL;
    PyObject* pyOwned_args = NULL;
    PyObject* pyOwned_kwargs = NULL;
    PyObject* pyOwned_result = NULL;

    pyOwned_runFn = PyObject_GetAttrString(this->m_pyOwned_module, EXA_SCRIPT_RUN);
    if (pyOwned_runFn == NULL) goto error;

    pyOwned_args = PyTuple_New(0);
    if (pyOwned_args == NULL) goto error;

    pyOwned_kwargs = PyDict_New();
    if (pyOwned_kwargs == NULL) goto error;

    for (const auto& arg : args) {
        int result;
        PyObject* pyOwned_val = NULL;
        if (arg.value.empty()) {
            pyOwned_val = Py_None;
        } else {
            try {
                switch (arg.type) {
                default:
                case RunParamType::STRING:
                    pyOwned_val = PyUnicode_FromString(arg.value.c_str());
                    break;
                case RunParamType::INT:
                    pyOwned_val = PyLong_FromLong(std::stol(arg.value));
                    break;
                case RunParamType::FLOAT:
                    pyOwned_val = PyFloat_FromDouble(std::stod(arg.value));
                    break;
                }
            } catch ([[maybe_unused]] std::invalid_argument const& e) {
                status = Error{
                    Error::ARGUMENT,
                    std::string{"Invalid argument for parameter '"}.append(arg.identifier).append("'")
                };
                goto done;
            } catch ([[maybe_unused]] std::out_of_range const& e) {
                status = Error{
                    Error::ARGUMENT,
                    std::string{"Out of range for parameter '"}.append(arg.identifier).append("'")
                };
                goto done;
            }
        }
        if (pyOwned_val == NULL) goto error;
        result = PyDict_SetItemString(pyOwned_kwargs, arg.identifier.c_str(), pyOwned_val);
        Py_DECREF(pyOwned_val);
        if (result != 0) goto error;
    }
    pyOwned_result = PyObject_Call(pyOwned_runFn, pyOwned_args, pyOwned_kwargs);
    if (pyOwned_result == NULL) goto error;

done:
    Py_DECREF(pyOwned_result);
    Py_DECREF(pyOwned_kwargs);
    Py_DECREF(pyOwned_args);
    Py_DECREF(pyOwned_runFn);
    return status;

error:
    Py_XDECREF(pyOwned_kwargs);
    Py_XDECREF(pyOwned_args);
    Py_XDECREF(pyOwned_runFn);
    return Error::pyerror(Error::RUNTIME);
}


/**
 * @brief Sets the script's datafile path. By default, this
 * path is 'data.hdf5'. Users can update the datafile path
 * via the 'path' argument passed to the 'datafile' function
 * (e.g. exa.datafile(path='data.hdf5')). The path argument
 * may be a 'PathLike' value or a callable with no arguments
 * that returns a 'PathLike' value.
 * 
 * @param pyBorrowed_datafile 
 */
void
ScriptModule::setDatafile(PyObject* pyBorrowed_datafile)
{
    this->ensureThreadState();
    if (this->m_pyOwned_datafile != NULL)
        Py_DECREF(this->m_pyOwned_datafile);
    this->m_pyOwned_datafile = pyBorrowed_datafile;
    if (this->m_pyOwned_datafile != NULL)
        Py_INCREF(this->m_pyOwned_datafile);
}


/**
 * @brief Calls the datafile function and converts
 * the result to a 'std::filesystem::path' object.
 * 
 * @param datafile 
 * @return Error 
 */
static Error
callDatafileFn(PyObject* pyBorrowed_datafileFn, std::filesystem::path& datafile)
{
    Error status{Error::NONE};
    PyObject* pyOwned_datafile = NULL;

    pyOwned_datafile = PyObject_CallNoArgs(pyBorrowed_datafileFn);
    if (pyOwned_datafile == NULL) goto error;

    Py_DECREF(pyOwned_datafile);
    return status;

error:
    Py_XDECREF(pyOwned_datafile);
    return Error::pyerror(Error::DATAFILE);
}


/**
 * @brief Converts a 'PathLike' Python object to a
 * 'std::filesystem::path' object.
 * 
 * @param pyBorrowed_datafile 
 * @param datafile 
 * @return Error 
 */
static Error
convertDatafilePath(PyObject* pyBorrowed_datafile, std::filesystem::path& datafile)
{
    Error status{Error::NONE};
    PyObject* pyOwned_fspathName = NULL;
    PyObject* pyOwned_fspath = NULL;

    pyOwned_fspathName = PyUnicode_FromString("__fspath__");
    if (pyOwned_fspathName == NULL) goto error;

    pyOwned_fspath = PyObject_CallMethodNoArgs(pyBorrowed_datafile, pyOwned_fspathName);
    if (pyOwned_fspath == NULL) goto error;

    if (PyUnicode_Check(pyOwned_fspath)) {
        auto fspath = PyUnicode_AsWideCharString(pyOwned_fspath, NULL);
        if (fspath == NULL) goto error;
        datafile = fspath;
    } else if (PyBytes_Check(pyOwned_fspath)) {
        auto fspath = PyBytes_AsString(pyOwned_fspath);
        if (fspath == NULL) goto error;
        datafile = fspath;
    } else {
        status = Error{
            Error::DATAFILE,
            "invalid return type from '__fspath__()'"
        };
    }

    Py_DECREF(pyOwned_fspathName);
    Py_DECREF(pyOwned_fspath);
    return status;

error:
    Py_XDECREF(pyOwned_fspathName);
    Py_XDECREF(pyOwned_fspath);
    return Error::pyerror(Error::DATAFILE);
}


/**
 * @brief Retrieves the script's datafile path. If the datafile
 * path is set to a callable, that callable will be invoked and
 * the result converted to a 'std::filesystem::path' object.
 * 
 * @param pyBorrowed_datafile 
 * @param datafile 
 * @return Error 
 */
Error
ScriptModule::getDatafile(std::filesystem::path& datafile)
{
    if (this->m_pyOwned_datafile == NULL) {
        datafile = std::filesystem::path{"data.hdf5"};
        return Error{Error::NONE};
    }

    this->ensureThreadState();
    return PyCallable_Check(this->m_pyOwned_datafile)
        ? callDatafileFn(this->m_pyOwned_datafile, datafile)
        : convertDatafilePath(this->m_pyOwned_datafile, datafile);
}


}
