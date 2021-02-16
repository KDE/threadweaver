########### next target ###############

set(ThreadWeaver_LIB_SRCS
    queuesignals.cpp
    queuesignals_p.cpp
    queuestream.cpp
    queueapi.cpp
    exception.cpp
    queue.cpp
    weaver.cpp
    weaver_p.cpp
    debuggingaids.cpp
    thread.cpp
    job.cpp
    job_p.cpp
    iddecorator.cpp
    qobjectdecorator.cpp
    executor.cpp
    executewrapper.cpp
    state.cpp
    weaverimplstate.cpp
    inconstructionstate.cpp
    workinghardstate.cpp
    suspendingstate.cpp
    suspendedstate.cpp
    shuttingdownstate.cpp
    destructedstate.cpp
    collection.cpp
    collection_p.cpp
    sequence.cpp
    sequence_p.cpp
    dependencypolicy.cpp
    dependency.cpp
    resourcerestrictionpolicy.cpp
)

add_library(KF5ThreadWeaver ${ThreadWeaver_LIB_SRCS})
add_library(KF5::ThreadWeaver ALIAS KF5ThreadWeaver)
ecm_generate_export_header(KF5ThreadWeaver
    BASE_NAME ThreadWeaver
    GROUP_BASE_NAME KF
    VERSION ${KF_VERSION}
    DEPRECATED_BASE_VERSION 0
    DEPRECATION_VERSIONS 5.0 5.80
    EXCLUDE_DEPRECATED_BEFORE_AND_AT ${EXCLUDE_DEPRECATED_BEFORE_AND_AT}
)
target_link_libraries(KF5ThreadWeaver PUBLIC Qt5::Core)
set(threadweaver_BUILD_INCLUDE_DIRS ${CMAKE_CURRENT_SOURCE_DIR}/.. ${CMAKE_CURRENT_BINARY_DIR})
target_include_directories(KF5ThreadWeaver PUBLIC "$<BUILD_INTERFACE:${threadweaver_BUILD_INCLUDE_DIRS}>")
target_include_directories(KF5ThreadWeaver INTERFACE "$<INSTALL_INTERFACE:${KDE_INSTALL_INCLUDEDIR_KF5}/ThreadWeaver>")
#FIXME: make this PUBLIC, so that it applies to anything that links against
kde_target_enable_exceptions(KF5ThreadWeaver PRIVATE)

set_target_properties(KF5ThreadWeaver PROPERTIES
    VERSION ${THREADWEAVER_VERSION_STRING}
    SOVERSION ${THREADWEAVER_SOVERSION}
    EXPORT_NAME ThreadWeaver
)
install(TARGETS KF5ThreadWeaver EXPORT KF5ThreadWeaverTargets ${KF5_INSTALL_TARGETS_DEFAULT_ARGS})

ecm_generate_headers(ThreadWeaver_CamelCase_HEADERS
  HEADER_NAMES
    ThreadWeaver
    Weaver
    WeaverInterface
    QueueAPI
    QueueStream
    Queueing
    Exception
    QueueInterface
    Queue
    DebuggingAids
    Thread
    JobInterface
    Job
    IdDecorator
    QObjectDecorator
    Lambda
    State
    WeaverImplState
    InConstructionState
    WorkingHardState
    SuspendingState
    SuspendedState
    ShuttingDownState
    DestructedState
    Collection
    Sequence
    Dependency
    DependencyPolicy
    ResourceRestrictionPolicy
    QueueSignals
    QueuePolicy
    JobPointer
    ManagedJobPointer

    PREFIX ThreadWeaver
    REQUIRED_HEADERS ThreadWeaver_HEADERS
)
install(FILES ${ThreadWeaver_CamelCase_HEADERS} DESTINATION ${KDE_INSTALL_INCLUDEDIR_KF5}/ThreadWeaver/ThreadWeaver COMPONENT Devel)

install(FILES
    ${CMAKE_CURRENT_BINARY_DIR}/threadweaver_export.h
    ${ThreadWeaver_HEADERS}
    DESTINATION ${KDE_INSTALL_INCLUDEDIR_KF5}/ThreadWeaver/threadweaver COMPONENT Devel
)

if(BUILD_QCH)
    ecm_add_qch(
        KF5ThreadWeaver_QCH
        NAME ThreadWeaver
        BASE_NAME KF5ThreadWeaver
        VERSION ${KF_VERSION}
        ORG_DOMAIN org.kde
        SOURCES # using only public headers, to cover only public API
            ${ThreadWeaver_HEADERS}
            "${CMAKE_SOURCE_DIR}/docs/use-cases.md"
            "${CMAKE_SOURCE_DIR}/docs/whymultithreading.md"
        MD_MAINPAGE "${CMAKE_SOURCE_DIR}/README.md"
        LINK_QCHS
            Qt5Core_QCH
        INCLUDE_DIRS
            ${threadweaver_BUILD_INCLUDE_DIRS}
        BLANK_MACROS
            THREADWEAVER_EXPORT
            THREADWEAVER_DEPRECATED
            THREADWEAVER_DEPRECATED_EXPORT
            "THREADWEAVER_DEPRECATED_VERSION(x, y, t)"
        TAGFILE_INSTALL_DESTINATION ${KDE_INSTALL_QTQCHDIR}
        QCH_INSTALL_DESTINATION ${KDE_INSTALL_QTQCHDIR}
        COMPONENT Devel
    )
endif()

include(ECMGeneratePriFile)
ecm_generate_pri_file(BASE_NAME ThreadWeaver LIB_NAME KF5ThreadWeaver DEPS "core" FILENAME_VAR PRI_FILENAME INCLUDE_INSTALL_DIR ${KDE_INSTALL_INCLUDEDIR_KF5}/ThreadWeaver)
install(FILES ${PRI_FILENAME} DESTINATION ${ECM_MKSPECS_INSTALL_DIR})
