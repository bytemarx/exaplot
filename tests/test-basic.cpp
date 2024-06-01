#include "test.hpp"

#include <filesystem>
#include <fstream>
#include <iostream>
#include <mutex>
#include <string>
#include <utility>
#include <vector>


namespace exatest {


class BasicTest : public ModuleTest
{
public:
    void init(const std::vector<exa::RunParam>& params, const std::vector<exa::GridPoint>& plots) override;
    void msg(const std::string& message, bool append) override;
    void datafile(const exa::DatafileConfig& config, PyObject* path, bool prompt) override;
    void plot2D(std::size_t plotID, double x, double y, bool write) override;
    void plot2DVec(std::size_t plotID, const std::vector<double>& x, const std::vector<double>& y) override;
    void clear(std::size_t plotID) override;
protected:
    BasicTest() { this->scriptsDir = this->scriptsDir / "basic"; }
};


// init(a='', b='', c=1, d=10.0)

TEST_F(BasicTest, TestInit)
{
    this->run("test-basic-init.py");
}

void
BasicTest::init(
    const std::vector<exa::RunParam>& params,
    const std::vector<exa::GridPoint>& plots)
{
    ASSERT_EQ(params.size(), 4);
    std::vector<exa::RunParam> expected{
        {
            .identifier = "a",
            .type = exa::RunParamType::STRING,
            .value = "",
            .display = "a"
        },
        {
            .identifier = "b",
            .type = exa::RunParamType::STRING,
            .value = "",
            .display = "b"
        },
        {
            .identifier = "c",
            .type = exa::RunParamType::INT,
            .value = "1",
            .display = "c"
        },
        {
            .identifier = "d",
            .type = exa::RunParamType::FLOAT,
            .value = "10.0",
            .display = "d"
        },
    };
    std::size_t i = 0;
    for (const auto& param : params) {
        ASSERT_STREQ(param.identifier.c_str(), expected[i].identifier.c_str());
        ASSERT_EQ(param.type, expected[i].type);
        ASSERT_STREQ(param.value.c_str(), expected[i].value.c_str());
        ASSERT_STREQ(param.display.c_str(), expected[i].display.c_str());
        i++;
    }
}


// msg("test", append=False)

TEST_F(BasicTest, TestMsg)
{
    this->run("test-basic-msg.py");
}

void
BasicTest::msg(const std::string& message, bool append)
{
    ASSERT_STREQ(message.c_str(), "test");
    ASSERT_FALSE(append);
}


// datafile()

TEST_F(BasicTest, TestDatafile)
{
    this->run("test-basic-datafile.py");
}

void
BasicTest::datafile(const exa::DatafileConfig& config, PyObject* path, bool prompt)
{
    ASSERT_TRUE(config.enable);
    ASSERT_FALSE(prompt);
    ASSERT_EQ(path, nullptr);
}


// plot(1, 1, 2.2)

TEST_F(BasicTest, TestPlot)
{
    this->run("test-basic-plot.py");
}

void
BasicTest::plot2D(std::size_t plotID, double x, double y, bool write)
{
    ASSERT_EQ(plotID, 1);
    ASSERT_EQ(x, 1);
    ASSERT_EQ(y, 2.2);
    ASSERT_TRUE(write);
}


// plotVec(2, [0, 1, 2, 3], [1, 2, 3.3, 4.4])

TEST_F(BasicTest, TestPlotVec)
{
    this->run("test-basic-plotVec.py");
}

void
BasicTest::plot2DVec(std::size_t plotID, const std::vector<double>& x, const std::vector<double>& y)
{
    ASSERT_EQ(plotID, 2);
    std::vector<double> expected_x{0, 1, 2, 3};
    ASSERT_EQ(x, expected_x);
    std::vector<double> expected_y{1, 2, 3.3, 4.4};
    ASSERT_EQ(y, expected_y);
}


// plot(3)

TEST_F(BasicTest, TestClear)
{
    this->run("test-basic-clear.py");
}

void
BasicTest::clear(std::size_t plotID)
{
    ASSERT_EQ(plotID, 3);
}


}
