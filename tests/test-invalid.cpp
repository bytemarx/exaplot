#include "test.hpp"

#include <filesystem>
#include <fstream>
#include <iostream>
#include <mutex>
#include <string>
#include <utility>
#include <vector>


namespace zetatest {


class InvalidTest : public ModuleTest
{
public:
    void init(const std::vector<zeta::RunParam>&, const std::vector<zeta::GridPoint>&) override;
    void msg(const std::string&, bool) override;
    void plot2D(std::size_t, double, double) override;
    void plot2DVec(std::size_t, const std::vector<double>&, const std::vector<double>&) override;
    void clear(std::size_t) override;
protected:
    InvalidTest() { this->scriptsDir = this->scriptsDir / "invalid"; }
};


TEST_F(InvalidTest, TestInit)
{
    this->run("test-invalid-init.py");
}

void
InvalidTest::init(
    [[maybe_unused]] const std::vector<zeta::RunParam>& params,
    [[maybe_unused]] const std::vector<zeta::GridPoint>& plots)
{
    ASSERT_FALSE(true);
}


TEST_F(InvalidTest, TestMsg)
{
    this->run("test-invalid-msg.py");
}

void
InvalidTest::msg(
    [[maybe_unused]] const std::string& message,
    [[maybe_unused]] bool append)
{
    ASSERT_FALSE(true);
}


TEST_F(InvalidTest, TestPlot)
{
    this->run("test-invalid-plot.py");
}

void
InvalidTest::plot2D(
    [[maybe_unused]] std::size_t plotID,
    [[maybe_unused]] double x,
    [[maybe_unused]] double y)
{
    ASSERT_FALSE(true);
}

void
InvalidTest::plot2DVec(
    [[maybe_unused]] std::size_t plotID,
    [[maybe_unused]] const std::vector<double>& x,
    [[maybe_unused]] const std::vector<double>& y)
{
    ASSERT_FALSE(true);
}

void
InvalidTest::clear([[maybe_unused]] std::size_t plotID)
{
    ASSERT_FALSE(true);
}


}
