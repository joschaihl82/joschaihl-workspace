# jbrowser.pro
QT       += widgets webenginewidgets webenginecore

CONFIG   += c++17 release
TEMPLATE = app
TARGET   = jbrowser

SOURCES += main.cpp

QMAKE_CXXFLAGS += -Wall -Wextra -Wpedantic
