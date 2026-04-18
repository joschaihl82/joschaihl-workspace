cmake_minimum_required(VERSION 3.16)
project(qirc LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# Enable automatic Qt features
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTOUIC ON)
set(CMAKE_AUTORCC ON)

find_package(Qt6 REQUIRED COMPONENTS Widgets Network)

add_executable(qirc
    qlilt1.cpp
)

target_link_libraries(qirc PRIVATE
    Qt6::Widgets
    Qt6::Network
)

# Optional: set a default icon on Windows (uncomment and adjust)
# if (WIN32)
#     set_target_properties(qirc PROPERTIES WIN32_EXECUTABLE TRUE)
#     # add_custom_command to embed icon if needed
# endif
