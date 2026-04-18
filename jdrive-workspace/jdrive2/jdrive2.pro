QT += core network networkauth
QT += gui widgets

CONFIG += c++17

# Erzwinge das GUI-Flag für den Linker
linux: QMAKE_LFLAGS += -Wl,--export-dynamic

SOURCES += main.cpp
TARGET = jdrive
