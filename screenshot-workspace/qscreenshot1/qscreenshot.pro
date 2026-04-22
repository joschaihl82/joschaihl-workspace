# qscreenshot.pro - qmake6 project file for qscreenshot.cpp with OpenCV
TEMPLATE = app
TARGET = qscreenshot

QT += core gui widgets

CONFIG += c++17
CONFIG += qt6

SOURCES += qscreenshot.cpp

unix {
    INCLUDEPATH += /usr/include/opencv4
    LIBS += -lX11 -lopencv_core -lopencv_imgproc -lopencv_imgcodecs -lopencv_highgui -lopencv_imgcodecs
}

# --- Variante B: manuelle Pfade (Fallback) ---
# Wenn pkg-config nicht vorhanden ist, entkommentiere die folgenden Zeilen
# und passe INCLUDEPATH/LIBS an dein System an.
#unix {
#
#    # Link die typischen OpenCV-Module; passe an, falls dein System andere Namen hat
#    LIBS += -lX11
#}
