set(CMAKE_INCLUDE_CURRENT_DIR ON)

set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)
set(CMAKE_AUTOUIC ON)

find_package(Qt6 REQUIRED COMPONENTS Core Widgets Gui Svg PrintSupport)

set(APP_UI_FILES
	mainwindow
	about
	ploteditor
)

foreach(ui_file ${APP_UI_FILES})
	list(APPEND APP_FORM_FILES ui/${ui_file}.ui)
	list(APPEND APP_SOURCES_UI ui/${ui_file}.cpp)
endforeach(ui_file)

set(QBUTTONGRID_SOURCES
	qbuttongrid/qbuttongrid.cpp
	qbuttongrid/qbuttongrid.hpp
	qbuttongrid/qbuttongridprivate.hpp
)

set(QPLOTTAB_SOURCES
	qplottab/qplottab.cpp
	qplottab/qplottab.hpp
	qplottab/qplottabprivate.cpp
	qplottab/qplottabprivate.hpp
)

set(QCUSTOMPLOT_SOURCES
	qcustomplot/qcustomplot.cpp
	qcustomplot/qcustomplot.h
)

set(APP_SOURCES
	${APP_FORM_FILES}
	${APP_SOURCES_UI}
	${QBUTTONGRID_SOURCES}
	${QPLOTTAB_SOURCES}
	${QCUSTOMPLOT_SOURCES}
	appui.cpp
	appmain.cpp
	main.cpp
	resources.qrc
)

set(ADDITIONAL_LIBRARY_DEPENDENCIES
	Qt6::Core
	Qt6::Gui
	Qt6::Widgets
	Qt6::Svg
	Qt6::PrintSupport
)
add_executable(app ${APP_SOURCES})

target_include_directories(
	app PUBLIC
	"${CMAKE_CURRENT_SOURCE_DIR}/ui"
	"${CMAKE_CURRENT_SOURCE_DIR}/qbuttongrid"
	"${CMAKE_CURRENT_SOURCE_DIR}/qplottab"
	"${CMAKE_CURRENT_SOURCE_DIR}/qcustomplot"
	"${CMAKE_CURRENT_SOURCE_DIR}/../include"
    "${PYTHON_INCLUDES}"
)

set_target_properties(app PROPERTIES
	OUTPUT_NAME "orbital"
	INSTALL_RPATH "$ENV{DESTDIR}${CMAKE_INSTALL_PREFIX}/${LIB_INSTALL_DIR}"
)

target_link_libraries(app PRIVATE orbital ${ADDITIONAL_LIBRARY_DEPENDENCIES})

add_subdirectory("qbuttongrid")
add_subdirectory("qplottab")
