set(QBUTTONGRID_LIBS
    exaplot
    Qt6::Widgets
)

add_library(qbuttongrid OBJECT
    qbuttongrid.cpp
    qbuttongridprivate.hpp
)
target_link_libraries(qbuttongrid PRIVATE ${QBUTTONGRID_LIBS})

if((CMAKE_BUILD_TYPE STREQUAL "Debug") OR WITH_TESTS)
    add_library(qbuttongridtests OBJECT
        tests/test.cpp
    )
    target_link_libraries(qbuttongridtests PRIVATE gtest ${QBUTTONGRID_LIBS})
endif()

if(APP_PLUGINS)
    qt_add_plugin(qbuttongridplugin)
    target_sources(qbuttongridplugin PRIVATE qbuttongridplugin.cpp $<TARGET_OBJECTS:qbuttongrid>)
    set_target_properties(qbuttongridplugin PROPERTIES WIN32_EXECUTABLE TRUE MACOSX_BUNDLE TRUE)
    target_link_libraries(qbuttongridplugin PUBLIC ${QBUTTONGRID_LIBS} Qt6::UiPlugin)
endif()
