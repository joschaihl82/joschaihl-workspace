TEMPLATE = app
TARGET = eyectl
CONFIG += c++17 debug
QT += core gui widgets

SOURCES += eyectl.cpp

unix {
    INCLUDEPATH += /usr/include/opencv4
    LIBS += -L/usr/lib/x86_64-linux-gnu \
            -lopencv_core \
            -lopencv_imgproc \
            -lopencv_highgui \
            -lopencv_imgcodecs \
            -lopencv_videoio \
            -lopencv_objdetect \
            -lopencv_calib3d \
            -lopencv_video \
            -lfftw3 -lpthread
}

QMAKE_CXXFLAGS += -Wall -Wextra -O2
