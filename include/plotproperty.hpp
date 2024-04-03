/*
 * ZetaPlot
 * plot property object
 * 
 * SPDX-License-Identifier: GPL-3.0
 * Copyright (C) 2024 bytemarx
 */

#pragma once

#include <map>
#include <string>
#include <variant>

#include "external.hpp"


namespace zeta {


class ZETA_API PlotProperty
{
public:
    typedef std::variant<int, double, std::string, bool> Value;
    enum Type {
        TITLE,                  // str
        XAXIS,                  // str
        YAXIS,                  // str
        MINSIZE,                // tuple[int, int]
        MINSIZE_W,              // int
        MINSIZE_H,              // int
        TWODIMEN_XRANGE_MIN,    // float
        TWODIMEN_XRANGE_MAX,    // float
        TWODIMEN_YRANGE_MIN,    // float
        TWODIMEN_YRANGE_MAX,    // float
        TWODIMEN_LINE_TYPE,     // str
        TWODIMEN_LINE_COLOR,    // str
        TWODIMEN_LINE_STYLE,    // str
        TWODIMEN_POINTS_SHAPE,  // str
        TWODIMEN_POINTS_COLOR,  // str
        TWODIMEN_POINTS_SIZE,   // float
        TWODIMEN_AUTORS_AXES,   // bool
        COLORMAP_XRANGE_MIN,    // float
        COLORMAP_XRANGE_MAX,    // float
        COLORMAP_YRANGE_MIN,    // float
        COLORMAP_YRANGE_MAX,    // float
        COLORMAP_ZRANGE_MIN,    // float
        COLORMAP_ZRANGE_MAX,    // float
        COLORMAP_DATASIZE_X,    // int
        COLORMAP_DATASIZE_Y,    // int
        COLORMAP_COLOR_MIN,     // str
        COLORMAP_COLOR_MAX,     // str
        COLORMAP_AUTORS_AXES,   // bool
        COLORMAP_AUTORS_DATA,   // bool
    };

    static const char* toStr(Type);

    PlotProperty(const std::string&);

    constexpr operator Type() const noexcept { return this->m_property; }
    constexpr bool operator==(Type t) const noexcept { return t == this->m_property; }
    const char* c_str() const noexcept { return this->m_str.c_str(); }

private:
    Type m_property;
    std::string m_str;
};


}
