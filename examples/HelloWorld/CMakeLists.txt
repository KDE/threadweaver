# For more information see the CMake documentation:
# http://www.cmake.org/cmake/help/documentation.html
# http://techbase.kde.org/Development/Tutorials/CMake
##@@snippet_begin(adding_tw_1)
cmake_minimum_required(VERSION 3.0)
find_package(ECM 1.1.0 REQUIRED NO_MODULE)
##@@snippet_end

if (NOT TARGET KF5ThreadWeaver)
  # Find the ThreadWeaver framework when we compiling this example stand-alone
##@@snippet_begin(adding_tw_2)
find_package(KF5ThreadWeaver ${KF5_VERSION} REQUIRED)
##@@snippet_end
endif()

##@@snippet_begin(adding_tw_3)
# Define the project name
project(HelloWorld)
# Add the HelloWorld executable and link the ThreadWeaver
# library to it
add_executable(ThreadWeaver_HelloWorld HelloWorld.cpp)
target_link_libraries(ThreadWeaver_HelloWorld KF5::ThreadWeaver)
##@@snippet_end
