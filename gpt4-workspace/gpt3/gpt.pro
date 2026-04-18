# GptProController.pro
TEMPLATE = app
TARGET = gpt

QT += widgets webenginewidgets webchannel webenginecore

CONFIG += c++17 release
SOURCES += main.cpp

# Optional: enable warnings
QMAKE_CXXFLAGS += -Wall -Wextra -Wpedantic

