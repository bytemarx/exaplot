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


ModuleTest::ModuleTest()
    : iface{new Interface{this}}
    , core{this->iface}
    , scriptsDir{TEST_SCRIPTS_DIR}
{
}


ModuleTest::~ModuleTest()
{
    delete this->iface;
}


Interface::Interface(ModuleTest* tester)
    : m_tester{tester}
{
}

// orbital.init("a", "b", "c", "d")

TEST_F(ModuleTest, TestInit)
{
    std::unique_ptr<ScriptModule> mod;
    auto status = this->core.load(this->scriptsDir / "test-basic-init.py", mod);
    ASSERT_TRUE(status == OrbitalError::NONE);
}

PyObject*
Interface::init(
    const std::vector<std::string>& params,
    const std::vector<orbital::GridPoint>& plots) const
{
    this->m_tester->init(params, plots);
    Py_RETURN_NONE;
}

void
ModuleTest::init(
    const std::vector<std::string>& params,
    const std::vector<orbital::GridPoint>& plots)
{
    ASSERT_EQ(params.size(), 4);
    const char* expected[] = {
        "a",
        "b",
        "c",
        "d"
    };
    std::size_t i = 0;
    for (const auto& param : params) {
        ASSERT_STREQ(param.c_str(), expected[i++]);
    }
}

// orbital.msg("test", append=False)

TEST_F(ModuleTest, TestMsg)
{
    std::unique_ptr<ScriptModule> mod;
    auto status = this->core.load(this->scriptsDir / "test-basic-msg.py", mod);
    ASSERT_TRUE(status == OrbitalError::NONE);
}

PyObject*
Interface::msg(const std::string& message, bool append) const
{
    this->m_tester->msg(message, append);
    Py_RETURN_NONE;
}

void
ModuleTest::msg(const std::string& message, bool append)
{
    ASSERT_STREQ(message.c_str(), "test");
    ASSERT_FALSE(append);
}

// orbital.plot

PyObject*
Interface::plot(long dataSet, const std::vector<double>& data) const
{
    Py_RETURN_NONE;
}

// orbital.plotVec

PyObject*
Interface::plotVec(long dataSet, const std::vector<std::vector<double>>& data) const
{
    Py_RETURN_NONE;
}

// orbital.clear

PyObject*
Interface::clear(long dataSet) const
{
    Py_RETURN_NONE;
}


}
}
