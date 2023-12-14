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

    private:
        ModuleTest* m_tester;
    };
    ScriptTest() {};
};


TEST_F(ScriptTest, InitBasic)
{
    Interface* iface = new Interface;
    OrbitalCore* core = new OrbitalCore{iface};
    std::shared_ptr<ScriptModule> mod;
    auto status = core->load(TEST_SCRIPTS_DIR "/init/basic.py", mod);
    ASSERT_TRUE(status == OrbitalError::NONE) << status.message() << '\n' << status.traceback();
    status = mod->init();
    ASSERT_TRUE(status == OrbitalError::NONE) << status.message() << '\n' << status.traceback();
    delete core;
    delete iface;
}


TEST_F(ScriptTest, InitArgs)
{
    Interface* iface = new Interface;
    OrbitalCore* core = new OrbitalCore{iface};
    std::shared_ptr<ScriptModule> mod;
    auto status = core->load(TEST_SCRIPTS_DIR "/init/args.py", mod);
    ASSERT_TRUE(status == OrbitalError::NONE) << status.message() << '\n' << status.traceback();
    status = mod->init({{"a", "1"}, {"b", "2"}, {"c", "3"}});
    ASSERT_TRUE(status == OrbitalError::NONE) << status.message() << '\n' << status.traceback();
    delete core;
    delete iface;
}


}
}
