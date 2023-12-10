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
    void plot(long dataSet, const std::vector<double>& data) override;
    void plotVec(long dataSet, const std::vector<std::vector<double>>& data) override;
    void clear(long dataSet) override;
protected:
    BasicTest() {}
    void run(const char* file)
    {
        std::unique_ptr<ScriptModule> mod;
        auto status = this->core.load(this->scriptsDir / file, mod);
        ASSERT_TRUE(status == OrbitalError::NONE);
    }
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

// orbital.plot(1, 1, 2, 3, 4.4, 5.5)

TEST_F(BasicTest, TestPlot)
{
    this->run("test-basic-plot.py");
}

void
BasicTest::plot(long dataSet, const std::vector<double>& data)
{
    ASSERT_EQ(dataSet, 1);
    std::vector<double> expected{1, 2, 3, 4.4, 5.5};
    ASSERT_EQ(data, expected);
}

// orbital.plotVec(2, [0, 1, 2, 3], [1, 2, 3.3, 4.4])

TEST_F(BasicTest, TestPlotVec)
{
    this->run("test-basic-plotVec.py");
}

void
BasicTest::plotVec(long dataSet, const std::vector<std::vector<double>>& data)
{
    ASSERT_EQ(dataSet, 2);
    std::vector<std::vector<double>> expected{{0, 1, 2, 3}, {1, 2, 3.3, 4.4}};
    ASSERT_EQ(data, expected);
}

// orbital.plot(3)

TEST_F(BasicTest, TestClear)
{
    this->run("test-basic-clear.py");
}

void
BasicTest::clear(long dataSet)
{
    ASSERT_EQ(dataSet, 3);
}


}
}
