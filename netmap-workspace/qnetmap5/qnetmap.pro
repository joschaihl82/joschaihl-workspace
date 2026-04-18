# qnetmap.pro
TEMPLATE = app
TARGET = qnetmap
CONFIG += c++17 console
CONFIG -= app_bundle

# Enable Qt modules
QT += core gui widgets webenginewidgets

# Sources
SOURCES += main.cpp

# Include paths (system headers)
INCLUDEPATH += /usr/include \
               /usr/include/x86_64-linux-gnu \
               /usr/include/x86_64-linux-gnu/qt5

# Link against external libraries
LIBS += -lpcap -lmaxminddb

# Compiler flags
QMAKE_CXXFLAGS += -Wall -Wextra -O2
