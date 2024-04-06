/*
 * ExaPlot
 * app entry point
 * 
 * SPDX-License-Identifier: GPL-3.0
 * Copyright (C) 2024 bytemarx
 */

#include "appmain.hpp"

#include "exaplot.hpp"

#include <filesystem>
#include <string>


int main(int argc, char* argv[])
{
    AppMain a{argc, argv, std::vector<std::filesystem::path>{}};
    return a.exec();
}
