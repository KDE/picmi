project (picmi)

cmake_minimum_required (VERSION 3.5 FATAL_ERROR)
set (QT_MIN_VERSION "5.12.0")
set (KF5_MIN_VERSION "5.68.0")


find_package(ECM ${KF5_MIN_VERSION} REQUIRED CONFIG)
set(CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH} ${ECM_MODULE_PATH} )

find_package(Qt5 ${QT_MIN_VERSION} CONFIG REQUIRED Core Widgets Svg Quick QuickWidgets Test)
find_package(KF5 ${KF5_MIN_VERSION} REQUIRED COMPONENTS
    CoreAddons
    Crash
    DBusAddons
    Declarative
    DocTools
    I18n
    KIO
    NewStuff
    XmlGui
)

find_package(KF5KDEGames 5.2.0 REQUIRED)

include(KDEInstallDirs)
include(KDECompilerSettings NO_POLICY_SCOPE)
include(KDECMakeSettings)
include(FeatureSummary)
include(ECMAddAppIcon)
include(ECMInstallIcons)
include(ECMAddTests)
include(ECMQtDeclareLoggingCategory)


add_subdirectory(themes)
add_subdirectory(icons)
add_subdirectory(doc)
add_subdirectory(levels)
add_subdirectory(src)
add_subdirectory(test)

install(FILES ${CMAKE_SOURCE_DIR}/org.kde.picmi.desktop DESTINATION ${KDE_INSTALL_APPDIR})
install(FILES ${CMAKE_SOURCE_DIR}/org.kde.picmi.appdata.xml DESTINATION ${KDE_INSTALL_METAINFODIR})
install(FILES ${CMAKE_SOURCE_DIR}/picmiui.rc DESTINATION ${KDE_INSTALL_KXMLGUI5DIR}/picmi)
ecm_qt_install_logging_categories(EXPORT PICMI FILE picmi.categories DESTINATION ${KDE_INSTALL_LOGGINGCATEGORIESDIR})
feature_summary(WHAT ALL INCLUDE_QUIET_PACKAGES FATAL_ON_MISSING_REQUIRED_PACKAGES)

# vim:set ts=4 sw=4 et:
