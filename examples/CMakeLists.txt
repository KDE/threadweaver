find_package(Qt5Widgets ${REQUIRED_QT_VERSION} REQUIRED NO_MODULE)
find_package(Qt5Test ${REQUIRED_QT_VERSION} REQUIRED NO_MODULE)

# CMake file that produces Markdown files to check if all snippet references resolve.
find_program(SNIPPETEXTRACTOR snippetextractor)
if (SNIPPETEXTRACTOR)
   # ...generate Markdown files with Snippets for HelloWorldRaw.in.md and HelloInternet.in.md
endif()

# Tell CMake we will be using C++ 11:
if (CMAKE_CXX_COMPILER_ID STREQUAL "GNU" OR CMAKE_CXX_COMPILER_ID STREQUAL "Clang")
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++0x")
elseif (CMAKE_CXX_COMPILER_ID STREQUAL "Intel" AND NOT WIN32)
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++0x")
endif()

add_subdirectory(HelloWorld)
add_subdirectory(HelloWorldRaw)
add_subdirectory(HelloInternet)
add_subdirectory(ThumbNailer)
