# CMake file that produces Markdown files to check if all snippet references resolve.
find_program(SNIPPETEXTRACTOR snippetextractor)
if (SNIPPETEXTRACTOR)
   # ...generate Markdown files with Snippets for HelloWorldRaw.in.md and HelloInternet.in.md
endif()

# Tell CMake we will be using C++ 11:
set(CMAKE_CXX_FLAGS "-Wall -std=c++11")

add_subdirectory(HelloWorld)
add_subdirectory(HelloWorldRaw)
add_subdirectory(HelloInternet)
add_subdirectory(ThumbNailer)
