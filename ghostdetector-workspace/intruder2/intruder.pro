QT += widgets
CONFIG += c++17

SOURCES += main.cpp

INCLUDEPATH += /usr/include/opencv4

LIBS += -lopencv_core \
        -lopencv_imgproc \
        -lopencv_highgui \
        -lopencv_videoio \
        -lopencv_face \
        -lopencv_objdetect \
        -lopencv_imgcodecs
