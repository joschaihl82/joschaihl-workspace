cmake_minimum_required(VERSION 3.5)
project(Qt5EmailCrawler LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTOUIC OFF)
set(CMAKE_AUTORCC OFF)

find_package(Qt5 5.9 REQUIRED COMPONENTS Core Gui WebEngineWidgets WebEngineCore)

add_executable(Qt5EmailCrawler
    emailcrawl3.cpp
)

target_link_libraries(Qt5EmailCrawler
    Qt5::Core
    Qt5::Gui
    Qt5::WebEngineWidgets
    Qt5::WebEngineCore
)

