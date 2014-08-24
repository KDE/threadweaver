# Project name
project(ThumbNailer)
kde_enable_exceptions()
set(CMAKE_INCLUDE_CURRENT_DIR ON)
set(CMAKE_AUTOMOC ON)
qt5_wrap_ui(UI_FILES MainWindow.ui)
add_executable(ThreadWeaver_ThumbNailer
    ThumbNailer.cpp
    Image.cpp
    Model.cpp
    MainWindow.cpp
    Benchmark.cpp
    PriorityDecorator.cpp
    ItemDelegate.cpp
    ImageListFilter.cpp
    ${UI_FILES}
)
qt5_use_modules(ThreadWeaver_ThumbNailer Core Widgets Test)
target_link_libraries(ThreadWeaver_ThumbNailer KF5::ThreadWeaver)
