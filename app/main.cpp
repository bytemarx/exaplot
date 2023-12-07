#include "AppMain.hpp"

#include "orbital.hpp"
#include "config.h"

#include <filesystem>
#include <string>


int main(int argc, char* argv[])
{
    std::wstring executable{L"chainsaw"};
    std::filesystem::path prefix{ORBITAL_LIBRARY_PATH "/python"};

    // TODO: For uninstalled applications, find a better way to locate library
    if (!std::filesystem::exists(prefix))
        prefix = std::filesystem::canonical("/proc/self/exe").parent_path() / "python";

    PyStatus status = orbital::OrbitalCore::init(executable, prefix);
    if (PyStatus_Exception(status)) {
        if (PyStatus_IsExit(status)) return status.exitcode;
        Py_ExitStatusException(status);
    }
    AppMain a{argc, argv};
    return a.exec() && orbital::OrbitalCore::deinit();
}