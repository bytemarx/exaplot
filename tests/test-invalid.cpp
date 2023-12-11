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


class InvalidTest : public ModuleTest
{
public:
    void init(const std::vector<std::string>& params, const std::vector<orbital::GridPoint>& plots) override;
    void msg(const std::string& message, bool append) override;
    void plot(long dataSet, const std::vector<double>& data) override;
    void plotVec(long dataSet, const std::vector<std::vector<double>>& data) override;
    void clear(long dataSet) override;
protected:
    InvalidTest() { this->scriptsDir = this->scriptsDir / "invalid"; }
};


TEST_F(InvalidTest, TestInit)
{
    this->run("test-invalid-init.py");
}

void
InvalidTest::init(
    [[maybe_unused]] const std::vector<std::string>& params,
    [[maybe_unused]] const std::vector<orbital::GridPoint>& plots)
{
    ASSERT_FALSE(true);
}


TEST_F(InvalidTest, TestMsg)
{
    this->run("test-invalid-msg.py");
}

void
InvalidTest::msg([[maybe_unused]] const std::string& message, [[maybe_unused]] bool append)
{
    ASSERT_FALSE(true);
}


TEST_F(InvalidTest, TestPlot)
{
    this->run("test-invalid-plot.py");
}

void
InvalidTest::plot([[maybe_unused]] long dataSet, [[maybe_unused]] const std::vector<double>& data)
{
    ASSERT_FALSE(true);
}

void
InvalidTest::plotVec([[maybe_unused]] long dataSet, [[maybe_unused]] const std::vector<std::vector<double>>& data)
{
    ASSERT_FALSE(true);
}

void
InvalidTest::clear([[maybe_unused]] long dataSet)
{
    ASSERT_FALSE(true);
}


}
}
