# Project name
project(HelloWorldRaw)

# Add the HelloWorldRaw executable and link the ThreadWeaver framework:
add_executable(ThreadWeaver_HelloWorldRaw HelloWorldRaw.cpp)
target_link_libraries(ThreadWeaver_HelloWorldRaw KF5::ThreadWeaver)
