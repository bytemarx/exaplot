file(GLOB TEST_SOURCES *.cpp)

configure_file(test-config.h.in test-config.h)

if(HAVING_A_BAD_TIME)
    set(CMAKE_EXE_LINKER_FLAGS "${CMAKE_EXE_LINKER_FLAGS} -Wl,--verbose")
endif()

enable_testing()

add_executable(tests ${TEST_SOURCES})

target_include_directories(
	tests PUBLIC
	../include
    "${PYTHON_INCLUDES}"
    "${CMAKE_BINARY_DIR}/tests"
)

target_link_libraries(tests PRIVATE exaplot gtest)

include(GoogleTest)
gtest_discover_tests(tests)
