# jscreenrec.pro
TEMPLATE = app
TARGET = jscreenrec
CONFIG += c++17
QT += widgets multimedia multimediawidgets

SOURCES += screenrec.cpp

unix {
    LIBS += -lavformat -lavcodec -lavutil -lswscale -lswresample -lasound -lX11 -lXfixes
    # If you have libfdk-aac or other libs, add them here
    # LIBS += -lfdk-aac
}

# Add include paths if pkg-config is not used by qmake
# INCLUDEPATH += /usr/include

