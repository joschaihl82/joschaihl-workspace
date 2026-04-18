# gpt.pro — single-file Qt project

TEMPLATE = app
TARGET = gpt
CONFIG += c++17 warn_on release
QMAKE_CXXFLAGS += -Wall -Wextra -Wpedantic

# Qt modules needed
QT += core gui widgets network

# Only one source file
SOURCES += main.cpp

