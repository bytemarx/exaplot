cmake_minimum_required(VERSION 3.22)

project(
    ZetaPlot
    VERSION 0.0.0
    LANGUAGES CXX
)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

set(CMAKE_RUNTIME_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/bin/$<CONFIG>")
set(LIB_OUTPUT_DIRECTORY "${CMAKE_RUNTIME_OUTPUT_DIRECTORY}/lib")

include(ExternalProject)

set(PYTHON_VERSION "3.12.0")

################################
######### Python setup #########
################################

set(CPYTHON_TARGET "python")
string(REGEX MATCH "^[0-9]+\.[0-9]+" PYTHON_VER_MAJ_MIN ${PYTHON_VERSION})
if("${PYTHON_VER_MAJ_MIN}" STREQUAL "")
    message(FATAL_ERROR "Failed to parse Python version")
endif()

set(CPYTHON_SRC_DIR "${CMAKE_SOURCE_DIR}/cpython")
set(PYTHON_PROJECT_DIR "${CMAKE_SOURCE_DIR}/python")
set(PYTHON_INSTALL_DIR "${CMAKE_RUNTIME_OUTPUT_DIRECTORY}/python")
set(PYTHON_STDLIB_DIR "${PYTHON_INSTALL_DIR}/lib")
set(PYTHON_STDLIBEXT_DIR "${PYTHON_INSTALL_DIR}/lib-dynload")

# CPython
if(WIN32)
    set(PYTHON_INCLUDES
        "${CPYTHON_SRC_DIR}/PC"
        "${CPYTHON_SRC_DIR}/Include"
    )
    if(CMAKE_BUILD_TYPE STREQUAL "Debug")
        set(CPYTHON_BUILD_COMMAND PCbuild/build.bat -c Debug)
        set(CPYTHON_LIB "python312_d.dll")
        set(CPYTHON_IMPLIB "python312_d.lib")
    else()
        set(CPYTHON_BUILD_COMMAND PCbuild/build.bat -c Release)
        set(CPYTHON_LIB "python312.dll")
        set(CPYTHON_IMPLIB "python312.lib")
    endif()
    ExternalProject_Add(
        ${CPYTHON_TARGET}
        PREFIX python-${PYTHON_VERSION}
        SOURCE_DIR ${CPYTHON_SRC_DIR}
        GIT_SUBMODULES cpython
        CONFIGURE_COMMAND ""
        BUILD_COMMAND ${CPYTHON_BUILD_COMMAND}
        BUILD_IN_SOURCE TRUE
        INSTALL_COMMAND
                ${CMAKE_COMMAND} -E rm -rf -- "${PYTHON_INSTALL_DIR}"
        COMMAND ${CMAKE_COMMAND} -E make_directory "${PYTHON_STDLIBEXT_DIR}"

        # copy all *.dll files to PYTHON_STDLIBEXT_DIR
        COMMAND PowerShell -Command "Get-ChildItem -Path '${CPYTHON_SRC_DIR}/PCbuild/amd64' -Include '*.dll', '*.pyd' -Recurse | ForEach-Object { Copy-Item -Path $_.FullName -Destination '${PYTHON_STDLIBEXT_DIR}' -Force }"

        # move interpreter library (separate interpreter from stdlib extensions)
        COMMAND ${CMAKE_COMMAND} -E copy -t "${CMAKE_RUNTIME_OUTPUT_DIRECTORY}" "${PYTHON_STDLIBEXT_DIR}/${CPYTHON_LIB}"
        COMMAND ${CMAKE_COMMAND} -E rm -- "${PYTHON_STDLIBEXT_DIR}/${CPYTHON_LIB}"

        # copy interpreter import library
        COMMAND ${CMAKE_COMMAND} -E copy -t "${CMAKE_RUNTIME_OUTPUT_DIRECTORY}" "${CPYTHON_SRC_DIR}/PCbuild/amd64/${CPYTHON_IMPLIB}"

        # copy stdlib
        COMMAND ${CMAKE_COMMAND} -E copy_directory "${CPYTHON_SRC_DIR}/Lib" "${PYTHON_STDLIB_DIR}"

        # copy project package
        COMMAND ${CMAKE_COMMAND} -E copy -t "${PYTHON_STDLIB_DIR}" "${PYTHON_PROJECT_DIR}/zetaplot.py"
    )
else()
    set(PYTHON_INCLUDES
        "${CPYTHON_SRC_DIR}"
        "${CPYTHON_SRC_DIR}/Include"
    )
    if(CMAKE_BUILD_TYPE STREQUAL "Debug")
        set(CPYTHON_CONFIGURE_COMMAND ./configure --enable-shared --with-pydebug)
        set(CPYTHON_LIB "libpython${PYTHON_VER_MAJ_MIN}d.so.1.0")
    else()
        set(CPYTHON_CONFIGURE_COMMAND ./configure --enable-shared --disable-test-modules)
        set(CPYTHON_LIB "libpython${PYTHON_VER_MAJ_MIN}.so.1.0")
    endif()
    set(CPYTHON_BUILD_COMMAND CFLAGS=-w make -j)
    ExternalProject_Add(
        ${CPYTHON_TARGET}
        PREFIX python-${PYTHON_VERSION}
        SOURCE_DIR ${CPYTHON_SRC_DIR}
        GIT_SUBMODULES cpython
        CONFIGURE_COMMAND ${CPYTHON_CONFIGURE_COMMAND}
        BUILD_COMMAND ${CPYTHON_BUILD_COMMAND}
        BUILD_IN_SOURCE TRUE
        INSTALL_COMMAND
                ${CMAKE_COMMAND} -E rm -rf -- "${PYTHON_INSTALL_DIR}"
        COMMAND ${CMAKE_COMMAND} -E make_directory "${PYTHON_STDLIBEXT_DIR}"

        # copy interpreter library
        COMMAND ${CMAKE_COMMAND} -E copy "${CPYTHON_SRC_DIR}/${CPYTHON_LIB}" "${CMAKE_RUNTIME_OUTPUT_DIRECTORY}"

        # copy stdlib extensions
        COMMAND bash -c "cp -rLv $(cat pybuilddir.txt)/* ${PYTHON_STDLIBEXT_DIR}"

        # copy stdlib
        COMMAND ${CMAKE_COMMAND} -E copy_directory "${CPYTHON_SRC_DIR}/Lib" "${PYTHON_STDLIB_DIR}"

        # copy project package
        COMMAND ${CMAKE_COMMAND} -E copy "${PYTHON_PROJECT_DIR}/zetaplot.py" "${PYTHON_STDLIB_DIR}"
    )
endif()

add_library(pythonlib SHARED IMPORTED)
set_target_properties(pythonlib PROPERTIES
    IMPORTED_LOCATION
        "${CMAKE_BINARY_DIR}/bin/${CPYTHON_LIB}"
    IMPORTED_LOCATION_DEBUG
        "${CMAKE_BINARY_DIR}/bin/Debug/${CPYTHON_LIB}"
    IMPORTED_LOCATION_RELEASE
        "${CMAKE_BINARY_DIR}/bin/Release/${CPYTHON_LIB}"
    IMPORTED_IMPLIB
        "${CMAKE_BINARY_DIR}/bin/${CPYTHON_IMPLIB}"
    IMPORTED_IMPLIB_DEBUG
        "${CMAKE_BINARY_DIR}/bin/Debug/${CPYTHON_IMPLIB}"
    IMPORTED_IMPLIB_RELEASE
        "${CMAKE_BINARY_DIR}/bin/Release/${CPYTHON_IMPLIB}"
)
add_dependencies(pythonlib ${CPYTHON_TARGET})

################################
####### Python setup end #######
################################

add_compile_definitions(QT_NO_SIGNALS_SLOTS_KEYWORDS)

set(ZPLOT_LIBRARY_PATH "${CMAKE_INSTALL_PREFIX}/lib/zetaplot")
configure_file(config.h.in config.h)

set(SOURCES
    source/core.cpp
    source/module.cpp
    source/script.cpp
)

add_library(zetaplot SHARED ${SOURCES})
add_dependencies(zetaplot pythonlib)

if(WIN32)
    set(LIBRARIES
        pythonlib
    )
    set(COMPILE_FLAGS
        /W4
        /std:c++20
    )
else()
    set(LIBRARIES
        pythonlib
        dl
        m
    )
    set(COMPILE_FLAGS
        -Wsign-compare
        -fwrapv
        -Wall
    )
    if(CMAKE_BUILD_TYPE STREQUAL "Debug")
        list(APPEND COMPILE_FLAGS
            -O0
            -g
        )
    else()
        list(APPEND COMPILE_FLAGS
            -O3
        )
    endif()
endif()

target_include_directories(zetaplot PUBLIC
    $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>
    $<INSTALL_INTERFACE:include/zetaplot>
    "${CMAKE_BINARY_DIR}"
    "${PYTHON_INCLUDES}"
)

target_compile_options(zetaplot PUBLIC ${COMPILE_FLAGS})
target_compile_definitions(zetaplot PRIVATE BUILD_ZETAPLOT=1)
target_link_libraries(zetaplot PUBLIC ${LIBRARIES})
set_target_properties(zetaplot PROPERTIES INSTALL_RPATH "$ENV{DESTDIR}${CMAKE_INSTALL_PREFIX}/lib/zetaplot")

if((CMAKE_BUILD_TYPE STREQUAL "Debug") OR WITH_TESTS)
    add_subdirectory("tests")
endif()

add_subdirectory("app")
