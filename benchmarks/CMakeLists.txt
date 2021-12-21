find_package(Qt${QT_MAJOR_VERSION}Test ${REQUIRED_QT_VERSION} REQUIRED NO_MODULE)

remove_definitions(-DQT_NO_CAST_FROM_ASCII)

########### next target ###############

add_executable(ThreadWeaverQueueBenchmarks QueueBenchmarks.cpp)
# add_test(ThreadWeaverQueueBenchmarks ThreadWeaverQueueBenchmarks)

target_link_libraries(ThreadWeaverQueueBenchmarks Qt${QT_MAJOR_VERSION}::Test KF5::ThreadWeaver)
set_target_properties(ThreadWeaverQueueBenchmarks PROPERTIES MACOSX_BUNDLE FALSE)
