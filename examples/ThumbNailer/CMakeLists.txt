# Project name
project(ThumbNailer)
kde_enable_exceptions()
add_executable(ThreadWeaver_ThumbNailer ThumbNailer.cpp Image.cpp)
qt5_use_modules(ThreadWeaver_ThumbNailer Core Widgets)
target_link_libraries(ThreadWeaver_ThumbNailer KF5::ThreadWeaver)
