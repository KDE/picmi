cmake_minimum_required (VERSION 3.16 FATAL_ERROR)

# KDE Gear version, managed by release script
set (RELEASE_SERVICE_VERSION_MAJOR "24")
set (RELEASE_SERVICE_VERSION_MINOR "01")
set (RELEASE_SERVICE_VERSION_MICRO "80")
set (RELEASE_SERVICE_COMPACT_VERSION "${RELEASE_SERVICE_VERSION_MAJOR}${RELEASE_SERVICE_VERSION_MINOR}${RELEASE_SERVICE_VERSION_MICRO}")

# Bump PICMI_BASE_VERSION once new features are added
set(PICMI_BASE_VERSION "2.0")
set(PICMI_VERSION "${PICMI_BASE_VERSION}.${RELEASE_SERVICE_COMPACT_VERSION}")

project(picmi VERSION ${PICMI_VERSION})

set(QT_MIN_VERSION "6.5.0")
set(KF_MIN_VERSION "5.240.0")

find_package(ECM ${KF_MIN_VERSION} REQUIRED CONFIG)
set(CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH} ${ECM_MODULE_PATH} )

include(KDEInstallDirs)
include(KDECMakeSettings)
include(KDECompilerSettings NO_POLICY_SCOPE)

include(FeatureSummary)
include(ECMAddAppIcon)
include(ECMInstallIcons)
include(ECMAddTests)
include(ECMQtDeclareLoggingCategory)
include(ECMDeprecationSettings)
include(ECMSetupVersion)

find_package(Qt6 ${QT_MIN_VERSION} REQUIRED COMPONENTS
    Core
    Widgets
    Svg
    Test
)
find_package(KF6 ${KF_MIN_VERSION} REQUIRED COMPONENTS
    CoreAddons
    Crash
    DBusAddons
    DocTools
    I18n
    WidgetsAddons
    XmlGui
)

find_package(KDEGames6 6.0.0 REQUIRED)

ecm_set_disabled_deprecation_versions(
    QT 6.5
    KF 5.239
    KDEGAMES 6.0
)

add_subdirectory(themes)
add_subdirectory(icons)
add_subdirectory(doc)
add_subdirectory(levels)
add_subdirectory(src)
add_subdirectory(test)

install(FILES ${CMAKE_SOURCE_DIR}/org.kde.picmi.desktop DESTINATION ${KDE_INSTALL_APPDIR})
install(FILES ${CMAKE_SOURCE_DIR}/org.kde.picmi.appdata.xml DESTINATION ${KDE_INSTALL_METAINFODIR})

ki18n_install(po)
kdoctools_install(po)

ecm_qt_install_logging_categories(EXPORT PICMI FILE picmi.categories DESTINATION ${KDE_INSTALL_LOGGINGCATEGORIESDIR})
feature_summary(WHAT ALL INCLUDE_QUIET_PACKAGES FATAL_ON_MISSING_REQUIRED_PACKAGES)

# vim:set ts=4 sw=4 et:
