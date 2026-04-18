TEMPLATE = app
TARGET = ghost
CONFIG += c++11
QT += widgets

SOURCES += ghost.cpp

DEFINES += _GNU_SOURCE

# Link against ALSA on Unix-like systems
unix {
    LIBS += -lasound
}


