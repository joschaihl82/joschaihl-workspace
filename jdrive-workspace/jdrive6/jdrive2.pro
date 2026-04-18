QT += core gui widgets network networkauth

CONFIG += c++17
CONFIG += strict_c++ # Hilft bei der Einhaltung des C++17 Standards

# Erzwinge das GUI-Flag für den Linker (Linux spezifisch)
linux: QMAKE_LFLAGS += -Wl,--export-dynamic

SOURCES += main.cpp
TARGET = jdrive

# Optional, aber empfohlen:
# Zeigt eine Warnung beim Kompilieren, falls SSL (für HTTPS) im Qt-Build fehlen sollte.
contains(QT_CONFIG, ssl) {
    message("SSL support is enabled.")
} else {
    warning("SSL support is missing in this Qt build! Google OAuth will fail.")
}
