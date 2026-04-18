cmake_minimum_required(VERSION 3.10)
project(nettop_qt VERSION 0.1 LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTOUIC OFF)
set(CMAKE_AUTORCC OFF)

# Find Qt5 Widgets
find_package(Qt5 COMPONENTS Widgets REQUIRED)

# Find libpcap via pkg-config
find_package(PkgConfig REQUIRED)
pkg_check_modules(PCAP REQUIRED libpcap)

# Source
set(SRC
    nettop.cpp
)

add_executable(nettop_qt ${SRC})

# Include dirs from libpcap (pkg-config)
target_include_directories(nettop_qt PRIVATE ${PCAP_INCLUDE_DIRS})

# Link libraries
target_link_libraries(nettop_qt PRIVATE Qt5::Widgets ${PCAP_LIBRARIES})

# Compiler warnings (optional)
if(MSVC)
    target_compile_options(nettop_qt PRIVATE /W4 /permissive-)
else()
    target_compile_options(nettop_qt PRIVATE -Wall -Wextra -Wpedantic)
endif()

# Install (optional)
install(TARGETS nettop_qt RUNTIME DESTINATION bin)
