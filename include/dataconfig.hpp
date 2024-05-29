/*
 * ExaPlot
 * datafile configuration
 * 
 * SPDX-License-Identifier: GPL-3.0
 * Copyright (C) 2024 bytemarx
 */

#pragma once


#include <filesystem>
#include <optional>


namespace exa {


struct DatafileConfig
{
    std::optional<bool> enable;
};


}
