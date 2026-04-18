# nettop.pro - qmake project for the nettop single-file application
TEMPLATE = app
TARGET = nettop

# C++ standard and basic build config
CONFIG += c++17 release
CONFIG += warn_on
QMAKE_CXXFLAGS += -Wall -Wextra -Wpedantic

# Qt modules used
QT += widgets network concurrent opengl

# Single-source project
SOURCES += nettop.cpp

# Optional resources or headers (none required for the single-file build)
HEADERS =
RESOURCES =

# Include paths (add any custom include dirs here)
INCLUDEPATH += /usr/include

# Link with libpcap and pthread on Unix
unix {
    LIBS += -lpcap
    LIBS += -lpthread
}

# If you prefer pkg-config to find Qt libs on some systems, you can enable:
# CONFIG += link_pkgconfig
# PKGCONFIG += Qt5Widgets Qt5Network Qt5Concurrent Qt5OpenGL

# Installation (optional)
target.path = /usr/local/bin
INSTALLS += target

# Useful debug/help targets (uncomment if you want)
# QMAKE_CXXFLAGS_DEBUG += -g
# CONFIG(debug, debug|release) {
#     message("Building debug")
# }

# End of nettop.pro

