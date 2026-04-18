cmake_minimum_required(VERSION 3.16)
project(shader_ide LANGUAGES CXX)

# C++ Standard
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

# Automoc für Q_OBJECT
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTOUIC OFF)
set(CMAKE_AUTORCC OFF)
set(CMAKE_INCLUDE_CURRENT_DIR ON)

# Falls Qt an einem nicht-standard Ort liegt, beim cmake-Aufruf Qt6_DIR setzen:
# cmake -DQt6_DIR=/path/to/Qt6/lib/cmake/Qt6 ..

# Benötigte Qt-Module: Widgets + OpenGLWidgets (+ OpenGL)
find_package(Qt6 COMPONENTS Widgets OpenGLWidgets OpenGL REQUIRED)

# Executable
add_executable(shader_ide
    main.cpp
)

# Link libraries: Qt + ALSA (libasound)
target_link_libraries(shader_ide
    PRIVATE
        Qt6::Widgets
        Qt6::OpenGLWidgets
        Qt6::OpenGL
        asound
)

# Optional: zusätzliche Compiler-Warnungen aktivieren
if (MSVC)
    target_compile_options(shader_ide PRIVATE /W4 /permissive-)
else()
    target_compile_options(shader_ide PRIVATE -Wall -Wextra -Wpedantic)
endif()

# Install rule (optional)
install(TARGETS shader_ide RUNTIME DESTINATION bin)
