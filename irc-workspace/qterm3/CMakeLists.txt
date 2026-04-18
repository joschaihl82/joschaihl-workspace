cmake_minimum_required(VERSION 3.16)
project(qterm1 LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTOUIC OFF)
set(CMAKE_AUTORCC OFF)

find_package(Qt6 REQUIRED COMPONENTS Widgets)

# Optional: try to find libutil (forkpty may require it on some systems)
find_library(LIBUTIL_LIBRARY util)

add_executable(qterm1
    qterm1.cpp
)

target_link_libraries(qterm1
    PRIVATE
    Qt6::Widgets
)

if(LIBUTIL_LIBRARY)
    target_link_libraries(qterm1 PRIVATE ${LIBUTIL_LIBRARY})
endif()

# Install rule (optional)
install(TARGETS qterm1 RUNTIME DESTINATION bin)
