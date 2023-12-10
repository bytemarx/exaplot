#include "orbital.hpp"

#include <cstdio>


namespace orbital {


ScriptModule::ScriptModule(const std::filesystem::path& file)
    : m_file{file}
    , m_pyOwned_module{NULL}
{
}


ScriptModule::~ScriptModule()
{
    Py_XDECREF(this->m_pyOwned_module);
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

    PyObject* codeObject = Py_CompileString(code, this->m_file.c_str(), Py_file_input);
    free(code);
    if (codeObject == NULL)
        return OrbitalError{OrbitalError::IMPORT};

    this->m_pyOwned_module = PyImport_ExecCodeModule("__orbital__", codeObject);
    Py_DECREF(codeObject);
    if (this->m_pyOwned_module == NULL)
        return OrbitalError{OrbitalError::IMPORT};

    return OrbitalError{OrbitalError::NONE};
}


}
