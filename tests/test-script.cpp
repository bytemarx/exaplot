#include "test.hpp"
#include "test-config.h"

#include <filesystem>
#include <fstream>
#include <iostream>
#include <mutex>
#include <string>
#include <utility>
#include <vector>


namespace orbital {
namespace testing {


class ScriptTest : public ::testing::Test
{
protected:
    class Interface : public OrbitalInterface
    {
    public:
        PyObject* init(const std::vector<orbital::RunParam>&, const std::vector<orbital::GridPoint>&) override { Py_RETURN_NONE; }
        PyObject* stop() override { Py_RETURN_NONE; }
        PyObject* msg(const std::string&, bool) override { Py_RETURN_NONE; }
        PyObject* plot2D(std::size_t, double, double) override { Py_RETURN_NONE; }
        PyObject* plot2DVec(std::size_t, const std::vector<double>&, const std::vector<double>&) override { Py_RETURN_NONE; }
        PyObject* plotCM(std::size_t, int, int, double) override { Py_RETURN_NONE; }
        PyObject* plotCMVec(std::size_t, int, const std::vector<double>&) override { Py_RETURN_NONE; }
        PyObject* plotCMFrame(std::size_t, const std::vector<std::vector<double>>&) override { Py_RETURN_NONE; }
        PyObject* clear(std::size_t) override { Py_RETURN_NONE; }
        PyObject* setPlotProperty(std::size_t, const PlotProperty&, const PlotProperty::Value& value) override { Py_RETURN_NONE; }
        PyObject* getPlotProperty(std::size_t, const PlotProperty&) override { Py_RETURN_NONE; }
        PyObject* showPlot(std::size_t, std::size_t) override { Py_RETURN_NONE; }
        Py_ssize_t currentPlotType(std::size_t) override { return 0; }

    private:
        ModuleTest* m_tester;
    };
    ScriptTest() {};
};


TEST_F(ScriptTest, RunBasic)
{
    Interface* iface = new Interface;
    OrbitalCore* core = new OrbitalCore{iface};
    std::shared_ptr<ScriptModule> mod;
    auto status = core->load(TEST_SCRIPTS_DIR "/run/basic.py", mod);
    ASSERT_FALSE(status) << status.message() << '\n' << status.traceback();
    status = mod->run({});
    ASSERT_FALSE(status) << status.message() << '\n' << status.traceback();
    delete core;
    delete iface;
}


TEST_F(ScriptTest, RunArgs)
{
    Interface* iface = new Interface;
    OrbitalCore* core = new OrbitalCore{iface};
    std::shared_ptr<ScriptModule> mod;
    auto status = core->load(TEST_SCRIPTS_DIR "/run/args.py", mod);
    ASSERT_FALSE(status) << status.message() << '\n' << status.traceback();
    status = mod->run({
        {
            .identifier = "a",
            .type = RunParamType::STRING,
            .value = "1"
        },
        {
            .identifier = "b",
            .type = RunParamType::STRING,
            .value = "2"
        },
        {
            .identifier = "c",
            .type = RunParamType::STRING,
            .value = "3"
        }
    });
    ASSERT_FALSE(status) << status.message() << '\n' << status.traceback();
    delete core;
    delete iface;
}


}
}
