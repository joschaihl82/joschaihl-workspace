cmake_minimum_required(VERSION 3.16)
project(beat_visualizer LANGUAGES CXX)

# C++ standard
set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# Automoc / Autouic / Autorcc for Qt
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTOUIC ON)
set(CMAKE_AUTORCC ON)

# Default build type if not specified
if(NOT CMAKE_BUILD_TYPE)
  set(CMAKE_BUILD_TYPE "Debug" CACHE STRING "Choose the type of build." FORCE)
endif()

# Find Qt6
find_package(Qt6 REQUIRED COMPONENTS Widgets OpenGL OpenGLWidgets)

# Prefer pkg-config for ALSA, fallback to find_library
find_package(PkgConfig QUIET)
if(PKG_CONFIG_FOUND)
  pkg_check_modules(ALSA REQUIRED alsa)
endif()

# Create executable
add_executable(beat_visualizer main.cpp)

# Include ALSA headers if found via pkg-config
if(ALSA_FOUND)
  target_include_directories(beat_visualizer PRIVATE ${ALSA_INCLUDE_DIRS})
  target_link_libraries(beat_visualizer PRIVATE ${ALSA_LIBRARIES})
else()
  find_library(ASOUND_LIB asound)
  if(ASOUND_LIB)
    target_link_libraries(beat_visualizer PRIVATE ${ASOUND_LIB})
  endif()
endif()

# Link Qt
target_link_libraries(beat_visualizer PRIVATE Qt6::Widgets Qt6::OpenGL Qt6::OpenGLWidgets)

# Threads
find_package(Threads)
if(Threads_FOUND)
  target_link_libraries(beat_visualizer PRIVATE Threads::Threads)
endif()

# Compiler flags: ensure -g3 is used by default for GCC/Clang builds
if(NOT MSVC)
  # Add -g3 for all C++ compilations (GCC/Clang). Use generator expression to apply to the target.
  target_compile_options(beat_visualizer PRIVATE
    $<$<COMPILE_LANGUAGE:CXX>:-g3>
  )

  # Recommended extra warnings (optional, keep or remove as desired)
  target_compile_options(beat_visualizer PRIVATE
    $<$<COMPILE_LANGUAGE:CXX>:-Wall;-Wextra;-Wpedantic;-Wno-unused-parameter>
  )
else()
  # MSVC: keep default debug info flags; optionally enable /Zi for debug info
  target_compile_options(beat_visualizer PRIVATE
    $<$<CONFIG:Debug>:/Zi>
  )
endif()

# Ensure position independent code if needed (optional)
set_target_properties(beat_visualizer PROPERTIES POSITION_INDEPENDENT_CODE OFF)

# Convenience: print a message about the debug flag
if(NOT MSVC)
  message(STATUS "GCC/Clang debug flag -g3 will be added to target 'beat_visualizer'")
else()
  message(STATUS "MSVC detected: using MSVC debug options (no -g3)")
endif()
