# jide.pro - Qt project for the simple IDE (ide.cpp)

QT       += core gui widgets

CONFIG   += c++17 console
CONFIG   -= app_bundle

TEMPLATE = app
TARGET   = jide

SOURCES += ide.cpp

# If you split code into headers/sources later, add them here:
# HEADERS += ...
# FORMS += ...

# Include path adjustments (optional)
#INCLUDEPATH += $$PWD/include

# Linker flags (normal Qt libs are linked automatically)
# unix: LIBS += -lm

# Enable MOC for single-file with #include "ide.moc"
# (no extra settings required; qmake handles moc automatically)

# Deployment / build settings
DESTDIR = bin
