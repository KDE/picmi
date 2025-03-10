if(NOT PICMI_FILEPATH)
    set(PICMI_FILEPATH ${KDE_INSTALL_DATADIR}/picmi/)
endif()

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
    CATEGORY_NAME picmi
    OLD_CATEGORY_NAMES org.kde.kdegames.picmi
    DESCRIPTION "Picmi game"
    EXPORT PICMI
)

file(GLOB ICONS_SRCS "${CMAKE_SOURCE_DIR}/icons/*-apps-picmi.png")
ecm_add_app_icon(picmi ICONS ${ICONS_SRCS})

if(APPLE)
    set_target_properties(picmi PROPERTIES
           MACOSX_BUNDLE_DISPLAY_NAME "Picmi"
        MACOSX_BUNDLE_BUNDLE_NAME "Picmi"
       MACOSX_BUNDLE_LONG_VERSION_STRING "Picmi ${RELEASE_SERVICE_VERSION}"
        MACOSX_BUNDLE_SHORT_VERSION_STRING "${RELEASE_SERVICE_VERSION_MAJOR}.${RELEASE_SERVICE_VERSION_MINOR}"
        MACOSX_BUNDLE_BUNDLE_VERSION "${RELEASE_SERVICE_VERSION}"
        MACOSX_BUNDLE_GUI_IDENTIFIER "org.kde.picmi"
        MACOSX_BUNDLE_COPYRIGHT "2024 The Picmi Developers")
endif()


target_link_libraries(picmi
    picmi_gui
    picmi_logic
    KDEGames6
    KF6::CoreAddons
    KF6::Crash
    KF6::DBusAddons
    KF6::I18n
    KF6::IconThemes
    KF6::XmlGui
    Qt6::Svg
)

install(TARGETS picmi DESTINATION ${KDE_INSTALL_TARGETS_DEFAULT_ARGS})

# vim:set ts=4 sw=4 et:
