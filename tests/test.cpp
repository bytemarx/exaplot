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


OrbitalEnvironment::~OrbitalEnvironment()
{
}


void
OrbitalEnvironment::SetUp()
{
    std::filesystem::path executable = std::filesystem::canonical("/proc/self/exe");
    std::filesystem::path prefix = executable.parent_path() / "python";
    std::cout << "Prefix path: " << prefix << '\n';

    PyStatus status = OrbitalCore::init(executable, prefix);
    ASSERT_FALSE(PyStatus_Exception(status));
}


void
OrbitalEnvironment::TearDown()
{
    // ASSERT_EQ(OrbitalCore::deinit(), 0);
}


}
}


int main(int argc, char** argv) {
    ::testing::InitGoogleTest(&argc, argv);
    ::testing::AddGlobalTestEnvironment(new orbital::testing::OrbitalEnvironment);
    return RUN_ALL_TESTS();
}