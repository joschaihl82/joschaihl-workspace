cmake_minimum_required(VERSION 3.16)
project(BeatVisualizerQt6 LANGUAGES CXX)

# --- Project settings ---
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# Enable Qt automatic features
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTOUIC ON)
set(CMAKE_AUTORCC ON)

# Find Qt6 components (including OpenGLWidgets for QOpenGLWidget)
find_package(Qt6 REQUIRED COMPONENTS Widgets Gui OpenGL OpenGLWidgets)

# Find OpenGL (modern CMake target OpenGL::GL)
find_package(OpenGL REQUIRED)

# ALSA (libasound)
find_library(ASOUND_LIB asound)
if(NOT ASOUND_LIB)
  message(FATAL_ERROR "ALSA library (asound) not found. Install libasound2-dev or equivalent.")
endif()

# Create executable
add_executable(beatvisualizer
    main.cpp
)

# Link against Qt and OpenGL
target_link_libraries(beatvisualizer
    PRIVATE
        Qt6::Widgets
        Qt6::Gui
        Qt6::OpenGL
        Qt6::OpenGLWidgets
        OpenGL::GL
        ${ASOUND_LIB}
)

# --- Explicit include directories (generator expressions) ---
# Qt imported targets already propagate include dirs, but expose them explicitly for IDEs/tools.
target_include_directories(beatvisualizer
    PRIVATE
        $<TARGET_PROPERTY:Qt6::Widgets,INTERFACE_INCLUDE_DIRECTORIES>
        $<TARGET_PROPERTY:Qt6::Gui,INTERFACE_INCLUDE_DIRECTORIES>
        $<TARGET_PROPERTY:Qt6::OpenGL,INTERFACE_INCLUDE_DIRECTORIES>
        $<TARGET_PROPERTY:Qt6::OpenGLWidgets,INTERFACE_INCLUDE_DIRECTORIES>
        $<TARGET_PROPERTY:OpenGL::GL,INTERFACE_INCLUDE_DIRECTORIES>
)

# If the system provides OPENGL_INCLUDE_DIR variable, add it as well (compat)
if(DEFINED OPENGL_INCLUDE_DIR)
    target_include_directories(beatvisualizer PRIVATE ${OPENGL_INCLUDE_DIR})
endif()

# Compiler warnings and useful flags
if(MSVC)
    target_compile_options(beatvisualizer PRIVATE /W4 /permissive-)
else()
    target_compile_options(beatvisualizer PRIVATE -Wall -Wextra -Wpedantic -Wno-unused-parameter)
    # Optional: enable LTO when supported
    include(CheckIPOSupported)
    check_ipo_supported(RESULT ipo_supported OUTPUT ipo_error)
    if(ipo_supported)
        set_target_properties(beatvisualizer PROPERTIES INTERPROCEDURAL_OPTIMIZATION TRUE)
    endif()
endif()

# RPATH handling for runtime on Unix-like systems (optional)
if(UNIX AND NOT APPLE)
    set_target_properties(beatvisualizer PROPERTIES INSTALL_RPATH "$ORIGIN/../lib")
endif()

# Installation (optional)
install(TARGETS beatvisualizer RUNTIME DESTINATION bin)

# Helpful summary printed during configure
message(STATUS "Configuring BeatVisualizerQt6")
message(STATUS "  Qt6::Widgets include dirs: $<TARGET_PROPERTY:Qt6::Widgets,INTERFACE_INCLUDE_DIRECTORIES>")
message(STATUS "  Qt6::OpenGLWidgets include dirs: $<TARGET_PROPERTY:Qt6::OpenGLWidgets,INTERFACE_INCLUDE_DIRECTORIES>")
message(STATUS "  OpenGL include dirs: $<TARGET_PROPERTY:OpenGL::GL,INTERFACE_INCLUDE_DIRECTORIES>")
message(STATUS "  ALSA library: ${ASOUND_LIB}")
