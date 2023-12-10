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
};

// orbital.init("a", "b", "c", "d")

TEST_F(BasicTest, TestInit)
{
    std::unique_ptr<ScriptModule> mod;
    auto status = this->core.load(this->scriptsDir / "test-basic-init.py", mod);
    ASSERT_TRUE(status == OrbitalError::NONE);
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
    std::unique_ptr<ScriptModule> mod;
    auto status = this->core.load(this->scriptsDir / "test-basic-msg.py", mod);
    ASSERT_TRUE(status == OrbitalError::NONE);
}

void
BasicTest::msg(const std::string& message, bool append)
{
    ASSERT_STREQ(message.c_str(), "test");
    ASSERT_FALSE(append);
}

// orbital.plot

void
BasicTest::plot(long dataSet, const std::vector<double>& data)
{
}

// orbital.plotVec

void
BasicTest::plotVec(long dataSet, const std::vector<std::vector<double>>& data)
{
}

// orbital.clear

void
BasicTest::clear(long dataSet)
{
}


}
}
