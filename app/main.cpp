/*
 * ExaPlot
 * app entry point
 * 
 * SPDX-License-Identifier: GPL-3.0
 * Copyright (C) 2024 bytemarx
 */

#include "appmain.hpp"

#include "exaplot.hpp"
#include "toml.hpp"

#include <filesystem>
#include <iostream>
#include <string>


struct Config
{
    Config() {
        const char* configPath = std::getenv("EXACONFIG");
        if (configPath == NULL)
            return;

        try {
            auto config = toml::parse_file(configPath);

            const auto& config_searchPaths = config.at_path("python.search_paths");
            if (config_searchPaths && config_searchPaths.is_array()) {
                for (auto&& entry : *config_searchPaths.as_array())
                    if (auto path = entry.value<std::string>()) this->m_searchPaths.push_back(*path);
            }
        }
        catch (const toml::parse_error& e) {
            std::cerr << "Failed to read config (" << configPath << "):\n" << e << '\n';
        }
    }

    const std::vector<std::filesystem::path>& searchPaths() const { return this->m_searchPaths; }

private:
    std::vector<std::filesystem::path> m_searchPaths;
};


int main(int argc, char* argv[])
{
    Config config{};
    AppMain a{argc, argv, config.searchPaths()};
    return a.exec();
}
