cmake_minimum_required(VERSION 3.5)

set(KF5_VERSION "5.68.0") # handled by release scripts
project(ThreadWeaver VERSION ${KF5_VERSION})

include(FeatureSummary)
find_package(ECM 5.67.0  NO_MODULE)
set_package_properties(ECM PROPERTIES TYPE REQUIRED DESCRIPTION "Extra CMake Modules." URL "https://commits.kde.org/extra-cmake-modules")
feature_summary(WHAT REQUIRED_PACKAGES_NOT_FOUND FATAL_ON_MISSING_REQUIRED_PACKAGES)


set(CMAKE_MODULE_PATH ${ECM_MODULE_PATH})


include(KDEInstallDirs)
include(KDEFrameworkCompilerSettings NO_POLICY_SCOPE)
include(KDECMakeSettings)

set(REQUIRED_QT_VERSION 5.12.0)
find_package(Qt5Core ${REQUIRED_QT_VERSION} REQUIRED NO_MODULE)

include(ECMGenerateExportHeader)
include(ECMSetupVersion)
include(ECMGenerateHeaders)
include(ECMAddQch)

set(EXCLUDE_DEPRECATED_BEFORE_AND_AT 0 CACHE STRING "Control the range of deprecated API excluded from the build [default=0].")

option(BUILD_QCH "Build API documentation in QCH format (for e.g. Qt Assistant, Qt Creator & KDevelop)" OFF)
add_feature_info(QCH ${BUILD_QCH} "API documentation in QCH format (for e.g. Qt Assistant, Qt Creator & KDevelop)")

ecm_setup_version(PROJECT VARIABLE_PREFIX THREADWEAVER
                        VERSION_HEADER "${CMAKE_CURRENT_BINARY_DIR}/threadweaver_version.h"
                        PACKAGE_VERSION_FILE "${CMAKE_CURRENT_BINARY_DIR}/KF5ThreadWeaverConfigVersion.cmake"
                        SOVERSION 5)
# --
add_definitions(-DQT_NO_FOREACH)
add_definitions(-DQT_DISABLE_DEPRECATED_BEFORE=0x050d00)
add_subdirectory(src)
if (BUILD_TESTING)
    add_subdirectory(autotests)
    add_subdirectory(benchmarks)
endif()
add_subdirectory(examples)

# create a Config.cmake and a ConfigVersion.cmake file and install them
set(CMAKECONFIG_INSTALL_DIR "${KDE_INSTALL_CMAKEPACKAGEDIR}/KF5ThreadWeaver")

if (BUILD_QCH)
    ecm_install_qch_export(
        TARGETS KF5ThreadWeaver_QCH
        FILE KF5ThreadWeaverQchTargets.cmake
        DESTINATION "${CMAKECONFIG_INSTALL_DIR}"
        COMPONENT Devel
    )
    set(PACKAGE_INCLUDE_QCHTARGETS "include(\"\${CMAKE_CURRENT_LIST_DIR}/KF5ThreadWeaverQchTargets.cmake\")")
endif()

include(CMakePackageConfigHelpers)

configure_package_config_file("${CMAKE_CURRENT_SOURCE_DIR}/KF5ThreadWeaverConfig.cmake.in"
                              "${CMAKE_CURRENT_BINARY_DIR}/KF5ThreadWeaverConfig.cmake"
                              INSTALL_DESTINATION  ${CMAKECONFIG_INSTALL_DIR}
                              )

install(FILES ${CMAKE_CURRENT_BINARY_DIR}/threadweaver_version.h
        DESTINATION ${KDE_INSTALL_INCLUDEDIR_KF5} COMPONENT Devel )

install(FILES
  "${CMAKE_CURRENT_BINARY_DIR}/KF5ThreadWeaverConfig.cmake"
  "${CMAKE_CURRENT_BINARY_DIR}/KF5ThreadWeaverConfigVersion.cmake"
  DESTINATION "${CMAKECONFIG_INSTALL_DIR}"
  COMPONENT Devel
)

install(EXPORT KF5ThreadWeaverTargets DESTINATION "${CMAKECONFIG_INSTALL_DIR}" FILE KF5ThreadWeaverTargets.cmake NAMESPACE KF5:: )

feature_summary(WHAT ALL FATAL_ON_MISSING_REQUIRED_PACKAGES)
