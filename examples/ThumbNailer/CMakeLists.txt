# Project name
project(ThumbNailer)
kde_enable_exceptions()
add_executable(ThreadWeaver_ThumbNailer ThumbNailer.cpp Image.cpp Model.cpp)
qt5_use_modules(ThreadWeaver_ThumbNailer Core Widgets Test)
target_link_libraries(ThreadWeaver_ThumbNailer KF5::ThreadWeaver)
