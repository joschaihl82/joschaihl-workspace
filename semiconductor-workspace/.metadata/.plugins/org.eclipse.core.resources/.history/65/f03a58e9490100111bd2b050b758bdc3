cmake_minimum_required(VERSION 3.16)
project(QtPaintClone LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

find_package(Qt6 REQUIRED COMPONENTS Widgets Gui Core)

qt_standard_project_setup()

add_executable(QtPaintClone paintbrush.cpp)

target_link_libraries(QtPaintClone PRIVATE Qt6::Widgets Qt6::Gui Qt6::Core)
