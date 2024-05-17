#include "test.hpp"
#include "test-config.h"

#include <filesystem>
#include <fstream>
#include <iostream>
#include <mutex>
#include <string>
#include <utility>
#include <vector>


namespace exatest {


class ScriptTest : public ::testing::Test
{
protected:
    class Interface : public exa::Interface
    {
    public:
        PyObject* init(const std::vector<exa::RunParam>&, const std::vector<exa::GridPoint>&) override { Py_RETURN_NONE; }
        PyObject* stop() override { Py_RETURN_NONE; }
        PyObject* msg(const std::string&, bool) override { Py_RETURN_NONE; }
        PyObject* plot2D(std::size_t, double, double, bool) override { Py_RETURN_NONE; }
        PyObject* plot2DVec(std::size_t, const std::vector<double>&, const std::vector<double>&, bool) override { Py_RETURN_NONE; }
        PyObject* plotCM(std::size_t, int, int, double, bool) override { Py_RETURN_NONE; }
        PyObject* plotCMVec(std::size_t, int, const std::vector<double>&, bool) override { Py_RETURN_NONE; }
        PyObject* plotCMFrame(std::size_t, const std::vector<std::vector<double>>&, bool) override { Py_RETURN_NONE; }
        PyObject* clear(std::size_t) override { Py_RETURN_NONE; }
        PyObject* setPlotProperty(std::size_t, const exa::PlotProperty&, const exa::PlotProperty::Value& value) override { Py_RETURN_NONE; }
        PyObject* getPlotProperty(std::size_t, const exa::PlotProperty&) override { Py_RETURN_NONE; }
        PyObject* showPlot(std::size_t, std::size_t) override { Py_RETURN_NONE; }
        Py_ssize_t currentPlotType(std::size_t) override { return 0; }

    private:
        ModuleTest* m_tester;
    };
    ScriptTest() {};
};


TEST_F(ScriptTest, RunBasic)
{
    auto iface = new Interface;
    auto core = new exa::Core{iface};
    std::shared_ptr<exa::ScriptModule> mod;
    auto status = core->load(TEST_SCRIPTS_DIR "/run/basic.py", mod);
    ASSERT_FALSE(status) << status.message() << '\n' << status.traceback();
    status = mod->run({});
    ASSERT_FALSE(status) << status.message() << '\n' << status.traceback();
    delete core;
    delete iface;
}


TEST_F(ScriptTest, RunArgs)
{
    auto iface = new Interface;
    auto core = new exa::Core{iface};
    std::shared_ptr<exa::ScriptModule> mod;
    auto status = core->load(TEST_SCRIPTS_DIR "/run/args.py", mod);
    ASSERT_FALSE(status) << status.message() << '\n' << status.traceback();
    status = mod->run({
        {
            .identifier = "a",
            .type = exa::RunParamType::STRING,
            .value = "1"
        },
        {
            .identifier = "b",
            .type = exa::RunParamType::STRING,
            .value = "2"
        },
        {
            .identifier = "c",
            .type = exa::RunParamType::STRING,
            .value = "3"
        }
    });
    ASSERT_FALSE(status) << status.message() << '\n' << status.traceback();
    delete core;
    delete iface;
}


}
