#include "gtest/gtest.h"
#include "orbital.hpp"

#include <filesystem>
#include <fstream>
#include <iostream>
#include <mutex>
#include <string>
#include <utility>
#include <vector>


namespace orbital {
namespace testing {


class Interface : public orbital::OrbitalInterface
{
public:
    Interface()
    {
    }

    PyObject* init(
        const std::vector<std::string>& params,
        const std::vector<orbital::GridPoint>& plots) const
    {
        Py_RETURN_NONE;
    }

    PyObject* msg(const std::string& message, bool append) const
    {
        Py_RETURN_NONE;
    }

    PyObject* plot(long dataSet, const std::vector<double>& data) const
    {
        Py_RETURN_NONE;
    }

    PyObject* plotVec(long dataSet, const std::vector<std::vector<double>>& data) const
    {
        Py_RETURN_NONE;
    }

    PyObject* clear(long dataSet) const
    {
        Py_RETURN_NONE;
    }
};


class OrbitalEnvironment : public ::testing::Environment
{
public:
    ~OrbitalEnvironment() override {}

    void
    SetUp() override
    {
        std::wstring executable(L"orbital");
        std::filesystem::path prefix
            = std::filesystem::canonical("/proc/self/exe").parent_path() / "python";
        std::cout << "Prefix path: " << prefix << '\n';

        PyStatus status = orbital::OrbitalCore::init(executable, prefix);
        ASSERT_FALSE(PyStatus_Exception(status));
    }

    void
    TearDown() override
    {
        ASSERT_EQ(orbital::OrbitalCore::deinit(), 0);
    }
};


class ModuleTest : public ::testing::Test
{
protected:
    
};


}
}


int main(int argc, char** argv) {
    ::testing::InitGoogleTest(&argc, argv);
    ::testing::AddGlobalTestEnvironment(new orbital::testing::OrbitalEnvironment);
    return RUN_ALL_TESTS();
}