if (NOT PICMI_FILEPATH)
    set(PICMI_FILEPATH ${KDE_INSTALL_DATADIR}/picmi/)
endif ()

kde_enable_exceptions()

include(CheckIncludeFileCXX)

configure_file(
    "${CMAKE_CURRENT_SOURCE_DIR}/config.h.in"
    "${CMAKE_CURRENT_BINARY_DIR}/config.h"
)

include_directories(
    ${CMAKE_SOURCE_DIR}
    ${CMAKE_CURRENT_BINARY_DIR}
)

ecm_setup_version(${PICMI_VERSION}
    VARIABLE_PREFIX PICMI
    VERSION_HEADER picmi_version.h
)

add_subdirectory(gui)
add_subdirectory(logic)

add_executable(picmi)

target_sources(picmi PRIVATE
    main.cpp
    outofboundsexception.cpp outofboundsexception.h
    systemexception.cpp systemexception.h

    picmi.qrc
)

ecm_qt_declare_logging_category(picmi
    HEADER picmi_debug.h
    IDENTIFIER PICMIC_LOG
    CATEGORY_NAME org.kde.kdegames.picmi
    DESCRIPTION "picmi (kdegames)"
    EXPORT PICMI
)

file(GLOB ICONS_SRCS "${CMAKE_SOURCE_DIR}/icons/*-apps-picmi.png")
ecm_add_app_icon(picmi ICONS ${ICONS_SRCS})

if (QT_MAJOR_VERSION STREQUAL "6")
    target_link_libraries(picmi KDEGames6)
else()
    target_link_libraries(picmi KF5KDEGames)
endif()

target_link_libraries(picmi
    Qt${QT_MAJOR_VERSION}::Svg
    KF${KF_MAJOR_VERSION}::CoreAddons
    KF${KF_MAJOR_VERSION}::Crash
    KF${KF_MAJOR_VERSION}::DBusAddons
    KF${KF_MAJOR_VERSION}::I18n
    KF${KF_MAJOR_VERSION}::KIOWidgets
    KF${KF_MAJOR_VERSION}::XmlGui
    picmi_gui
    picmi_logic
)

install(TARGETS picmi DESTINATION ${KDE_INSTALL_TARGETS_DEFAULT_ARGS})

# vim:set ts=4 sw=4 et:
