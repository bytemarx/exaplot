#pragma once

#define PY_SSIZE_T_CLEAN
#include <Python.h>

#include <filesystem>
#include <functional>
#include <map>
#include <memory>
#include <string>


#define ORBITAL_MODULE  "_orbital"

#define ORBITAL_INIT    "init"  // orbital.init(*params, plots = 1) -> None
#define ORBITAL_MSG     "msg"   // orbital.msg(message, append = False) -> None
#define ORBITAL_PLOT    "plot"  // orbital.plot(data_set, *data) -> None
#define ORBITAL_STOP    "stop"  // orbital.stop(module_name) -> bool

#define ORBITAL_MAX_PLOTS 64

#define ORBITAL_SCRIPT_MODULE   "__orbital__"
#define ORBITAL_SCRIPT_INIT     "init"  // init() -> None
#define ORBITAL_SCRIPT_RUN      "run"   // run(**kwargs) -> None


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
        RELOAD,
        SYSTEM,
        UNDEFINED
    };

    static OrbitalError pyerror(Type);

    OrbitalError(
        const Type type = UNDEFINED,
        const std::string& msg = "",
        const std::string& tb = "");

    bool operator==(const OrbitalError::Type& type) const { return this->m_type == type; }
    bool operator!=(const OrbitalError::Type& type) const { return !this->operator==(type); }
    std::string message() const { return this->m_message; }
    std::string traceback() const { return this->m_traceback; }

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
    class _OrbIFace
    {
        friend class OrbitalCore;
    public:
        PyObject* init(const std::vector<std::string>&, const std::vector<GridPoint>&) const;
        PyObject* msg(const std::string&, bool) const;
        PyObject* plot(long, const std::vector<double>&) const;
        PyObject* plotVec(long, const std::vector<std::vector<double>>&) const;
        PyObject* clear(long) const;
        PyObject* stop(std::size_t) const;

    private:
        _OrbIFace(const OrbitalCore* const, const OrbitalInterface* const);

        const OrbitalCore* const m_core;
        const OrbitalInterface* const m_iface;
    };

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
    PyObject* scriptStop(std::size_t id) const;

    const _OrbIFace* const m_interface;
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
    void stop();

private:
    ScriptModule(PyThreadState* tState, const std::filesystem::path& file, std::size_t id);
    void ensureThreadState();
    OrbitalError load();

    PyThreadState* m_tState;
    std::filesystem::path m_file;
    PyObject* m_pyOwned_module;
    const std::size_t m_id;
    const std::string m_moduleName;
    bool m_haltFlag;
};


}