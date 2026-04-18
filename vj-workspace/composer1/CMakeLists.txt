cmake_minimum_required(VERSION 3.16)
project(nodeapp LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# Enable Qt automoc/autouic if you use signals/slots or .ui files
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTOUIC ON)
set(CMAKE_AUTORCC ON)

# Find required Qt6 components (OpenGLWidgets is required for QOpenGLWidget)
find_package(Qt6 COMPONENTS Widgets OpenGL OpenGLWidgets REQUIRED)

add_executable(nodeapp
    main.cpp
)

# Link the Qt modules; OpenGLWidgets is essential for QOpenGLWidget
target_link_libraries(nodeapp PRIVATE
    Qt6::Widgets
    Qt6::OpenGL
    Qt6::OpenGLWidgets
)

# Optional: on Qt6 use qt_finalize_executable if using qt_add_executable
# if(QT_VERSION_MAJOR EQUAL 6)
#   qt_finalize_executable(nodeapp)
# endif()
