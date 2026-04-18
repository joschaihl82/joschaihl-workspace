cmake_minimum_required(VERSION 3.13)
project(remove_unused_static_funcs_to_c LANGUAGES C CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

# Allow user to pass -DLLVM_DIR=/path/to/llvm/cmake
if(NOT DEFINED LLVM_DIR)
  message(STATUS "LLVM_DIR not set; you can pass -DLLVM_DIR=/path/to/llvm/cmake")
endif()

# Find LLVM (headers/definitions)
find_package(LLVM REQUIRED CONFIG)
message(STATUS "Found LLVM ${LLVM_PACKAGE_VERSION}")
message(STATUS "LLVM include dirs: ${LLVM_INCLUDE_DIRS}")

# Try to find Clang package; fall back to libclang-cpp if needed
set(USE_LIBCLANG_CPP OFF)
find_package(Clang QUIET CONFIG)

if(Clang_FOUND)
  message(STATUS "Found Clang package")
  if(TARGET clangBasic)
    message(STATUS "Clang imported targets available")
    set(CLANG_LINK_TARGETS clangTooling clangBasic clangAST clangFrontend clangLex clangRewrite)
  else()
    message(WARNING "Clang package found but clangBasic target is missing; falling back to libclang-cpp")
    set(USE_LIBCLANG_CPP ON)
  endif()
else()
  message(WARNING "Clang package not found via CMake; falling back to libclang-cpp")
  set(USE_LIBCLANG_CPP ON)
endif()

# If fallback is required, locate libclang-cpp
if(USE_LIBCLANG_CPP)
  find_library(LIBCLANG_CPP NAMES clang-cpp libclang-cpp
               PATHS /usr/lib /usr/lib64 /usr/lib/llvm-20/lib /usr/local/lib
               NO_DEFAULT_PATH)
  if(LIBCLANG_CPP)
    message(STATUS "Found libclang-cpp: ${LIBCLANG_CPP}")
  else()
    message(FATAL_ERROR "libclang-cpp not found. Install libclang-cpp-dev (or libclang-dev) or provide a working Clang CMake package. You can also set LLVM_DIR to your LLVM installation's cmake directory.")
  endif()
endif()

# Include LLVM headers and definitions
include_directories(${LLVM_INCLUDE_DIRS})
add_definitions(${LLVM_DEFINITIONS})

# Source file (adjust name if your source file differs)
set(SRC unused-optimizer1.cpp)
if(NOT EXISTS "${CMAKE_SOURCE_DIR}/${SRC}")
  file(GLOB CPP_GLOB RELATIVE ${CMAKE_SOURCE_DIR} "${CMAKE_SOURCE_DIR}/*.cpp")
  if(CPP_GLOB)
    list(SORT CPP_GLOB)
    list(GET CPP_GLOB 0 FIRST_CPP)
    message(WARNING "Default source ${SRC} not found. Using first .cpp file: ${FIRST_CPP}")
    set(SRC ${FIRST_CPP})
  else()
    message(FATAL_ERROR "No .cpp source file found. Place your source (e.g., ${SRC}) in the project root or pass -DSRC_FILES=\"file.cpp\"")
  endif()
endif()

add_executable(${PROJECT_NAME} ${SRC})

# Link appropriately depending on availability
if(Clang_FOUND AND NOT USE_LIBCLANG_CPP)
  if(DEFINED CLANG_INCLUDE_DIRS)
    target_include_directories(${PROJECT_NAME} PRIVATE ${CLANG_INCLUDE_DIRS})
  endif()
  # Link only the Clang component targets (do not require LLVM::LLVM)
  target_link_libraries(${PROJECT_NAME} PRIVATE ${CLANG_LINK_TARGETS})
else()
  target_include_directories(${PROJECT_NAME} PRIVATE ${LLVM_INCLUDE_DIRS})
  target_link_libraries(${PROJECT_NAME} PRIVATE ${LIBCLANG_CPP})
endif()

message(STATUS "To configure: cmake -DLLVM_DIR=/path/to/llvm/cmake -S . -B build")
message(STATUS "Then: cmake --build build -- -j")
