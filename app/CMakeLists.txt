set(CMAKE_PREFIX_PATH $ENV{QT_DIR})
set(CMAKE_INCLUDE_CURRENT_DIR ON)

set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)
set(CMAKE_AUTOUIC ON)

find_package(Qt6 REQUIRED COMPONENTS Core Gui PrintSupport Svg UiPlugin Widgets)

file(GLOB APP_FORM_FILES ui/*.ui)
file(GLOB APP_SOURCES_UI ui/*.cpp)

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
)

target_include_directories(app PUBLIC
	ui
	qbuttongrid
	qplottab
	qcustomplot
	qplot
	toml++
)

set_target_properties(app PROPERTIES
	OUTPUT_NAME "exaplot"
	INSTALL_RPATH "$ENV{DESTDIR}${CMAKE_INSTALL_PREFIX}/${LIB_INSTALL_DIR}"
)

target_link_libraries(app PRIVATE
	exaplot
	Qt6::Core
	Qt6::Gui
	Qt6::Widgets
	Qt6::Svg
	Qt6::PrintSupport
)
