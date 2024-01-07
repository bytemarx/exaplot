cmake_minimum_required(VERSION 3.22)

project(
    qplot
    VERSION 0.0.0
    LANGUAGES CXX
)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

set(CMAKE_AUTOMOC ON)

find_package(Qt6 REQUIRED COMPONENTS Core Gui UiPlugin Widgets PrintSupport)

qt_add_plugin(qplotplugin)

target_sources(qplotplugin PRIVATE
    plot.hpp
    plot.cpp
    plot2d.hpp
    plot2d.cpp
    plotcolormap.hpp
    plotcolormap.cpp
    qplot.hpp
    qplot.cpp
    qplotplugin.hpp
    qplotplugin.cpp
    ../qcustomplot/qcustomplot.h
    ../qcustomplot/qcustomplot.cpp
)

target_include_directories(qplotplugin PRIVATE
    "${CMAKE_CURRENT_SOURCE_DIR}/../qcustomplot"
)

set_target_properties(qplotplugin PROPERTIES
    WIN32_EXECUTABLE TRUE
    MACOSX_BUNDLE TRUE
)

target_link_libraries(qplotplugin PUBLIC
    Qt6::Core
    Qt6::Gui
    Qt6::UiPlugin
    Qt6::Widgets
    Qt6::PrintSupport
)

add_subdirectory("tests")
