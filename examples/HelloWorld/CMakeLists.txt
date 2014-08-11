# For more information see the CMake documentation:
# http://www.cmake.org/cmake/help/documentation.html
# http://techbase.kde.org/Development/Tutorials/CMake
cmake_minimum_required(VERSION 2.8.12)
# find KDE's extra cmake modules:
find_package(ECM 1.1.0 REQUIRED NO_MODULE)

# Project name
project(HelloWorld)

# Tell CMake we will be using C++ 11:
set(CMAKE_CXX_FLAGS "-Wall -std=c++11")

if (NOT TARGET KF5ThreadWeaver)
  # Find the ThreadWeaver framework when we compiling this example stand-alone
  find_package(KF5ThreadWeaver ${KF5_VERSION} REQUIRED)
endif()

# Add the HelloWorld executable and link the ThreadWeaver library to it
add_executable(ThreadWeaver_HelloWorld HelloWorld.cpp)
target_link_libraries(ThreadWeaver_HelloWorld KF5::ThreadWeaver)
