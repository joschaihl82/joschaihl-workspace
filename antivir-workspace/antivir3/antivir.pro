# antivir.pro — qmake Projektdatei für Qt5/Widgets
# Erzeugt ein einzelnes GUI-Programm aus antivirus.cpp

QT       += widgets network
CONFIG   += c++17 console
CONFIG   -= app_bundle
TEMPLATE = app

TARGET   = ClamAVQuickScan

# Hauptquelle (amalgamiert)
SOURCES += antivir.cpp

# Optionale Warnungen/Optimierungen
QMAKE_CXXFLAGS += -Wall -Wextra
QMAKE_CXXFLAGS_RELEASE += -O2

# Linux-spezifisch: setze Ausgabepfad
unix {
    DESTDIR = bin
}

# Für MOC am Dateiende (include "antivirus.moc")
# qmake generiert automatisch MOC-Dateien für QObject/Q_OBJECT Klassen

