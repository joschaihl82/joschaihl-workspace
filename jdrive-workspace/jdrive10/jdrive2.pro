# jdrive.pro - qmake project file for DriveSync / jdrive
# This file forces qmake to use qmake6 (Qt6) by setting QMAKE to the qmake6 binary.
# Adjust QMAKE_PATH if your qmake6 is located elsewhere.

TEMPLATE = app
TARGET = jdrive
CONFIG += c++17 warn_on release
QT += core gui widgets network networkauth

# --- Force qmake binary (adjust path if necessary) ---
# Typical locations:
#  - /usr/lib/qt6/bin/qmake6
#  - /usr/bin/qmake6
#  - /opt/Qt/6.x.x/gcc_64/bin/qmake
# Set QMAKE to the absolute path of your qmake6 binary.
QMAKE = /usr/lib/qt6/bin/qmake6

# Sources / headers
SOURCES += \
    main.cpp

HEADERS +=

INCLUDEPATH += $$PWD

# Ensure automoc is active (qmake normally enables this)
CONFIG += qt

# Keep main.moc include at end of main.cpp if using Q_OBJECT in the same file
