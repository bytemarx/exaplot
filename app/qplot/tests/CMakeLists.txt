cmake_minimum_required(VERSION 3.22)

project(
    qplot-test
)

include(FetchContent)

FetchContent_Declare(
    googletest
    GIT_REPOSITORY https://github.com/google/googletest.git
    GIT_TAG c5be3930f59f3ae99725ed99031aae4d8b8bbb3b # release-1.11.0
)
FetchContent_MakeAvailable(googletest)

file(GLOB TEST_SOURCES ../*.cpp)

set(SOURCES
    test.cpp
    ../../qcustomplot/qcustomplot.cpp
)

set(TEST_LIBRARIES
    gtest
)

set(QT_LIBRARIES
    Qt6::Core
    Qt6::Gui
    Qt6::UiPlugin
    Qt6::Widgets
    Qt6::PrintSupport
)

enable_testing()

add_executable(qplottest ${TEST_SOURCES} ${SOURCES})

target_include_directories(
	qplottest PUBLIC
	"${CMAKE_CURRENT_SOURCE_DIR}/.."
	"${CMAKE_CURRENT_SOURCE_DIR}/../../qcustomplot"
    "${CMAKE_BINARY_DIR}"
)

target_compile_options(qplottest PUBLIC -g)
target_link_libraries(qplottest PRIVATE ${TEST_LIBRARIES} ${QT_LIBRARIES})

include(GoogleTest)
gtest_discover_tests(qplottest)
