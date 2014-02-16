########### next target ###############

set(ThreadWeaver_LIB_SRCS
    queuesignals.cpp
    queuestream.cpp
    queueapi.cpp
    exception.cpp
    queue.cpp
    weaver.cpp
    debuggingaids.cpp
    thread.cpp
    job.cpp
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
    sequence.cpp
    dependencypolicy.cpp
    dependency.cpp
    resourcerestrictionpolicy.cpp
)

add_library(KF5ThreadWeaver ${ThreadWeaver_LIB_SRCS})
generate_export_header(KF5ThreadWeaver BASE_NAME ThreadWeaver)
add_library(KF5::ThreadWeaver ALIAS KF5ThreadWeaver)
target_link_libraries(KF5ThreadWeaver PUBLIC Qt5::Core)
target_include_directories(KF5ThreadWeaver PUBLIC "$<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/..;${CMAKE_CURRENT_BINARY_DIR}>")
target_include_directories(KF5ThreadWeaver INTERFACE "$<INSTALL_INTERFACE:${INCLUDE_INSTALL_DIR}/ThreadWeaver>")
#FIXME: make this PUBLIC, so that it applies to anything that links against
#       KF5::ThreadWeaver?
kde_target_enable_exceptions(KF5ThreadWeaver PRIVATE)

set_target_properties(KF5ThreadWeaver PROPERTIES
    VERSION ${THREADWEAVER_VERSION_STRING}
    SOVERSION ${THREADWEAVER_SOVERSION}
    EXPORT_NAME ThreadWeaver
)
install(TARGETS KF5ThreadWeaver EXPORT KF5ThreadWeaverTargets ${INSTALL_TARGETS_DEFAULT_ARGS})

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
install(FILES ${ThreadWeaver_CamelCase_HEADERS} DESTINATION ${INCLUDE_INSTALL_DIR}/ThreadWeaver/ThreadWeaver COMPONENT Devel)

install(FILES
    ${CMAKE_CURRENT_BINARY_DIR}/threadweaver_export.h
    ${ThreadWeaver_HEADERS}
    DESTINATION ${INCLUDE_INSTALL_DIR}/ThreadWeaver/threadweaver COMPONENT Devel
)

include(ECMGeneratePriFile)
ecm_generate_pri_file(BASE_NAME ThreadWeaver LIB_NAME KF5ThreadWeaver DEPS "core" FILENAME_VAR PRI_FILENAME)
install(FILES ${PRI_FILENAME} DESTINATION ${ECM_MKSPECS_INSTALL_DIR})
