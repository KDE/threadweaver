# Project name
project(HelloInternet)
find_package(Qt5 COMPONENTS Core Network Xml Widgets REQUIRED)
# Find includes in corresponding build directories
set(CMAKE_INCLUDE_CURRENT_DIR ON)
# Instruct CMake to run moc automatically when needed
set(CMAKE_AUTOMOC ON)
QT5_ADD_RESOURCES(helloInternet_RESOURCES_RCC HelloInternetResources.qrc)
# Add the SequenceDemo executable and link the ThreadWeaver framework:
add_executable(ThreadWeaver_HelloInternet
    main.cpp
    MainWidget.cpp
    ViewController.cpp
    ${helloInternet_RESOURCES_RCC}
)

kde_enable_exceptions()
target_link_libraries(ThreadWeaver_HelloInternet Qt5::Network Qt5::Xml Qt5::Widgets KF5::ThreadWeaver)

