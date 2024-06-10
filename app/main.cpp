/*
 * ExaPlot
 * app entry point
 * 
 * SPDX-License-Identifier: GPL-3.0
 * Copyright (C) 2024 bytemarx
 */

#include "appmain.hpp"


int main(int argc, char* argv[])
{
    Config config{};
    AppMain a{argc, argv, config};
    return a.exec();
}
