add_library(picmi_gui STATIC)

target_sources(picmi_gui PRIVATE
    mainwindow.cpp mainwindow.h
    renderer.cpp renderer.h
    scene.cpp scene.h
    selectboardwindow.cpp selectboardwindow.h
    settingswindow.cpp settingswindow.h
    view.cpp view.h
)

target_sources(picmi_gui PRIVATE
    graphicsitems/cellitem.cpp graphicsitems/cellitem.h
    graphicsitems/highlightitem.cpp graphicsitems/highlightitem.h
    graphicsitems/pixmapitem.cpp graphicsitems/pixmapitem.h
    graphicsitems/reloadableitem.cpp graphicsitems/reloadableitem.h
    graphicsitems/streakitem.cpp graphicsitems/streakitem.h
    graphicsitems/textbanneritem.cpp graphicsitems/textbanneritem.h
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
