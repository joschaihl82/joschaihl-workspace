# xspy.pro - qmake6 project for xspy (Qt6 + X11 + XComposite + XRender + AT-SPI)
# Uses pkg-config to discover include paths and libs robustly (no sed).
QT += widgets concurrent
CONFIG += c++17 release
TEMPLATE = app
TARGET = xspy

SOURCES += xspy.cpp

# Primary pkg-config packages required
PKGCONFIG += atspi-2 x11 xcomposite xrender gobject-2.0 glib-2.0 dbus-1

unix {
    # Get cflags (may contain -I... tokens). Use qmake replace to strip "-I".
    pkg_cflags = $$system(pkg-config --cflags-only-I atspi-2 x11 xcomposite xrender gobject-2.0 glib-2.0 dbus-1 2>/dev/null)
    pkg_cflags = $$replace(pkg_cflags, "-I", "")
    # Add include paths (space-separated list is accepted)
    INCLUDEPATH += $$pkg_cflags

    # Get libs and linker flags
    pkg_libs = $$system(pkg-config --libs atspi-2 x11 xcomposite xrender gobject-2.0 glib-2.0 dbus-1 2>/dev/null)
    LIBS += $$pkg_libs
}

# Fallbacks if pkg-config is not available or packages are installed in nonstandard locations.
# Uncomment and adjust paths if needed.
# INCLUDEPATH += /usr/include /usr/local/include
# LIBS += -latspi -lX11 -lXcomposite -lXrender -lgobject-2.0 -lglib-2.0 -ldbus-1

# Extra compiler flags
QMAKE_CXXFLAGS += -Wall -Wextra -Wpedantic

# Link pthread (QtConcurrent may require it on some systems)
LIBS += -lpthread

# Optional install target
target.path = /usr/local/bin
INSTALLS += target

