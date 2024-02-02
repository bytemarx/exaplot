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


OrbitalEnvironment::~OrbitalEnvironment()
{
}


void
OrbitalEnvironment::SetUp()
{
    std::filesystem::path executable = std::filesystem::canonical("/proc/self/exe");
    std::filesystem::path prefix = executable.parent_path() / "python";
    std::cout << "Prefix path: " << prefix << '\n';

    PyStatus status = OrbitalCore::init(executable, prefix);
    ASSERT_FALSE(PyStatus_Exception(status));
}


void
OrbitalEnvironment::TearDown()
{
    ASSERT_EQ(OrbitalCore::deinit(), 0);
}


ModuleTest::ModuleTest()
    : iface{new ModuleTest::Interface{this}}
    , core{this->iface}
    , scriptsDir{TEST_SCRIPTS_DIR}
{
}


ModuleTest::~ModuleTest()
{
    delete this->iface;
}


void
ModuleTest::run(const char* file)
{
    std::shared_ptr<ScriptModule> mod;
    auto error = this->core.load(this->scriptsDir / file, mod);
    ASSERT_FALSE(error) << error.message() << '\n' << error.traceback();
}


ModuleTest::Interface::Interface(ModuleTest* tester)
    : m_tester{tester}
{
}


PyObject*
ModuleTest::Interface::init(
    const std::vector<std::string>& params,
    const std::vector<orbital::GridPoint>& plots) const
{
    this->m_tester->init(params, plots);
    Py_RETURN_NONE;
}


PyObject*
ModuleTest::Interface::msg(const std::string& message, bool append) const
{
    this->m_tester->msg(message, append);
    Py_RETURN_NONE;
}


PyObject*
ModuleTest::Interface::plot(long dataSet, const std::vector<double>& data) const
{
    this->m_tester->plot(dataSet, data);
    Py_RETURN_NONE;
}


PyObject*
ModuleTest::Interface::plotVec(long dataSet, const std::vector<std::vector<double>>& data) const
{
    this->m_tester->plotVec(dataSet, data);
    Py_RETURN_NONE;
}


PyObject*
ModuleTest::Interface::clear(long dataSet) const
{
    this->m_tester->clear(dataSet);
    Py_RETURN_NONE;
}


class BaselineTest : public ::testing::Test
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
        PyObject* setPlotProperty(
            long plotID, const std::string& property,
            const std::variant<int, double, std::string>& value) const override { Py_RETURN_NONE; }
        PyObject* getPlotProperty(long plotID, const std::string& property) const override { Py_RETURN_NONE; }

    private:
        ModuleTest* m_tester;
    };
    BaselineTest() {};
};


TEST_F(BaselineTest, Init)
{
    ASSERT_TRUE(true);
}


TEST_F(BaselineTest, Instantiate)
{
    Interface* iface = new Interface;
    OrbitalCore* core = new OrbitalCore{iface};
    delete core;
    delete iface;
}


TEST_F(BaselineTest, DoubleInstantiate)
{
    Interface* iface = new Interface;
    OrbitalCore* core0 = new OrbitalCore{iface};
    OrbitalCore* core1 = new OrbitalCore{iface};
    delete core0;
    delete core1;
    delete iface;
}


TEST_F(BaselineTest, InterleaveLoads)
{
    Interface* iface = new Interface;
    OrbitalCore* core0 = new OrbitalCore{iface};
    OrbitalCore* core1 = new OrbitalCore{iface};
    for (int i = 0; i < 2; ++i) {
        std::shared_ptr<ScriptModule> mod0;
        std::shared_ptr<ScriptModule> mod1;
        {
            auto error = core0->load(TEST_SCRIPTS_DIR "/baseline/isolated-interp-0.py", mod0);
            ASSERT_FALSE(error) << error.message() << '\n' << error.traceback();
        }
        {
            auto error = core1->load(TEST_SCRIPTS_DIR "/baseline/isolated-interp-1.py", mod1);
            ASSERT_FALSE(error) << error.message() << '\n' << error.traceback();
        }
    }
    delete core0;
    delete core1;
    delete iface;
}


TEST_F(BaselineTest, Reload)
{
    Interface* iface = new Interface;
    OrbitalCore* core = new OrbitalCore{iface};
    std::shared_ptr<ScriptModule> mod;
    {
        std::ofstream file(TEST_SCRIPTS_DIR "/baseline/reload.py");
        file << "";
        file.close();
        auto error = core->load(TEST_SCRIPTS_DIR "/baseline/reload.py", mod);
        ASSERT_FALSE(error) << error.message() << '\n' << error.traceback();
    }
    {
        std::ofstream file;
        file.open(TEST_SCRIPTS_DIR "/baseline/reload.py");
        file << "assert(False)";
        file.close();
        auto error = mod->reload();
        ASSERT_TRUE(error);
        ASSERT_STREQ(error.traceback().c_str(), "  File \"" TEST_SCRIPTS_DIR "/baseline/reload.py\", line 1, in <module>\n    assert(False)\n");
    }
    delete core;
    delete iface;
}


TEST_F(BaselineTest, NotIsolatedWithinCore)
{
    Interface* iface = new Interface;
    OrbitalCore* core = new OrbitalCore{iface};
    std::shared_ptr<ScriptModule> mod0;
    std::shared_ptr<ScriptModule> mod1;
    auto error = core->load(TEST_SCRIPTS_DIR "/baseline/reload2-0.py", mod0);
    ASSERT_FALSE(error) << error.message() << '\n' << error.traceback();
    error = core->load(TEST_SCRIPTS_DIR "/baseline/reload2-1.py", mod1);
    ASSERT_FALSE(error) << error.message() << '\n' << error.traceback();
    delete core;
    delete iface;
}


}
}


int main(int argc, char** argv) {
    ::testing::InitGoogleTest(&argc, argv);
    ::testing::AddGlobalTestEnvironment(new orbital::testing::OrbitalEnvironment);
    return RUN_ALL_TESTS();
}
