/*
 * ZetaPlot
 * library api
 * 
 * SPDX-License-Identifier: GPL-3.0
 * Copyright (C) 2024 bytemarx
 */

#pragma once

#define PY_SSIZE_T_CLEAN
#include <Python.h>

#include <filesystem>
#include <map>
#include <memory>
#include <string>
#include <vector>

#include "plotproperty.hpp"


#define ZETA_MODULE     "_zetaplot"
#define ZETA_RUNPARAM   "RunParam"
#define ZETA_INTERRUPT  "_Interrupt"

#define ZETA_INIT       "init"                  // init(plots = 1, **params)
#define ZETA_STOP       "stop"                  // stop()
#define ZETA_MSG        "msg"                   // msg(message, append = False)
#define ZETA_PLOT       "plot"                  // plot(data_set, *data)
#define ZETA_SET_PLOT   "_set_plot_property"    // _set_plot_property(plot_id, prop, value)
#define ZETA_GET_PLOT   "_get_plot_property"    // _get_plot_property(plot_id, prop)
#define ZETA_SHOW_PLOT  "_show_plot"            // _show_plot(plot_id, plot_type)

#define ZETA_SCRIPT_MODULE  "__zeta__"
#define ZETA_SCRIPT_RUN     "run"           // run(**kwargs)


namespace zeta {


typedef std::size_t GridPoint_t;
typedef struct {
    GridPoint_t x;
    GridPoint_t dx;
    GridPoint_t y;
    GridPoint_t dy;
} GridPoint;


enum RunParamType {
    STRING,
    INT,
    FLOAT,
};
typedef struct {
    std::string identifier;
    RunParamType type;
    std::string value;
    std::string display;
} RunParam;


struct ZETA_API Error
{
    typedef const char* Type;
    constexpr static Type NONE = "NONE";
    constexpr static Type IMPORT = "IMPORT";
    constexpr static Type RUNTIME = "RUNTIME";
    constexpr static Type RELOAD = "RELOAD";
    constexpr static Type SYSTEM = "SYSTEM";
    constexpr static Type ARGUMENT = "ARGUMENT";
    constexpr static Type INTERRUPT = "INTERRUPT";
    constexpr static Type UNDEFINED = "UNDEFINED";

    static Error pyerror(Type);

    Error(
        const Type type = UNDEFINED,
        const std::string& msg = "",
        const std::string& tb = "");

    explicit operator bool() const noexcept { return strcmp(this->m_type, NONE) != 0; }
    bool operator==(const Type& other) const noexcept { return strcmp(this->m_type, other) == 0; }
    bool operator!=(const Type& other) const noexcept { return strcmp(this->m_type, other) != 0; }
    auto type() const { return this->m_type; }
    auto message() const { return this->m_message; }
    auto traceback() const { return this->m_traceback; }

private:
    Type m_type;
    std::string m_message;
    std::string m_traceback;
};


class ZETA_API Interface
{
public:
    virtual ~Interface();
    virtual PyObject* init(const std::vector<RunParam>& params, const std::vector<GridPoint>& plots) = 0;
    virtual PyObject* stop() = 0;
    virtual PyObject* msg(const std::string& message, bool append) = 0;
    virtual PyObject* plot2D(std::size_t plotID, double x, double y) = 0;
    virtual PyObject* plot2DVec(std::size_t plotID, const std::vector<double>& x, const std::vector<double>& y) = 0;
    virtual PyObject* plotCM(std::size_t plotID, int x, int y, double value) = 0;
    virtual PyObject* plotCMVec(std::size_t plotID, int y, const std::vector<double>& values) = 0;
    virtual PyObject* plotCMFrame(std::size_t plotID, const std::vector<std::vector<double>>& frame) = 0;
    virtual PyObject* clear(std::size_t plotID) = 0;
    virtual PyObject* setPlotProperty(std::size_t plotID, const PlotProperty& property, const PlotProperty::Value& value) = 0;
    virtual PyObject* getPlotProperty(std::size_t plotID, const PlotProperty& property) = 0;
    virtual PyObject* showPlot(std::size_t plotID, std::size_t plotType) = 0;
    virtual Py_ssize_t currentPlotType(std::size_t plotID) = 0;
};


class ScriptModule;


class ZETA_API Core
{
public:
    static PyStatus init(
        const std::filesystem::path& executable,
        const std::filesystem::path& prefix);
    static int deinit();

    Core(Interface* interface);
    ~Core();
    Core(const Core&) = delete;

    Error load(const std::filesystem::path& file, std::shared_ptr<ScriptModule>& module);

private:
    static std::size_t coreCount;
    static PyThreadState* mainThreadState;

    const Interface* const m_interface;
    PyThreadState* m_tState;
    std::vector<std::weak_ptr<ScriptModule>> m_scripts;
};


class ZETA_API ScriptModule
{
    friend class Core;

public:
    ~ScriptModule();
    Error reload();
    Error run(const std::vector<RunParam>& args);

private:
    ScriptModule(PyThreadState* tState, const std::filesystem::path& file);
    void ensureThreadState();
    Error load();

    PyThreadState* m_tState;
    std::filesystem::path m_file;
    PyObject* m_pyOwned_module;
};


}
