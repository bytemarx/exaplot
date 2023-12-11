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
    virtual void plot(long dataSet, const std::vector<double>& data) = 0;
    virtual void plotVec(long dataSet, const std::vector<std::vector<double>>& data) = 0;
    virtual void clear(long dataSet) = 0;

private:
    class Interface : public OrbitalInterface
    {
    public:
        Interface(ModuleTest* tester);
        PyObject* init(const std::vector<std::string>& params, const std::vector<orbital::GridPoint>& plots) const override;
        PyObject* msg(const std::string& message, bool append) const override;
        PyObject* plot(long dataSet, const std::vector<double>& data) const override;
        PyObject* plotVec(long dataSet, const std::vector<std::vector<double>>& data) const override;
        PyObject* clear(long dataSet) const override;

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
