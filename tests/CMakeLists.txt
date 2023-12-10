cmake_minimum_required(VERSION 3.22)

project(
    Orbital-Tests
)

include(FetchContent)

FetchContent_Declare(
    googletest
    GIT_REPOSITORY https://github.com/google/googletest.git
    GIT_TAG c5be3930f59f3ae99725ed99031aae4d8b8bbb3b # release-1.11.0
)
FetchContent_MakeAvailable(googletest)

file(GLOB TEST_SOURCES *.cpp)

set(TEST_LIBRARIES
    gtest
)

configure_file(test-config.h.in test-config.h)

# set(CMAKE_EXE_LINKER_FLAGS "${CMAKE_EXE_LINKER_FLAGS} -Wl,--verbose")

enable_testing()

add_executable(tests ${TEST_SOURCES})

target_include_directories(
	tests PUBLIC
	"${CMAKE_CURRENT_SOURCE_DIR}/../include"
    "${PYTHON_INCLUDES}"
    "${CMAKE_BINARY_DIR}/tests"
)

target_link_libraries(tests PRIVATE orbital ${TEST_LIBRARIES})

include(GoogleTest)
gtest_discover_tests(tests)