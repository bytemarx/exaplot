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


class ComprehensiveTest : public ModuleTest
{
public:
    void init(const std::vector<orbital::RunParam>& params, const std::vector<orbital::GridPoint>& plots) override;
    void msg(const std::string& message, bool append) override;
    void plot2D(std::size_t plotID, double x, double y) override {};
    void plot2DVec(std::size_t plotID, const std::vector<double>& x, const std::vector<double>& y) override {};
    void clear(std::size_t plotID) override {};
protected:
    ComprehensiveTest() { this->scriptsDir = this->scriptsDir / "comprehensive"; }
};

TEST_F(ComprehensiveTest, TestInit)
{
    this->run("test-comprehensive-init.py");
}

void
ComprehensiveTest::init(
    const std::vector<orbital::RunParam>& params,
    const std::vector<orbital::GridPoint>& plots)
{
    std::vector<orbital::RunParam> expected{
        {
            .identifier = "a",
            .type = orbital::RunParamType::STRING,
            .value = "",
            .display = "a"
        },
        {
            .identifier = "b",
            .type = orbital::RunParamType::STRING,
            .value = "127.0.0.1",
            .display = "b"
        },
        {
            .identifier = "c",
            .type = orbital::RunParamType::INT,
            .value = "-100",
            .display = "c"
        },
        {
            .identifier = "d",
            .type = orbital::RunParamType::FLOAT,
            .value = "1.602e-19",
            .display = "d"
        },
        {
            .identifier = "e",
            .type = orbital::RunParamType::STRING,
            .value = "192.168.1.255",
            .display = "Gateway"
        },
        {
            .identifier = "f",
            .type = orbital::RunParamType::STRING,
            .value = "",
            .display = "Server"
        },
        {
            .identifier = "g",
            .type = orbital::RunParamType::INT,
            .value = "4",
            .display = "Rings"
        },
        {
            .identifier = "h",
            .type = orbital::RunParamType::INT,
            .value = "",
            .display = "Level"
        },
        {
            .identifier = "i",
            .type = orbital::RunParamType::FLOAT,
            .value = "5.0",
            .display = "Threshold (V)"
        },
        {
            .identifier = "j",
            .type = orbital::RunParamType::FLOAT,
            .value = "",
            .display = "Noise (dB)"
        },
        {
            .identifier = "k",
            .type = orbital::RunParamType::STRING,
            .value = "192.168.1.255",
            .display = "k"
        },
        {
            .identifier = "l",
            .type = orbital::RunParamType::STRING,
            .value = "",
            .display = "l"
        },
        {
            .identifier = "m",
            .type = orbital::RunParamType::INT,
            .value = "4",
            .display = "m"
        },
        {
            .identifier = "n",
            .type = orbital::RunParamType::INT,
            .value = "",
            .display = "n"
        },
        {
            .identifier = "o",
            .type = orbital::RunParamType::FLOAT,
            .value = "5.0",
            .display = "o"
        },
        {
            .identifier = "p",
            .type = orbital::RunParamType::FLOAT,
            .value = "",
            .display = "p"
        }
    };
    ASSERT_EQ(params.size(), expected.size());
    std::size_t i = 0;
    for (const auto& param : params) {
        ASSERT_STREQ(param.identifier.c_str(), expected[i].identifier.c_str());
        ASSERT_EQ(param.type, expected[i].type);
        ASSERT_STREQ(param.value.c_str(), expected[i].value.c_str());
        ASSERT_STREQ(param.display.c_str(), expected[i].display.c_str());
        i++;
    }
}

TEST_F(ComprehensiveTest, TestMsg)
{
    this->run("test-comprehensive-msg.py");
}

void
ComprehensiveTest::msg(const std::string& message, bool append)
{
    ASSERT_STREQ(message.c_str(), "test");
    ASSERT_TRUE(append);
}


}
}
