#pragma once

#define PY_SSIZE_T_CLEAN
#include <Python.h>

#include <filesystem>
#include <map>
#include <memory>
#include <string>
#include <variant>
#include <vector>


#define ORBITAL_MODULE  "_orbital"

#define ORBITAL_INIT        "init"                  // orbital.init(*params, plots = 1)
#define ORBITAL_MSG         "msg"                   // orbital.msg(message, append = False)
#define ORBITAL_PLOT        "plot"                  // orbital.plot(data_set, *data)
#define ORBITAL_SET_PLOT    "_set_plot_property"    // orbital._set_plot_property(plot_id, prop, value)
#define ORBITAL_GET_PLOT    "_get_plot_property"    // orbital._get_plot_property(plot_id, prop)

#define ORBITAL_SCRIPT_INIT "init"  // init()
#define ORBITAL_SCRIPT_RUN  "run"   // run(**kwargs)

#define ORBITAL_PLOT_PROPERTY_TITLE                 "title"
#define ORBITAL_PLOT_PROPERTY_XAXIS                 "x_axis"
#define ORBITAL_PLOT_PROPERTY_YAXIS                 "y_axis"
#define ORBITAL_PLOT_PROPERTY_MINSIZE_W             "min_size.w"
#define ORBITAL_PLOT_PROPERTY_MINSIZE_H             "min_size.h"
#define ORBITAL_PLOT_PROPERTY_TWODIMEN_XRANGE_MIN   "two_dimen.x_range.min"
#define ORBITAL_PLOT_PROPERTY_TWODIMEN_XRANGE_MAX   "two_dimen.x_range.max"
#define ORBITAL_PLOT_PROPERTY_TWODIMEN_YRANGE_MIN   "two_dimen.y_range.min"
#define ORBITAL_PLOT_PROPERTY_TWODIMEN_YRANGE_MAX   "two_dimen.y_range.max"
#define ORBITAL_PLOT_PROPERTY_TWODIMEN_LINE_TYPE    "two_dimen.line.type"
#define ORBITAL_PLOT_PROPERTY_TWODIMEN_LINE_COLOR   "two_dimen.line.color"
#define ORBITAL_PLOT_PROPERTY_TWODIMEN_LINE_STYLE   "two_dimen.line.style"
#define ORBITAL_PLOT_PROPERTY_TWODIMEN_POINTS_SHAPE "two_dimen.points.color"
#define ORBITAL_PLOT_PROPERTY_TWODIMEN_POINTS_COLOR "two_dimen.points.shape"
#define ORBITAL_PLOT_PROPERTY_TWODIMEN_POINTS_SIZE  "two_dimen.points.size"
#define ORBITAL_PLOT_PROPERTY_COLORMAP_XRANGE_MIN   "color_map.x_range.min"
#define ORBITAL_PLOT_PROPERTY_COLORMAP_XRANGE_MAX   "color_map.x_range.max"
#define ORBITAL_PLOT_PROPERTY_COLORMAP_YRANGE_MIN   "color_map.y_range.min"
#define ORBITAL_PLOT_PROPERTY_COLORMAP_YRANGE_MAX   "color_map.y_range.max"
#define ORBITAL_PLOT_PROPERTY_COLORMAP_ZRANGE_MIN   "color_map.z_range.min"
#define ORBITAL_PLOT_PROPERTY_COLORMAP_ZRANGE_MAX   "color_map.z_range.max"
#define ORBITAL_PLOT_PROPERTY_COLORMAP_DATASIZE_X   "color_map.data_size.x"
#define ORBITAL_PLOT_PROPERTY_COLORMAP_DATASIZE_Y   "color_map.data_size.y"
#define ORBITAL_PLOT_PROPERTY_COLORMAP_COLOR_MIN    "color_map.color.min"
#define ORBITAL_PLOT_PROPERTY_COLORMAP_COLOR_MAX    "color_map.color.max"


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
    virtual PyObject* setPlotProperty(
        long plotID, const std::string& property, const std::variant<int, double, std::string>& value
    ) const = 0;
    virtual PyObject* getPlotProperty(long plotID, const std::string& property) const = 0;
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