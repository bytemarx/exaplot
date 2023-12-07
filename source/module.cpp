#include "orbital.hpp"


namespace orbital {


ScriptModule::ScriptModule(const std::filesystem::path& file)
{
}


OrbitalError ScriptModule::load()
{
    return OrbitalError{OrbitalError::NONE};
}


}