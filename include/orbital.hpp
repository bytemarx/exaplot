#pragma once

#define PY_SSIZE_T_CLEAN
#include <Python.h>

#include <filesystem>
#include <functional>
#include <memory>
#include <string>


#define ORBITAL_MODULE  "_orbital"

#define ORBITAL_INIT    "init"  // orbital.init(*params, plots = 1)
#define ORBITAL_MSG     "msg"   // orbital.msg(message, append = False)
#define ORBITAL_PLOT    "plot"  // orbital.plot(data_set, *data)

#define ORBITAL_MAX_PLOTS 64


namespace orbital {


typedef struct s_gridpoint {
    int x;
    int dx;
    int y;
    int dy;
} GridPoint;


struct OrbitalError
{
    enum Type {
        NONE,
        IMPORT,
        RUNTIME,
        UNDEFINED
    };

    OrbitalError(
        const Type type = UNDEFINED,
        const std::string& msg = "",
        const std::string& tb = "")
        : m_type(type)
        , m_message(msg)
        , m_traceback(tb)
    {}

    bool operator==(const OrbitalError::Type& type) const { return this->m_type == type; }
    bool operator!=(const OrbitalError::Type& type) const { return !this->operator==(type); }

private:
    Type m_type;
    std::string m_message;
    std::string m_traceback;
};


class OrbitalInterface
{
public:
    virtual ~OrbitalInterface();
    virtual PyObject* init(const std::vector<std::string>& params, const std::vector<GridPoint>& plots) const = 0;
    virtual PyObject* msg(const std::string& message, bool append) const = 0;
    virtual PyObject* plot(long dataSet, const std::vector<double>& data) const = 0;
    virtual PyObject* plotVec(long dataSet, const std::vector<std::vector<double>>& data) const = 0;
    virtual PyObject* clear(long dataSet) const = 0;
};


typedef struct s_orbital_state {
    const OrbitalInterface* iface;
} orbital_state;


class ScriptModule;


class OrbitalCore
{
public:
    static PyStatus init(
        const std::filesystem::path& executable,
        const std::filesystem::path& prefix);
    static int deinit();

    OrbitalCore(const OrbitalInterface* interface);
    ~OrbitalCore();
    OrbitalCore(const OrbitalCore&) = delete;
    OrbitalCore(OrbitalCore&&) = delete;

    OrbitalError load(const std::filesystem::path& file, std::unique_ptr<ScriptModule>& module);

private:
    static ssize_t coreCount;

    std::string traceback(PyObject* tb) const;

    const OrbitalInterface* const m_interface;
    PyObject* m_pyOwned_formatTraceback;
    PyThreadState* m_tState;
};


class ScriptModule
{
    friend class OrbitalCore;

public:
    ~ScriptModule();
    OrbitalError reload();
    OrbitalError init();
    OrbitalError run();

private:
    ScriptModule(const std::filesystem::path& file);
    OrbitalError load();

    // PyObject* m_pyOwned_file;
    std::filesystem::path m_file;
    PyObject* m_pyOwned_globals;
    PyObject* m_pyOwned_locals;
    // PyObject* m_pyOwned_module;
};


}