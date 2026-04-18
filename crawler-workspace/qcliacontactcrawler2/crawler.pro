# crawler.pro
# qmake project for the single-file Qt5 + crawler amalgamation (crawler.cpp)
# Usage:
#   qmake -project -o crawler.pro "CONFIG+=qt warn_off"   # (optional helper)
#   qmake crawler.pro
#   make
#
# This .pro expects crawler.cpp to contain both the crawler core and the Qt GUI.

QT += widgets
CONFIG += c++17
TEMPLATE = app
TARGET = crawler

# Source file (single amalgamated file)
SOURCES += crawler.cpp

# Use pkg-config to pull in libxml2, libcurl, openssl and sqlite3 flags
CONFIG += link_pkgconfig
PKGCONFIG += libxml-2.0 libcurl openssl sqlite3

# Additional libraries
LIBS += -lpthread

# Optional: reduce warnings or add defines
# QMAKE_CXXFLAGS += -Wall -Wextra -O2

# Install path (optional)
# target.path = /usr/local/bin
# INSTALLS += target

