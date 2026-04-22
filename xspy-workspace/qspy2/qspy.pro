QT += widgets
CONFIG += c++17 console
CONFIG -= app_bundle

TEMPLATE = app
TARGET = qspy
SOURCES += qspy.cpp

# ---- Qt6: automatische MOC-Generierung ----
CONFIG += moc

# ---- AT-SPI 2.0 über pkg-config ----
CONFIG += link_pkgconfig
PKGCONFIG += atspi-2

# ---- Linux: zusätzliche Libraries (glib, gobject) ----
LIBS += -lgobject-2.0 -lglib-2.0

# ---- Compiler-Flags ----
QMAKE_CXXFLAGS += -Wall -Wextra -Wno-unused-parameter

