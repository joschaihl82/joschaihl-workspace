TEMPLATE = app
CONFIG += console c++17
QT += widgets core gui

SOURCES += main.cpp

INCLUDEPATH += /usr/include/opencv4 /usr/local/include /usr/include

LIBS += -L/usr/lib -L/usr/local/lib \
    -lopencv_core -lopencv_imgproc -lopencv_highgui -lopencv_videoio -lopencv_imgcodecs \
    -ldlib \
    -lopenblas -llapack -lblas \
    -lX11 -lXtst -lpthread

QMAKE_CXXFLAGS += -Wall -Wextra -O3 -march=native -ffast-math -g3
