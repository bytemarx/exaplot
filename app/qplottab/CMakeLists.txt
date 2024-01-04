cmake_minimum_required(VERSION 3.22)

project(
    qplottab
    VERSION 0.0.0
    LANGUAGES CXX
)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(QTVER "6")

set(CMAKE_AUTOMOC ON)

find_package(Qt${QTVER} REQUIRED COMPONENTS Core Gui UiPlugin Widgets)

qt_add_plugin(qplottabplugin)

target_sources(qplottabplugin PRIVATE
    qplottab.cpp
    qplottabprivate.cpp
    qplottabplugin.cpp
)

target_include_directories(qplottabplugin PRIVATE
    "${CMAKE_CURRENT_SOURCE_DIR}"
    "${CMAKE_CURRENT_SOURCE_DIR}/../qcustomplot"
)

set_target_properties(qplottabplugin PROPERTIES
    WIN32_EXECUTABLE TRUE
    MACOSX_BUNDLE TRUE
)

target_link_libraries(qplottabplugin PUBLIC
    Qt::Core
    Qt::Gui
    Qt::UiPlugin
    Qt::Widgets
)

add_subdirectory("tests")
