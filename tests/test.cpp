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
    // ASSERT_EQ(OrbitalCore::deinit(), 0);
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


}
}


int main(int argc, char** argv) {
    ::testing::InitGoogleTest(&argc, argv);
    ::testing::AddGlobalTestEnvironment(new orbital::testing::OrbitalEnvironment);
    return RUN_ALL_TESTS();
}
