QT += core gui widgets multimedia charts
CONFIG += c++17
SOURCES += geiger.cpp
# Linux-spezifische Bibliotheken hinzufügen
LIBS += -lasound

