cmake_minimum_required(VERSION 3.16)
project(qt_bt_headset LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_POSITION_INDEPENDENT_CODE ON)

# Enable Qt automoc/autouic/autorcc
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTOUIC ON)
set(CMAKE_AUTORCC ON)

# Find Qt6 components
find_package(Qt6 REQUIRED COMPONENTS Core Widgets DBus Multimedia)

# Find Threads for portable pthread linking
find_package(Threads REQUIRED)

# Optional: allow overriding ALSA device or other defines via -D
option(BUILD_TESTS "Build tests" OFF)

# Source
set(SRC
    pc-dialer.cpp
)

add_executable(${PROJECT_NAME} ${SRC})

# Include Qt headers automatically via target_link_libraries
target_link_libraries(${PROJECT_NAME}
    PRIVATE
        Qt6::Core
        Qt6::Widgets
        Qt6::DBus
        Qt6::Multimedia
        Threads::Threads
)

# Link system libs (ALSA, BlueZ)
find_library(LIBASOUND asound)
find_library(LIBBLUETOOTH bluetooth)

if(LIBASOUND)
    target_link_libraries(${PROJECT_NAME} PRIVATE ${LIBASOUND})
else()
    message(WARNING "libasound not found. Install libasound2-dev (Debian/Ubuntu) or equivalent.")
endif()

if(LIBBLUETOOTH)
    target_link_libraries(${PROJECT_NAME} PRIVATE ${LIBBLUETOOTH})
else()
    message(WARNING "libbluetooth not found. Install libbluetooth-dev (Debian/Ubuntu) or equivalent.")
endif()

# Compiler warnings and definitions
if(MSVC)
    target_compile_options(${PROJECT_NAME} PRIVATE /W4 /permissive-)
else()
    target_compile_options(${PROJECT_NAME} PRIVATE -Wall -Wextra -Wpedantic)
endif()

# Install target
install(TARGETS ${PROJECT_NAME}
    RUNTIME DESTINATION bin
)

# Useful CPack packaging (optional)
include(GNUInstallDirs)
set(CPACK_PACKAGE_NAME "${PROJECT_NAME}")
set(CPACK_PACKAGE_VENDOR "User")
set(CPACK_PACKAGE_CONTACT "you@example.com")
include(CPack)
