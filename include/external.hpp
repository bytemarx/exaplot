/*
 * ZetaPlot
 * api macros
 * 
 * SPDX-License-Identifier: GPL-3.0
 * Copyright (C) 2024 bytemarx
 */

#pragma once


#if defined(_WIN32)
    #if defined(BUILD_ZETAPLOT)
        #define ZETA_API __declspec(dllexport)
    #else
        #define ZETA_API __declspec(dllimport)
    #endif
#else
    #define ZETA_API __attribute__((visibility("default")))
#endif
