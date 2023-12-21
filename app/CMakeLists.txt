set(CMAKE_INCLUDE_CURRENT_DIR ON)

set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)
set(CMAKE_AUTOUIC ON)

find_package(Qt${QTVER} COMPONENTS Core Widgets Gui REQUIRED)

set(APP_UI_FILES
	mainwindow
	about
)

foreach(ui_file ${APP_UI_FILES})
	list(APPEND APP_FORM_FILES ui/${ui_file}.ui)
	list(APPEND APP_SOURCES_UI ui/${ui_file}.cpp)
endforeach(ui_file)

set(APP_SOURCES
	${APP_FORM_FILES}
	${APP_SOURCES_UI}
	appui.cpp
	appmain.cpp
	main.cpp
)

set(ADDITIONAL_LIBRARY_DEPENDENCIES
	Qt${QTVER}::Core
	Qt${QTVER}::Gui
	Qt${QTVER}::Widgets
)
add_executable(app ${APP_SOURCES})

target_include_directories(
	app PUBLIC
	"${CMAKE_CURRENT_SOURCE_DIR}/ui"
	"${CMAKE_CURRENT_SOURCE_DIR}/../include"
    "${PYTHON_INCLUDES}"
)

set_target_properties(app PROPERTIES
	OUTPUT_NAME "orbital"
	INSTALL_RPATH "$ENV{DESTDIR}${CMAKE_INSTALL_PREFIX}/${LIB_INSTALL_DIR}"
)

target_link_libraries(app PRIVATE orbital ${ADDITIONAL_LIBRARY_DEPENDENCIES})
