QT += widgets
CONFIG += c++11
TEMPLATE = app
TARGET = qgeister1

# Quell- und Header-Dateien
SOURCES += main.cpp
HEADERS +=

# Verwende ALSA statt QtMultimedia
QT -= multimedia

# Linker Flags (ALSA)
unix: LIBS += -lasound

# Optional: zusätzliche Include/Lib Pfade (falls nötig)
# unix: INCLUDEPATH += /usr/include
# unix: LIBS += -L/usr/lib

# Compiler-Warnungen und Optimierungen
QMAKE_CXXFLAGS += -Wall -Wextra
release: QMAKE_CXXFLAGS += -O2
debug: QMAKE_CXXFLAGS += -g

# MOC
CONFIG += qt_no_keywords

