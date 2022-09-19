add_library(picmi_gui STATIC)

target_sources(picmi_gui PRIVATE
    mainwindow.cpp
    renderer.cpp
    scene.cpp
    selectboardwindow.cpp
    settingswindow.cpp
    view.cpp
)

target_sources(picmi_gui PRIVATE
    graphicsitems/cellitem.cpp
    graphicsitems/highlightitem.cpp
    graphicsitems/pixmapitem.cpp
    graphicsitems/reloadableitem.cpp
    graphicsitems/streakitem.cpp
    graphicsitems/textbanneritem.cpp
)

ki18n_wrap_ui(picmi_gui
    ${CMAKE_CURRENT_SOURCE_DIR}/settingswindow.ui
    ${CMAKE_CURRENT_SOURCE_DIR}/selectboardwindow.ui
)

target_link_libraries(picmi_gui
    KF5KDEGames
    KF5::CoreAddons
    KF5::I18n
    KF5::XmlGui
    Qt${QT_MAJOR_VERSION}::Core
    Qt${QT_MAJOR_VERSION}::Svg
)

# vim:set ts=4 sw=4 et:
