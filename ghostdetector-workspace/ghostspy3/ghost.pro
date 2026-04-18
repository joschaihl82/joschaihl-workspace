# =========================================================================
# Qt Project File für Live Ghost Imaging (live_gi.cpp) mit OpenCV
# =========================================================================

QT += core gui widgets multimedia

# Name der ausführbaren Datei
TARGET = ghost_imaging_live

# Zusätzliche Konfiguration
CONFIG += console c++11

# Quellcode-Dateien
SOURCES += ghost.cpp

# --- OpenCV Konfiguration ---
# Qt verwendet pkg-config, um die notwendigen Compiler-Flags und Linker-Flags
# für OpenCV zu finden. Stellen Sie sicher, dass OpenCV auf Ihrem System installiert ist.
# Bei modernen Systemen mit OpenCV 3 oder 4 ist 'opencv' oft ausreichend.

LIBS += $(shell pkg-config --libs opencv)
INCLUDEPATH += $(shell pkg-config --cflags opencv)

# FALLS pkg-config nicht funktioniert, ersetzen Sie die obigen zwei Zeilen
# durch manuelle Pfadangaben, z.B. (Pfade müssen angepasst werden!):
# LIBS += -L/usr/local/lib -lopencv_core -lopencv_highgui -lopencv_imgproc -lopencv_videoio
# INCLUDEPATH += /usr/local/include/opencv4
