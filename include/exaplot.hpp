/*
 * ExaPlot
 * library api
 * 
 * SPDX-License-Identifier: GPL-3.0
 * Copyright (C) 2024 bytemarx
 */

#pragma once

#define PY_SSIZE_T_CLEAN
#if defined(_WIN32)
    #pragma warning(push, 0)
#endif
#include <Python.h>
#if defined(_WIN32)
    #pragma warning(pop)
#endif

#include <filesystem>
#include <map>
#include <memory>
#include <string>
#include <vector>

#include "plotproperty.hpp"


#define EXA_MODULE     "_exaplot"
#define EXA_RUNPARAM   "RunParam"
#define EXA_INTERRUPT  "_Interrupt"

#define EXA_INIT       "init"                  // init(plots = 1, **params)
#define EXA_STOP       "stop"                  // stop()
#define EXA_MSG        "msg"                   // msg(message, append = False)
#define EXA_PLOT       "plot"                  // plot(data_set, *data)
#define EXA_SET_PLOT   "_set_plot_property"    // _set_plot_property(plot_id, prop, value)
#define EXA_GET_PLOT   "_get_plot_property"    // _get_plot_property(plot_id, prop)
#define EXA_SHOW_PLOT  "_show_plot"            // _show_plot(plot_id, plot_type)

#define EXA_SCRIPT_MODULE  "__exa__"
#define EXA_SCRIPT_RUN     "run"           // run(**kwargs)


namespace exa {


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


struct Error
{
    typedef const char* Type;
    EXA_API constexpr static Type NONE = "NONE";
    EXA_API constexpr static Type IMPORT = "IMPORT";
    EXA_API constexpr static Type RUNTIME = "RUNTIME";
    EXA_API constexpr static Type RELOAD = "RELOAD";
    EXA_API constexpr static Type SYSTEM = "SYSTEM";
    EXA_API constexpr static Type ARGUMENT = "ARGUMENT";
    EXA_API constexpr static Type INTERRUPT = "INTERRUPT";
    EXA_API constexpr static Type UNDEFINED = "UNDEFINED";

    static Error pyerror(Type);

    EXA_API Error(
        const Type type = UNDEFINED,
        const std::string& msg = "",
        const std::string& tb = "");

    EXA_API explicit operator bool() const noexcept { return strcmp(this->m_type, NONE) != 0; }
    EXA_API bool operator==(const Type& other) const noexcept { return strcmp(this->m_type, other) == 0; }
    EXA_API bool operator!=(const Type& other) const noexcept { return strcmp(this->m_type, other) != 0; }
    EXA_API auto type() const { return this->m_type; }
    EXA_API auto message() const { return this->m_message; }
    EXA_API auto traceback() const { return this->m_traceback; }

private:
    Type m_type;
    std::string m_message;
    std::string m_traceback;
};


class Interface
{
public:
    EXA_API virtual ~Interface();
    EXA_API virtual PyObject* init(const std::vector<RunParam>& params, const std::vector<GridPoint>& plots) = 0;
    EXA_API virtual PyObject* stop() = 0;
    EXA_API virtual PyObject* msg(const std::string& message, bool append) = 0;
    EXA_API virtual PyObject* plot2D(std::size_t plotID, double x, double y, bool write) = 0;
    EXA_API virtual PyObject* plot2DVec(std::size_t plotID, const std::vector<double>& x, const std::vector<double>& y, bool write) = 0;
    EXA_API virtual PyObject* plotCM(std::size_t plotID, int x, int y, double value, bool write) = 0;
    EXA_API virtual PyObject* plotCMVec(std::size_t plotID, int y, const std::vector<double>& values, bool write) = 0;
    EXA_API virtual PyObject* plotCMFrame(std::size_t plotID, const std::vector<std::vector<double>>& frame, bool write) = 0;
    EXA_API virtual PyObject* clear(std::size_t plotID) = 0;
    EXA_API virtual PyObject* setPlotProperty(std::size_t plotID, const PlotProperty& property, const PlotProperty::Value& value) = 0;
    EXA_API virtual PyObject* getPlotProperty(std::size_t plotID, const PlotProperty& property) = 0;
    EXA_API virtual PyObject* showPlot(std::size_t plotID, std::size_t plotType) = 0;
    EXA_API virtual Py_ssize_t currentPlotType(std::size_t plotID) = 0;
};


class ScriptModule;


class Core
{
public:
    EXA_API static PyStatus init(
        const std::filesystem::path& executable,
        const std::filesystem::path& prefix,
        const std::vector<std::filesystem::path>& searchPaths = {});
    EXA_API static int deinit();

    EXA_API Core(Interface* interface);
    EXA_API ~Core();
    EXA_API Core(const Core&) = delete;

    EXA_API Error load(const std::filesystem::path& file, std::shared_ptr<ScriptModule>& module);

private:
    static std::size_t coreCount;
    static PyThreadState* mainThreadState;

    const Interface* const m_interface;
    PyThreadState* m_tState;
    std::vector<std::weak_ptr<ScriptModule>> m_scripts;
};


class ScriptModule
{
    friend class Core;

public:
    EXA_API ~ScriptModule();
    EXA_API Error reload();
    EXA_API Error run(const std::vector<RunParam>& args);

private:
    ScriptModule(PyThreadState* tState, const std::filesystem::path& file);
    void ensureThreadState();
    Error load();

    PyThreadState* m_tState;
    std::filesystem::path m_file;
    PyObject* m_pyOwned_module;
};


}
