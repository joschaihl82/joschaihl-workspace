# eyectl.pro - qmake6 project file for Qt6 + OpenCV4 eye tracker with ALSA and pthread
TEMPLATE = app
TARGET = eyectl
CONFIG += qt6 c++17 link_pkgconfig
QT += core gui widgets

# Use pkg-config to find OpenCV4
PKGCONFIG += opencv4

# Sources
SOURCES += \
    eyectl.cpp

# No headers, no resources (per request)

# Compiler and linker flags
QMAKE_CXXFLAGS += -Wall -Wextra -O2 -std=gnu++17
DEFINES += QT_DEPRECATED_WARNINGS

# Distribution files (optional)
DISTFILES += \
    data/haarcascade_frontalface_default.xml \
    data/haarcascade_eye.xml

# Platform-specific adjustments
win32 {
    # Windows: adjust INCLUDEPATH/LIBS if needed
}

unix {
    # rpath so the app finds OpenCV shared libs at runtime if needed
    QMAKE_RPATHDIR += /usr/lib /usr/local/lib

    # Link against ALSA for audio beep support
    LIBS += -lasound

    # Ensure pthread support for std::thread
    QMAKE_CXXFLAGS += -pthread
    LIBS += -pthread

    # Optional: copy system haarcascade into project data/ at build time if present
    SYSTEM_EYE_CASCADE = /usr/share/opencv4/haarcascades/haarcascade_eye.xml
    DEST_DATA_DIR = $$PWD/data

    # Ensure data directory exists
    QMAKE_EXTRA_TARGETS += make_data_dir
    make_data_dir.target = make_data_dir
    make_data_dir.commands = mkdir -p $$DEST_DATA_DIR
    make_data_dir.CONFIG += no_link

    # Copy system cascade if available
    QMAKE_EXTRA_TARGETS += copy_system_eye_cascade
    copy_system_eye_cascade.target = copy_system_eye_cascade
    copy_system_eye_cascade.depends = make_data_dir
    copy_system_eye_cascade.commands = test -f $$SYSTEM_EYE_CASCADE && cp $$SYSTEM_EYE_CASCADE $$DEST_DATA_DIR/haarcascade_eye.xml || echo "system cascade not found: $$SYSTEM_EYE_CASCADE"
    copy_system_eye_cascade.CONFIG += no_link

    PRE_TARGETDEPS += copy_system_eye_cascade

    # Avoid problematic characters in post-link echo
    QMAKE_POST_LINK += echo "Prepared data - system cascade copy attempted"
}

macx {
    # macOS specific flags if needed
}

# Install rules (optional)
target.path = $$[QT_INSTALL_EXAMPLES]/eyectl
INSTALLS += target
