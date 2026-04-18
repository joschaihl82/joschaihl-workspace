cmake_minimum_required(VERSION 3.16)
project(ext4recover LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_POSITION_INDEPENDENT_CODE ON)

# Prefer release by default
if(NOT CMAKE_BUILD_TYPE)
  set(CMAKE_BUILD_TYPE Release CACHE STRING "Build type" FORCE)
endif()

# Find Qt6 components
find_package(Qt6 REQUIRED COMPONENTS Widgets WebEngineWidgets)

# Single-source executable
add_executable(ext4recover
    ext4recover.cpp
)

# Enable automatic MOC, UIC, RCC
set_target_properties(ext4recover PROPERTIES
    AUTOMOC ON
    AUTOUIC ON
    AUTORCC ON
)

# Link Qt libraries
target_link_libraries(ext4recover PRIVATE
    Qt6::Widgets
    Qt6::WebEngineWidgets
)

# Platform-specific compile/link flags (optional)
if(UNIX)
    target_compile_definitions(ext4recover PRIVATE _POSIX_C_SOURCE=200809L)
    target_link_libraries(ext4recover PRIVATE pthread)
endif()

# Install target (optional)
install(TARGETS ext4recover RUNTIME DESTINATION bin)
