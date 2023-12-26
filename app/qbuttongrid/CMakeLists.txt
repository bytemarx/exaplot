cmake_minimum_required(VERSION 3.22)

project(
    qbuttongrid
    VERSION 0.0.0
    LANGUAGES CXX
)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(QTVER "6")

set(CMAKE_AUTOMOC ON)

find_package(Qt${QTVER} REQUIRED COMPONENTS Core Gui UiPlugin Widgets)

qt_add_plugin(qbuttongridplugin)

target_sources(qbuttongridplugin PRIVATE
    qbuttongrid.hpp
    qbuttongrid.cpp
    qbuttongridplugin.hpp
    qbuttongridplugin.cpp
    qbuttongridprivate.hpp
)

target_include_directories(
    qbuttongridplugin PUBLIC
    "${CMAKE_CURRENT_SOURCE_DIR}/../../include"
    "${CMAKE_CURRENT_SOURCE_DIR}/../../cpython"
    "${CMAKE_CURRENT_SOURCE_DIR}/../../cpython/Include"
)

set_target_properties(qbuttongridplugin PROPERTIES
    WIN32_EXECUTABLE TRUE
    MACOSX_BUNDLE TRUE
)

target_link_libraries(qbuttongridplugin PUBLIC
    Qt${QTVER}::Core
    Qt${QTVER}::Gui
    Qt${QTVER}::UiPlugin
    Qt${QTVER}::Widgets
)

add_subdirectory("tests")
