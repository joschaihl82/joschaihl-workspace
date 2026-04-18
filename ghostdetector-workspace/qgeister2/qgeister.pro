TEMPLATE = app
TARGET = qgeister1
QT += core gui widgets multimedia multimediawidgets network

CONFIG += c++17

SOURCES += main.cpp

# Wichtig für Compiler-Flags
DEFINES += QT_DEPRECATED_WARNINGS
