cmake_minimum_required(VERSION 3.22)

project(
    qbuttongrid-test
)

include(FetchContent)

FetchContent_Declare(
    googletest
    GIT_REPOSITORY https://github.com/google/googletest.git
    GIT_TAG c5be3930f59f3ae99725ed99031aae4d8b8bbb3b # release-1.11.0
)
FetchContent_MakeAvailable(googletest)

# file(GLOB TEST_SOURCES ${CMAKE_CURRENT_SOURCE_DIR}/../*.cpp)

set(SOURCES
    test.cpp
    ../qbuttongrid.cpp
    ../qbuttongridprivate.hpp
)

set(TEST_LIBRARIES
    gtest
)

set(QT_LIBRARIES
    Qt::Core
    Qt::Gui
    Qt::UiPlugin
    Qt::Widgets
)

enable_testing()

add_executable(qbuttongridtest ${TEST_SOURCES} ${SOURCES})

target_include_directories(
	qbuttongridtest PUBLIC
	"${CMAKE_CURRENT_SOURCE_DIR}/.."
    "${CMAKE_BINARY_DIR}"
    "${CMAKE_CURRENT_SOURCE_DIR}/../../../include"
    "${CMAKE_CURRENT_SOURCE_DIR}/../../../cpython"
    "${CMAKE_CURRENT_SOURCE_DIR}/../../../cpython/Include"
)

target_compile_options(qbuttongridtest PUBLIC -g)
target_link_libraries(qbuttongridtest PRIVATE ${TEST_LIBRARIES} ${QT_LIBRARIES})

include(GoogleTest)
gtest_discover_tests(qbuttongridtest)
