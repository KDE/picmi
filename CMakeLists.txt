cmake_minimum_required (VERSION 3.16 FATAL_ERROR)

# KDE Gear version, managed by release script
set (RELEASE_SERVICE_VERSION_MAJOR "22")
set (RELEASE_SERVICE_VERSION_MINOR "08")
set (RELEASE_SERVICE_VERSION_MICRO "1")
set (RELEASE_SERVICE_COMPACT_VERSION "${RELEASE_SERVICE_VERSION_MAJOR}${RELEASE_SERVICE_VERSION_MINOR}${RELEASE_SERVICE_VERSION_MICRO}")

# Bump PICMI_BASE_VERSION once new features are added
set(PICMI_BASE_VERSION "2.0")
set(PICMI_VERSION "${PICMI_BASE_VERSION}.${RELEASE_SERVICE_COMPACT_VERSION}")

project(picmi VERSION ${PICMI_VERSION})

set (QT_MIN_VERSION "5.15.0")
set (KF5_MIN_VERSION "5.89.0")


find_package(ECM ${KF5_MIN_VERSION} REQUIRED CONFIG)
set(CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH} ${ECM_MODULE_PATH} )
include(KDEInstallDirs)

find_package(Qt${QT_MAJOR_VERSION} ${QT_MIN_VERSION} CONFIG REQUIRED Core Widgets Svg Quick QuickWidgets Test)
find_package(KF5 ${KF5_MIN_VERSION} REQUIRED COMPONENTS
    CoreAddons
    Crash
    DBusAddons
    Declarative
    DocTools
    I18n
    KIO
    XmlGui
)

find_package(KF5KDEGames 7.3.0 REQUIRED)

include(KDECompilerSettings NO_POLICY_SCOPE)
include(KDECMakeSettings)
include(FeatureSummary)
include(ECMAddAppIcon)
include(ECMInstallIcons)
include(ECMAddTests)
include(ECMQtDeclareLoggingCategory)
include(ECMSetupVersion)

add_definitions(-DQT_DISABLE_DEPRECATED_BEFORE=0x050f02)
add_definitions(-DKF_DISABLE_DEPRECATED_BEFORE_AND_AT=0x055900)


add_subdirectory(themes)
add_subdirectory(icons)
add_subdirectory(doc)
add_subdirectory(levels)
add_subdirectory(src)
add_subdirectory(test)

install(FILES ${CMAKE_SOURCE_DIR}/org.kde.picmi.desktop DESTINATION ${KDE_INSTALL_APPDIR})
install(FILES ${CMAKE_SOURCE_DIR}/org.kde.picmi.appdata.xml DESTINATION ${KDE_INSTALL_METAINFODIR})
install(FILES ${CMAKE_SOURCE_DIR}/picmiui.rc DESTINATION ${KDE_INSTALL_KXMLGUIDIR}/picmi)

ki18n_install(po)
kdoctools_install(po)

ecm_qt_install_logging_categories(EXPORT PICMI FILE picmi.categories DESTINATION ${KDE_INSTALL_LOGGINGCATEGORIESDIR})
feature_summary(WHAT ALL INCLUDE_QUIET_PACKAGES FATAL_ON_MISSING_REQUIRED_PACKAGES)

# vim:set ts=4 sw=4 et:
