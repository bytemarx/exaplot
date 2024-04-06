set(QPLOT_LIBS
    Qt6::Widgets
)
set(QPLOT_INCL
    ../qcustomplot
)

add_library(qplot OBJECT
    plot.cpp
    plot2d.cpp
    plotcolormap.cpp
    qplot.cpp
)
target_link_libraries(qplot PRIVATE ${QPLOT_LIBS})
target_include_directories(qplot PUBLIC ${QPLOT_INCL})

if((CMAKE_BUILD_TYPE STREQUAL "Debug") OR WITH_TESTS)
    add_library(qplottests OBJECT
        tests/test.cpp
    )
    target_link_libraries(qplottests PRIVATE gtest ${QPLOT_LIBS})
    target_include_directories(qplottests PUBLIC ${QPLOT_INCL})
endif()

if(APP_PLUGINS)
    qt_add_plugin(qplotplugin)
    target_sources(qplotplugin PRIVATE qplotplugin.cpp $<TARGET_OBJECTS:qplot>)
    target_include_directories(qplotplugin PRIVATE ${QPLOT_INCL})
    set_target_properties(qplotplugin PROPERTIES WIN32_EXECUTABLE TRUE MACOSX_BUNDLE TRUE)
    target_link_libraries(qplotplugin PUBLIC ${QPLOT_LIBS} Qt6::UiPlugin)
endif()
