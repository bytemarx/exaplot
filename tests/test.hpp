#pragma once

#include "gtest/gtest.h"
#include "orbital.hpp"


namespace orbital {
namespace testing {


class OrbitalEnvironment : public ::testing::Environment
{
public:
    ~OrbitalEnvironment() override;
    void SetUp() override;
    void TearDown() override;
};


class Interface;
class ModuleTest : public ::testing::Test
{
public:
    virtual void init(const std::vector<std::string>& params, const std::vector<orbital::GridPoint>& plots) = 0;
    virtual void msg(const std::string& message, bool append) = 0;
    virtual void plot2D(std::size_t plotID, double x, double y) = 0;
    virtual void plot2DVec(std::size_t plotID, const std::vector<double>& x, const std::vector<double>& y) = 0;
    virtual void clear(std::size_t plotID) = 0;

private:
    class Interface : public OrbitalInterface
    {
    public:
        Interface(ModuleTest* tester);
        PyObject* init(const std::vector<std::string>&, const std::vector<orbital::GridPoint>&) override;
        PyObject* msg(const std::string&, bool) override;
        PyObject* plot2D(std::size_t, double, double) override;
        PyObject* plot2DVec(std::size_t, const std::vector<double>&, const std::vector<double>&) override;
        PyObject* plotCM(std::size_t, int, int, double) override { Py_RETURN_NONE; }
        PyObject* plotCMVec(std::size_t, int, const std::vector<double>&) override { Py_RETURN_NONE; }
        PyObject* plotCMFrame(std::size_t, const std::vector<std::vector<double>>&) override { Py_RETURN_NONE; }
        PyObject* clear(std::size_t) override;
        PyObject* setPlotProperty(std::size_t, const PlotProperty&, const PlotProperty::Value&) override { Py_RETURN_NONE; }
        PyObject* getPlotProperty(std::size_t, const PlotProperty&) override { Py_RETURN_NONE; }
        PyObject* showPlot(std::size_t, std::size_t) override { Py_RETURN_NONE; }
        Py_ssize_t currentPlotType(std::size_t) override { return 0; }

    private:
        ModuleTest* m_tester;
    };

protected:
    ModuleTest();
    virtual ~ModuleTest();
    void run(const char* file);

    Interface* iface;
    OrbitalCore core;
    std::filesystem::path scriptsDir;
};


}
}
