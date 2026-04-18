cmake_minimum_required(VERSION 3.16)

project(SimpleQtBrowser LANGUAGES CXX)

# C++ Standard
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

# Enable automatic MOC/UIC/RCC
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTOUIC ON)
set(CMAKE_AUTORCC ON)

# Find Qt6 packages
find_package(Qt6 REQUIRED COMPONENTS
    Widgets
    WebEngineWidgets
    Network
    Core
    Gui
)

# Source files
set(SOURCES
    main.cpp
)

# Create executable target
add_executable(${PROJECT_NAME}
    ${SOURCES}
)

# Link Qt libraries
target_link_libraries(${PROJECT_NAME}
    PRIVATE
        Qt6::Widgets
        Qt6::WebEngineWidgets
        Qt6::Network
        Qt6::Core
        Qt6::Gui
)

# Include directories (optional, Qt handles most)
target_include_directories(${PROJECT_NAME}
    PRIVATE
        ${CMAKE_CURRENT_SOURCE_DIR}
)

# Compiler warnings (optional)
if (MSVC)
    target_compile_options(${PROJECT_NAME} PRIVATE /W4 /permissive-)
else()
    target_compile_options(${PROJECT_NAME} PRIVATE -Wall -Wextra -Wpedantic)
endif()

# Install rule (optional)
install(TARGETS ${PROJECT_NAME} RUNTIME DESTINATION bin)

# Useful CMake variables for packaging or debugging
message(STATUS "Project: ${PROJECT_NAME}")
message(STATUS "C++ standard: ${CMAKE_CXX_STANDARD}")
message(STATUS "Qt6 include dirs: ${Qt6Widgets_INCLUDE_DIRS}")
