set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)
set(CMAKE_AUTOUIC ON)

enable_testing()

find_package(Qt6 REQUIRED COMPONENTS PrintSupport Svg)

file(GLOB APP_SOURCES_UI ../ui/*.cpp)

add_executable(apptests
    test.cpp
	$<TARGET_OBJECTS:qbuttongridtests>
    $<TARGET_OBJECTS:qbuttongrid>
	$<TARGET_OBJECTS:qplottabtests>
    $<TARGET_OBJECTS:qplottab>
	$<TARGET_OBJECTS:qplottests>
    $<TARGET_OBJECTS:qplot>
	../qcustomplot/qcustomplot.cpp
)

target_include_directories(
	apptests PUBLIC
	../qbuttongrid
	../qplottab
	../qcustomplot
	../qplot
    ../ui
	../../include
	../../build
    "${PYTHON_INCLUDES}"
)

target_compile_options(apptests PUBLIC -g)
target_link_libraries(apptests PRIVATE
    gtest
    exaplot
	Qt6::PrintSupport
	Qt6::Svg
)

include(GoogleTest)
gtest_discover_tests(apptests)
