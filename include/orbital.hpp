#pragma once

#define PY_SSIZE_T_CLEAN
#include <Python.h>

#include <filesystem>
#include <functional>
#include <map>
#include <memory>
#include <string>


#define ORBITAL_MODULE  "_orbital"

#define ORBITAL_INIT    "init"  // orbital.init(*params, plots = 1)
#define ORBITAL_MSG     "msg"   // orbital.msg(message, append = False)
#define ORBITAL_PLOT    "plot"  // orbital.plot(data_set, *data)

#define ORBITAL_SCRIPT_INIT "init"  // init()
#define ORBITAL_SCRIPT_RUN  "run"   // run(**kwargs)


namespace orbital {


typedef std::size_t GridPoint_t;
typedef struct s_gridpoint {
    GridPoint_t x;
    GridPoint_t dx;
    GridPoint_t y;
    GridPoint_t dy;
} GridPoint;


struct OrbitalError
{
    typedef const char* Type;
    constexpr static Type NONE = "NONE";
    constexpr static Type IMPORT = "IMPORT";
    constexpr static Type RUNTIME = "RUNTIME";
    constexpr static Type RELOAD = "RELOAD";
    constexpr static Type SYSTEM = "SYSTEM";
    constexpr static Type UNDEFINED = "UNDEFINED";

    static OrbitalError pyerror(Type);

    OrbitalError(
        const Type type = UNDEFINED,
        const std::string& msg = "",
        const std::string& tb = "");

    explicit operator bool() const noexcept { return strcmp(this->m_type, NONE) != 0; }
    // bool operator==(const OrbitalError::Type& type) const { return strcmp(this->m_type, type) == 0; }
    // bool operator!=(const OrbitalError::Type& type) const { return !this->operator==(type); }
    auto type() const { return this->m_type; }
    auto message() const { return this->m_message; }
    auto traceback() const { return this->m_traceback; }

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

    OrbitalError load(const std::filesystem::path& file, std::shared_ptr<ScriptModule>& module);

private:
    static ssize_t coreCount;
    static PyThreadState* mainThreadState;

    std::string traceback(PyObject* tb) const;

    const OrbitalInterface* const m_interface;
    PyThreadState* m_tState;
    std::vector<std::weak_ptr<ScriptModule>> m_scripts;
};


class ScriptModule
{
    friend class OrbitalCore;

public:
    ~ScriptModule();
    OrbitalError reload();
    OrbitalError run(const std::map<std::string, std::string>& kwargs = {});

private:
    ScriptModule(PyThreadState* tState, const std::filesystem::path& file);
    void ensureThreadState();
    OrbitalError load();

    PyThreadState* m_tState;
    std::filesystem::path m_file;
    PyObject* m_pyOwned_module;
};


}