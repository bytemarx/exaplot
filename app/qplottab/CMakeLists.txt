set(QPLOTTAB_LIBS
    exaplot
    Qt6::Widgets
)
set(QPLOTTAB_INCL
    ../qcustomplot
    ../qplot
    ../../include
)

add_library(qplottab OBJECT
    qplottab.cpp
    qplottabprivate.cpp
)
target_link_libraries(qplottab PRIVATE ${QPLOTTAB_LIBS})
target_include_directories(qplottab PUBLIC ${QPLOTTAB_INCL})

if((CMAKE_BUILD_TYPE STREQUAL "Debug") OR WITH_TESTS)
    add_library(qplottabtests OBJECT
        tests/test.cpp
    )
    target_link_libraries(qplottabtests PRIVATE gtest ${QPLOTTAB_LIBS})
    target_include_directories(qplottabtests PUBLIC ${QPLOTTAB_INCL})
endif()

if(APP_PLUGINS)
    qt_add_plugin(qplottabplugin)
    target_sources(qplottabplugin PRIVATE qplottabplugin.cpp $<TARGET_OBJECTS:qplottab>)
    target_include_directories(qplottabplugin PRIVATE ${QPLOTTAB_INCL})
    set_target_properties(qplottabplugin PROPERTIES WIN32_EXECUTABLE TRUE MACOSX_BUNDLE TRUE)
    target_link_libraries(qplottabplugin PUBLIC ${QPLOTTAB_LIBS} Qt::UiPlugin)
endif()
