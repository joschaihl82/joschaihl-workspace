cmake_minimum_required(VERSION 3.16)
project(ziparchiver_gui LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_AUTOMOC ON)

find_package(Qt6 COMPONENTS Widgets REQUIRED)
find_package(PkgConfig REQUIRED)
pkg_check_modules(LIBZIP libzip)

if(NOT LIBZIP_FOUND)
  message(FATAL_ERROR "libzip not found. Install libzip-dev / libzip.")
endif()

include_directories(${LIBZIP_INCLUDE_DIRS})
link_directories(${LIBZIP_LIBRARY_DIRS})
add_definitions(${LIBZIP_CFLAGS_OTHER})

add_executable(ziparchiver-gui qzipbak2.cpp)
target_link_libraries(ziparchiver-gui Qt6::Widgets ${LIBZIP_LIBRARIES})
