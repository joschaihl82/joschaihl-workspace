# qgeister.pro - Qt qmake project for qgeister "CIA Modell Berlin 6"
TEMPLATE = app
TARGET = qgeister
CONFIG += c++17 warn_on release
QT += widgets core gui

SOURCES += \
    main.cpp

# If you split code into headers/sources, list them here:
# HEADERS += \
#     geigerlogic.h \
#     audioengine.h

# Compiler flags
QMAKE_CXXFLAGS += -Wall -Wextra -Wpedantic

# Link with ALSA (Linux)
unix {
    LIBS += -lasound
}

# Deployment / resources (uncomment if you add .qrc)
# RESOURCES += resources.qrc

# Output directory (optional)
DESTDIR = bin

# Packaging: include debug info if needed
# CONFIG += debug_and_release

