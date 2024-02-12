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
        PyObject* init(const std::vector<std::string>& params, const std::vector<orbital::GridPoint>& plots) override { Py_RETURN_NONE; }
        PyObject* msg(const std::string& message, bool append) override { Py_RETURN_NONE; }
        PyObject* plot(long dataSet, const std::vector<double>& data) override { Py_RETURN_NONE; }
        PyObject* plotVec(long dataSet, const std::vector<std::vector<double>>& data) override { Py_RETURN_NONE; }
        PyObject* clear(long dataSet) override { Py_RETURN_NONE; }
        PyObject* setPlotProperty(long plotID, const PlotProperty& property, const PlotProperty::Value& value) override { Py_RETURN_NONE; }
        PyObject* getPlotProperty(long plotID, const PlotProperty& property) override { Py_RETURN_NONE; }
        PyObject* showPlot(long plotID, std::size_t plotType) override { Py_RETURN_NONE; }

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
    status = mod->run();
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
    status = mod->run({{"a", "1"}, {"b", "2"}, {"c", "3"}});
    ASSERT_FALSE(status) << status.message() << '\n' << status.traceback();
    delete core;
    delete iface;
}


}
}
