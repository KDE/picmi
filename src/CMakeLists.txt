if (NOT PICMI_FILEPATH)
    set(PICMI_FILEPATH ${KDE_INSTALL_DATADIR}/picmi/)
endif ()

kde_enable_exceptions()

if (CMAKE_COMPILER_IS_GNUCXX OR CMAKE_CXX_COMPILER_ID STREQUAL "Clang")
    add_compile_options(-Wextra -pedantic)
endif ()

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
    outofboundsexception.cpp
    systemexception.cpp

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

target_link_libraries(picmi
    Qt${QT_MAJOR_VERSION}::Svg
    KF5::CoreAddons
    KF5::Crash
    KF5::DBusAddons
    KF5::I18n
    KF5::KIOWidgets
    KF5::XmlGui
    KF5KDEGames
    picmi_gui
    picmi_logic
)

install(TARGETS picmi DESTINATION ${KDE_INSTALL_TARGETS_DEFAULT_ARGS})

# vim:set ts=4 sw=4 et:
