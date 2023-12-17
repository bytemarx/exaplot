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
        PyObject* init(const std::vector<std::string>& params, const std::vector<orbital::GridPoint>& plots) const override { Py_RETURN_NONE; }
        PyObject* msg(const std::string& message, bool append) const override { Py_RETURN_NONE; }
        PyObject* plot(long dataSet, const std::vector<double>& data) const override { Py_RETURN_NONE; }
        PyObject* plotVec(long dataSet, const std::vector<std::vector<double>>& data) const override { Py_RETURN_NONE; }
        PyObject* clear(long dataSet) const override { Py_RETURN_NONE; }
    };
    ScriptTest() {};
};


TEST_F(ScriptTest, RunBasic)
{
    Interface iface;
    OrbitalCore core{&iface};
    std::shared_ptr<ScriptModule> mod;
    auto status = core.load(TEST_SCRIPTS_DIR "/run/basic.py", mod);
    ASSERT_TRUE(status == OrbitalError::NONE) << status.message() << '\n' << status.traceback();
    status = mod->run();
    ASSERT_TRUE(status == OrbitalError::NONE) << status.message() << '\n' << status.traceback();
}


TEST_F(ScriptTest, RunArgs)
{
    Interface iface;
    OrbitalCore core{&iface};
    std::shared_ptr<ScriptModule> mod;
    auto status = core.load(TEST_SCRIPTS_DIR "/run/args.py", mod);
    ASSERT_TRUE(status == OrbitalError::NONE) << status.message() << '\n' << status.traceback();
    status = mod->run({{"a", "1"}, {"b", "2"}, {"c", "3"}});
    ASSERT_TRUE(status == OrbitalError::NONE) << status.message() << '\n' << status.traceback();
}


TEST_F(ScriptTest, TestStop)
{
    class StopInterface : public OrbitalInterface
    {
    public:
        PyObject* init(const std::vector<std::string>& params, const std::vector<orbital::GridPoint>& plots) const override { Py_RETURN_NONE; }
        PyObject* msg(const std::string& message, bool append) const override { Py_RETURN_NONE; }
        PyObject* plot(long dataSet, const std::vector<double>& data) const override { Py_RETURN_NONE; }
        PyObject* plotVec(long dataSet, const std::vector<std::vector<double>>& data) const override { Py_RETURN_NONE; }
        PyObject* clear(long dataSet) const override
        {
            mod->stop();
            Py_RETURN_NONE;
        }

        std::shared_ptr<ScriptModule> mod;
    };
    StopInterface iface;
    OrbitalCore core{&iface};
    auto status = core.load(TEST_SCRIPTS_DIR "/stop/stop.py", iface.mod);
    ASSERT_TRUE(status == OrbitalError::NONE) << status.message() << '\n' << status.traceback();
}


TEST_F(ScriptTest, TestStopInvalid)
{
    Interface iface;
    OrbitalCore core{&iface};
    std::shared_ptr<ScriptModule> mod;
    auto status = core.load(TEST_SCRIPTS_DIR "/stop/invalid.py", mod);
    ASSERT_TRUE(status == OrbitalError::NONE) << status.message() << '\n' << status.traceback();
}


}
}
