add_library(picmi_logic STATIC)

target_sources(picmi_logic PRIVATE
    board.cpp
    boardmap.cpp
    boardstate.cpp
    elapsedtime.cpp
    levelloader.cpp
    picmi.cpp
    streaks.cpp
    settings.cpp
)

target_link_libraries(picmi_logic
    KF5KDEGames
    KF5::CoreAddons
    KF5::I18n
    Qt${QT_MAJOR_VERSION}::Core
    Qt${QT_MAJOR_VERSION}::Xml
)

# vim:set ts=4 sw=4 et:
