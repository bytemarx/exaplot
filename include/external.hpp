/*
 * ExaPlot
 * api macros
 * 
 * SPDX-License-Identifier: GPL-3.0
 * Copyright (C) 2024 bytemarx
 */

#pragma once


#if defined(_WIN32)
    #if defined(BUILD_EXAPLOT)
        #define EXA_API __declspec(dllexport)
    #else
        #define EXA_API __declspec(dllimport)
    #endif
#else
    #define EXA_API __attribute__((visibility("default")))
#endif
