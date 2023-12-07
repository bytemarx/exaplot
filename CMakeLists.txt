cmake_minimum_required(VERSION 3.22)

project(
    Orbital
    VERSION 0.0.0
    LANGUAGES CXX
)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

set(CMAKE_RUNTIME_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/bin")
set(OUTPUT_LIB_DIRECTORY "${CMAKE_RUNTIME_OUTPUT_DIRECTORY}/lib")
set(LIB_INSTALL_DIR lib/orbital)

include(ExternalProject)

set(QTVER "6")
set(PYTHON_VERSION "3.12.0")

################################
######### Python setup #########
################################

# build/
#   - top-level build directory (CMAKE_BINARY_DIR)
#   bin/
#     - build output directory (CMAKE_RUNTIME_OUTPUT_DIRECTORY)
#     lib/
#       - CPython shared libraries (OUTPUT_LIB_DIRECTORY)
#     python/
#       - interpreter's library (PYTHON_INSTALL_DIR)
#       lib/
#         - stdlib modules
#       lib-dynload/
#         - stdlib extended modules
#       site-packages/
#         - third-party modules
#   python-${PYTHON_VERSION}/
#     - CPython external project root directory
#     src/python/
#       - CPython interpreter source directory; build location of CPython shared libraries (CPYTHON_SRC_DIR)
#   python-env/
#     - embedded Python environment workspace (PYTHON_ENV_DIR)
# python/
#   - custom built-in Python module project source (PYTHON_PROJECT_DIR)

set(PYTHON_TARGET "python")
string(REGEX MATCH "^[0-9]+\.[0-9]+" PYTHON_VER_MAJ_MIN ${PYTHON_VERSION})
if("${PYTHON_VER_MAJ_MIN}" STREQUAL "")
    message(FATAL_ERROR "Failed to parse Python version")
endif()
set(PYTHON_DOWNLOAD_URL "https://www.python.org/ftp/python/${PYTHON_VERSION}/Python-${PYTHON_VERSION}.tgz")

set(PYTHON_INSTALL_DIR "${CMAKE_RUNTIME_OUTPUT_DIRECTORY}/${PYTHON_TARGET}")
set(PYTHON_LIB_DIR "lib")
set(PYTHON_NATIVE_LIB_DIR "lib-dynload")
set(PYTHON_SITE_PACKAGES_DIR "site-packages")
set(CPYTHON_PROJ "${PYTHON_TARGET}-${PYTHON_VERSION}")
set(CPYTHON_SRC_DIR "${CMAKE_BINARY_DIR}/${CPYTHON_PROJ}/src/${PYTHON_TARGET}")
set(PYTHON_ENV_DIR "${CMAKE_BINARY_DIR}/${PYTHON_TARGET}-env")

set(PYTHON_PROJECT_DIR "${CMAKE_SOURCE_DIR}/python")

if(CMAKE_BUILD_TYPE STREQUAL "Debug")
    set(CPYTHON_CONFIGURE_FLAGS
        --enable-shared
        --with-pydebug
    )
    set(CPYTHON_LIBRARY_PREFIX "libpython${PYTHON_VER_MAJ_MIN}d")
    set(CPYTHON_LIBS
        "${CPYTHON_SRC_DIR}/${CPYTHON_LIBRARY_PREFIX}.so"
        "${CPYTHON_SRC_DIR}/${CPYTHON_LIBRARY_PREFIX}.so.1.0"
    )
else()
    set(CPYTHON_CONFIGURE_FLAGS
        --enable-shared
        --disable-test-modules
    )
    set(CPYTHON_LIBRARY_PREFIX "libpython${PYTHON_VER_MAJ_MIN}")
    set(CPYTHON_LIBS
        "${CPYTHON_SRC_DIR}/libpython3.so"
        "${CPYTHON_SRC_DIR}/${CPYTHON_LIBRARY_PREFIX}.so"
        "${CPYTHON_SRC_DIR}/${CPYTHON_LIBRARY_PREFIX}.so.1.0"
    )
endif()

message("Targeted Python version: ${PYTHON_VERSION}")
message("Python download URL: ${PYTHON_DOWNLOAD_URL}")

message("CMake output directory: ${CMAKE_RUNTIME_OUTPUT_DIRECTORY}")
message("Library build output directory: ${OUTPUT_LIB_DIRECTORY}")
message("Interpreter library directory: ${PYTHON_INSTALL_DIR}")
message("CPython interpreter source directory: ${CPYTHON_SRC_DIR}")
message("Python environment workspace: ${PYTHON_ENV_DIR}")

message("Python project directory: ${PYTHON_PROJECT_DIR}")

# Download CPython & set up isolated environment workspace
ExternalProject_Add(
    ${PYTHON_TARGET}
    PREFIX ${CPYTHON_PROJ}
    URL ${PYTHON_DOWNLOAD_URL}
    DOWNLOAD_NO_PROGRESS TRUE
    CONFIGURE_COMMAND
        ./configure ${CPYTHON_CONFIGURE_FLAGS}
    BUILD_COMMAND
        make -s -j $(nproc)
    BUILD_IN_SOURCE TRUE
    BUILD_ALWAYS TRUE
    INSTALL_COMMAND
        ${CMAKE_COMMAND} -E env LD_LIBRARY_PATH=${CPYTHON_SRC_DIR} ./python -m venv ${PYTHON_ENV_DIR}
)

# Copy CPython native libraries to known build directory
add_custom_command(
    TARGET ${PYTHON_TARGET}
    POST_BUILD
    COMMAND
        bash -c "cp -rL $(cat pybuilddir.txt) ${PYTHON_NATIVE_LIB_DIR}"
    COMMENT "Copying the interpreter's native libraries to ${CPYTHON_SRC_DIR}/${PYTHON_NATIVE_LIB_DIR}"
    WORKING_DIRECTORY ${CPYTHON_SRC_DIR}
    VERBATIM
    BYPRODUCTS ${PYTHON_NATIVE_LIB_DIR}
)

# Build Python project & install into isolated environment workspace
add_custom_target(
    python-project ALL
        ${CMAKE_COMMAND} -E env
            --unset=PYTHONHOME
            VIRTUAL_ENV=${PYTHON_ENV_DIR}
            PATH=${PYTHON_ENV_DIR}/bin:$ENV{PATH}
            LD_LIBRARY_PATH=${CPYTHON_SRC_DIR}
        pip install --force-reinstall --disable-pip-version-check .
    BYPRODUCTS
        ${PYTHON_PROJECT_DIR}/build
        ${PYTHON_PROJECT_DIR}/src/orbital.egg-info
    WORKING_DIRECTORY ${PYTHON_PROJECT_DIR}
)
add_dependencies(python-project ${PYTHON_TARGET})

# Copy the main CPython libraries from build location to the output directory
add_custom_command(
    TARGET ${PYTHON_TARGET}
    POST_BUILD
    COMMAND ${CMAKE_COMMAND} -E rm -rf -- "${OUTPUT_LIB_DIRECTORY}"
    COMMAND ${CMAKE_COMMAND} -E make_directory
        "${OUTPUT_LIB_DIRECTORY}"
    COMMAND ${CMAKE_COMMAND} -E copy
        ${CPYTHON_LIBS}
        "${OUTPUT_LIB_DIRECTORY}"
    COMMENT "Copying the interpreter's shared libraries to ${OUTPUT_LIB_DIRECTORY}"
    BYPRODUCTS ${OUTPUT_LIB_DIRECTORY}
)

# Copy interpreter's library modules to the output directory
add_custom_command(
    TARGET python-project
    POST_BUILD
    COMMAND ${CMAKE_COMMAND} -E rm -rf -- "${PYTHON_INSTALL_DIR}"
    COMMAND ${CMAKE_COMMAND} -E copy_directory
        "${CPYTHON_SRC_DIR}/Lib"
        "${PYTHON_INSTALL_DIR}/${PYTHON_LIB_DIR}"
    COMMAND ${CMAKE_COMMAND} -E copy_directory
        "${CPYTHON_SRC_DIR}/${PYTHON_NATIVE_LIB_DIR}"
        "${PYTHON_INSTALL_DIR}/${PYTHON_NATIVE_LIB_DIR}"
    COMMAND ${CMAKE_COMMAND} -E copy_directory
        "${PYTHON_ENV_DIR}/lib/python${PYTHON_VER_MAJ_MIN}/site-packages"
        "${PYTHON_INSTALL_DIR}/${PYTHON_SITE_PACKAGES_DIR}"
    COMMENT "Copying the interpreter's library modules to ${PYTHON_INSTALL_DIR}"
    BYPRODUCTS ${PYTHON_INSTALL_DIR}
)

add_library(pythonlib SHARED IMPORTED)
set_target_properties(pythonlib PROPERTIES IMPORTED_LOCATION "${OUTPUT_LIB_DIRECTORY}/${CPYTHON_LIBRARY_PREFIX}.so")
add_library(pythonlib10 SHARED IMPORTED)
set_target_properties(pythonlib10 PROPERTIES IMPORTED_LOCATION "${OUTPUT_LIB_DIRECTORY}/${CPYTHON_LIBRARY_PREFIX}.so.1.0")

set(PYTHON_INCLUDES
    "${CPYTHON_SRC_DIR}"
    "${CPYTHON_SRC_DIR}/Include"
    "${CPYTHON_SRC_DIR}/Include/internal"
)

################################
####### Python setup end #######
################################

add_compile_definitions(QT_NO_SIGNALS_SLOTS_KEYWORDS)

set(ORBITAL_LIBRARY_PATH "${CMAKE_INSTALL_PREFIX}/${LIB_INSTALL_DIR}")
configure_file(config.h.in config.h)

set(SOURCES
    source/core.cpp
    source/module.cpp
)

set(CFLAGS
    -Wsign-compare
    -fwrapv
    -O3
    -Wall
)

if(CMAKE_BUILD_TYPE STREQUAL "Debug")
    list(APPEND CFLAGS
        -g
    )
endif()

set(LIBRARIES
    pythonlib
    pythonlib10
    dl
    m
)

add_library(orbital SHARED ${SOURCES})

add_dependencies(orbital ${PYTHON_TARGET})

target_include_directories(orbital PUBLIC
    $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>
    $<INSTALL_INTERFACE:include/orbital>
    "${CMAKE_BINARY_DIR}"
    "${PYTHON_INCLUDES}"
)

target_compile_options(orbital PUBLIC ${CFLAGS})
target_link_libraries(orbital PUBLIC ${LIBRARIES})
set_target_properties(orbital PROPERTIES INSTALL_RPATH "$ENV{DESTDIR}${CMAKE_INSTALL_PREFIX}/${LIB_INSTALL_DIR}")

add_subdirectory("tests")

# add_subdirectory("app")
