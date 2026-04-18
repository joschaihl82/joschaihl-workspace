TEMPLATE = app
TARGET = qproxyperf

QT += widgets

CONFIG += c++11
CONFIG += warn_on

SOURCES += main.cpp

# If you split code into multiple files, add them here:
# SOURCES += other.cpp
# HEADERS += other.h

# Use pthread on Unix if needed
unix: LIBS += -lpthread

# Installation (optional)
# target.path = /usr/local/bin
# INSTALLS += target
