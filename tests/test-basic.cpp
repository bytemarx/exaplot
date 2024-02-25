#include "test.hpp"

#include <filesystem>
#include <fstream>
#include <iostream>
#include <mutex>
#include <string>
#include <utility>
#include <vector>


namespace orbital {
namespace testing {


class BasicTest : public ModuleTest
{
public:
    void init(const std::vector<std::string>& params, const std::vector<orbital::GridPoint>& plots) override;
    void msg(const std::string& message, bool append) override;
    void plot2D(std::size_t plotID, double x, double y) override;
    void plot2DVec(std::size_t plotID, const std::vector<double>& x, const std::vector<double>& y) override;
    void clear(std::size_t plotID) override;
protected:
    BasicTest() { this->scriptsDir = this->scriptsDir / "basic"; }
};

// orbital.init("a", "b", "c", "d")

TEST_F(BasicTest, TestInit)
{
    this->run("test-basic-init.py");
}

void
BasicTest::init(
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

// orbital.plot(1, 1, 2.2)

TEST_F(BasicTest, TestPlot)
{
    this->run("test-basic-plot.py");
}

void
BasicTest::plot2D(std::size_t plotID, double x, double y)
{
    ASSERT_EQ(plotID, 1);
    ASSERT_EQ(x, 1);
    ASSERT_EQ(y, 2.2);
}

// orbital.plotVec(2, [0, 1, 2, 3], [1, 2, 3.3, 4.4])

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

// orbital.plot(3)

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
}
