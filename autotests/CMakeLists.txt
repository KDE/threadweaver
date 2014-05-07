find_package(Qt5Test 5.2.0 REQUIRED NO_MODULE)

remove_definitions(-DQT_NO_CAST_FROM_ASCII)
kde_enable_exceptions()

include(ECMAddTests)

########### next target ###############

set(JobTests_SRCS JobTests.cpp AppendCharacterAndVerifyJob.cpp WaitForIdleAndFinished.cpp JobLoggingWeaver.cpp JobLoggingDecorator.cpp)

ecm_add_test(${JobTests_SRCS} TEST_NAME ThreadWeaverJobTests LINK_LIBRARIES Qt5::Test KF5::ThreadWeaver)
set_target_properties(ThreadWeaverJobTests PROPERTIES MACOSX_BUNDLE FALSE)

########### next target ###############

set(SequencesTests_SRCS SequencesTests.cpp WaitForIdleAndFinished.cpp)

ecm_add_test(${SequencesTests_SRCS} TEST_NAME ThreadWeaverSequencesTests LINK_LIBRARIES Qt5::Test KF5::ThreadWeaver)
set_target_properties(ThreadWeaverSequencesTests PROPERTIES MACOSX_BUNDLE FALSE)

########### next target ###############

set(QueueTests_SRCS QueueTests.cpp)

ecm_add_test(${QueueTests_SRCS} TEST_NAME ThreadWeaverQueueTests LINK_LIBRARIES Qt5::Test KF5::ThreadWeaver)
set_target_properties(ThreadWeaverQueueTests PROPERTIES MACOSX_BUNDLE FALSE)

########### next target ###############

set(DeleteTest_SRCS DeleteTest.cpp)

ecm_add_test(${DeleteTest_SRCS} TEST_NAME ThreadWeaverDeleteTest LINK_LIBRARIES Qt5::Test KF5::ThreadWeaver)
set_target_properties(ThreadWeaverDeleteTest PROPERTIES MACOSX_BUNDLE FALSE)

########### next target ###############

set(LifecycleTest_SRCS LifecycleTests.cpp)

ecm_add_test(${LifecycleTest_SRCS} TEST_NAME ThreadWeaverLifecycleTest LINK_LIBRARIES Qt5::Test KF5::ThreadWeaver)
set_target_properties(ThreadWeaverLifecycleTest PROPERTIES MACOSX_BUNDLE FALSE)

########### next target ###############

set(ShutdownOnQApplicationQuitTest_SRCS ShutdownOnQApplicationQuitTests.cpp)

ecm_add_test(${ShutdownOnQApplicationQuitTest_SRCS} TEST_NAME ThreadWeaverShutdownOnQApplicationQuitTest LINK_LIBRARIES Qt5::Test KF5::ThreadWeaver)
set_target_properties(ThreadWeaverShutdownOnQApplicationQuitTest PROPERTIES MACOSX_BUNDLE FALSE)

########### next target ###############

set(QueueFactoryTest_SRCS QueueFactoryTests.cpp)

ecm_add_test(${QueueFactoryTest_SRCS} TEST_NAME ThreadWeaverQueueFactoryTest LINK_LIBRARIES Qt5::Test KF5::ThreadWeaver)
Set_target_properties(ThreadWeaverQueueFactoryTest PROPERTIES MACOSX_BUNDLE FALSE)
