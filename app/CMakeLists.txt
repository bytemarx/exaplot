set(CMAKE_PREFIX_PATH $ENV{QT_DIR})
set(CMAKE_INCLUDE_CURRENT_DIR ON)

set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)
set(CMAKE_AUTOUIC ON)

find_package(Qt6
	REQUIRED COMPONENTS
	Core
	Gui
	PrintSupport
	Svg
	UiPlugin
	Widgets
)

find_package(ZLIB REQUIRED)

ExternalProject_Add(
	hdf5
	PREFIX ${CMAKE_BINARY_DIR}/app/hdf5
	SOURCE_DIR ${CMAKE_SOURCE_DIR}/app/hdf5
	CMAKE_ARGS -DCMAKE_INSTALL_PREFIX=<INSTALL_DIR>
)

file(GLOB APP_SOURCES_UI ui/*.cpp)
file(GLOB APP_FORM_FILES ui/*.ui)

add_subdirectory(qbuttongrid)
add_subdirectory(qplottab)
add_subdirectory(qplot)

add_executable(app
	main.cpp
	${APP_FORM_FILES}
	${APP_SOURCES_UI}
	resources.qrc
	appmain.cpp
	appinterface.cpp
	appui.cpp
	qcustomplot/qcustomplot.cpp
	$<TARGET_OBJECTS:qbuttongrid>
	$<TARGET_OBJECTS:qplottab>
	$<TARGET_OBJECTS:qplot>
	datamanager.cpp
)

target_include_directories(app PUBLIC
	ui
	qbuttongrid
	qplottab
	qcustomplot
	qplot
	toml++
	${CMAKE_BINARY_DIR}/app/hdf5/include
)

set_target_properties(app PROPERTIES
	OUTPUT_NAME "exaplot"
	INSTALL_RPATH "$ENV{DESTDIR}${CMAKE_INSTALL_PREFIX}/${LIB_INSTALL_DIR}"
)

target_link_libraries(app PRIVATE
	exaplot
	Qt6::Core
	Qt6::Gui
	Qt6::PrintSupport
	Qt6::Svg
	Qt6::Widgets
	${CMAKE_BINARY_DIR}/app/hdf5/lib/libhdf5.a
	ZLIB::ZLIB
)
