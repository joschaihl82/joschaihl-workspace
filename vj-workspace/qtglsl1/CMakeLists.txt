cmake_minimum_required(VERSION 3.16)
project(GleditorPro LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# Enable automoc/autorcc/autouic for single-file Qt projects
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)
set(CMAKE_AUTOUIC ON)

# Default to Release if not specified
if(NOT CMAKE_BUILD_TYPE)
  set(CMAKE_BUILD_TYPE Release CACHE STRING "Build type" FORCE)
endif()

# Find required Qt6 components
find_package(Qt6 REQUIRED COMPONENTS Widgets Gui Core OpenGL OpenGLWidgets)

add_executable(GleditorPro
    main.cpp
)

# Optional: position independent code on some platforms
if(NOT WIN32)
  set_target_properties(GleditorPro PROPERTIES POSITION_INDEPENDENT_CODE ON)
endif()

# Recommended compile definitions
target_compile_definitions(GleditorPro PRIVATE QT_DEPRECATED_WARNINGS)

# Include current source dir
target_include_directories(GleditorPro PRIVATE ${CMAKE_CURRENT_SOURCE_DIR})

# Link against the correct Qt6 libraries, including OpenGLWidgets
target_link_libraries(GleditorPro PRIVATE
    Qt6::Widgets
    Qt6::Gui
    Qt6::Core
    Qt6::OpenGL
    Qt6::OpenGLWidgets
)

# Output directory
set_target_properties(GleditorPro PROPERTIES
    RUNTIME_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/bin"
)

# Install rule (optional)
install(TARGETS GleditorPro
    RUNTIME DESTINATION bin
)
