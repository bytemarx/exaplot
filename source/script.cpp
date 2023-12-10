#include "orbital.hpp"

#include <cstdio>


namespace orbital {


ScriptModule::ScriptModule(const std::filesystem::path& file)
    : m_file{file}
    , m_pyOwned_globals{PyDict_New()}
    , m_pyOwned_locals{PyDict_New()}
{
    // this->m_pyOwned_file = PyUnicode_DecodeFSDefault(file.lexically_normal().c_str());
    // if (m_pyOwned_file == NULL) {
    //     throw std::runtime_error{"Failed to decode the module file"};
    // }
}


ScriptModule::~ScriptModule()
{
    // Py_DECREF(this->m_pyOwned_file);
    Py_DECREF(this->m_pyOwned_globals);
    Py_DECREF(this->m_pyOwned_locals);
    // Py_XDECREF(this->m_pyOwned_module);
}


OrbitalError ScriptModule::load()
{
    // this->m_pyOwned_module = PyImport_Import(this->m_pyOwned_file);

    // if (this->m_pyOwned_module == NULL) {
    //     return OrbitalError{OrbitalError::IMPORT};
    // }
    // PyObject* pyOwned_module = PyImport_AddModule("__main__");
    // PyObject* pyOwned_
    // PyObject* pyBorrowed_globals = PyEval_GetGlobals();
    // // PyObject* pyBorrowed_locals = PyEval_GetLocals();
    // // if (pyBorrowed_locals == NULL)
    // PyObject* pyBorrowed_locals = this->m_pyOwned_locals;
    // assert(!PyErr_Occurred());
    // // if (PyErr_Occurred())
    // //     return OrbitalError{OrbitalError::UNDEFINED};
    // FILE* fp = fopen(this->m_file.c_str(), "r");
    // PyObject* result = PyRun_FileExFlags(fp, this->m_file.c_str(), Py_file_input, pyBorrowed_globals, pyBorrowed_locals, 1, NULL);
    // if (result == NULL)
    //     return OrbitalError{OrbitalError::IMPORT};
    FILE* fp = fopen(this->m_file.c_str(), "r");
    int result = PyRun_AnyFile(fp, this->m_file.c_str());
    fclose(fp);
    if (result != 0)
        return OrbitalError{OrbitalError::IMPORT};
    // Py_DECREF(result);
    return OrbitalError{OrbitalError::NONE};
}


}
