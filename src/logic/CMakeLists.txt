add_library(picmi_logic STATIC)

target_sources(picmi_logic PRIVATE
    board.cpp board.h
    boardmap.cpp boardmap.h
    boardstate.cpp boardstate.h
    elapsedtime.cpp elapsedtime.h
    levelloader.cpp levelloader.h
    picmi.cpp picmi.h
    streaks.cpp streaks.h
    settings.cpp settings.h
)

target_link_libraries(picmi_logic
    KDEGames6
    KF6::CoreAddons
    KF6::I18n
    Qt6::Core
    Qt6::Xml
)

# vim:set ts=4 sw=4 et:
