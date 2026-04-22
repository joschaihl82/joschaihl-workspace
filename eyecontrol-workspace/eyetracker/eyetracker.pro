# eyetracker.pro - qmake6 project file for EyeTracker (Qt6 + OpenCV4 + dlib)
# Builds a Qt6 application that uses OpenCV4 and dlib.
# Includes explicit BLAS/LAPACK linking to resolve dlib -> cblas_dgemm undefined reference.
# Compiler flags: -O3 -g3 -ffast-math -march=native -flto and common warnings enabled.

QT += widgets gui core
CONFIG += c++17 link_pkgconfig

# Use pkg-config to find OpenCV 4
PKGCONFIG += opencv4

# Sources
SOURCES += \
    main.cpp

# Optional: set DLIB_DIR to the dlib include/lib location if not installed system-wide
# Usage example:
#   qmake6 "DLIB_DIR=/path/to/dlib" "SHAPE_MODEL=./shape_predictor_68_face_landmarks.dat" eyetracker.pro
DLIB_DIR = $$DLIB_DIR

# Include paths
INCLUDEPATH += $$PWD
!isEmpty(DLIB_DIR) {
    INCLUDEPATH += $$DLIB_DIR/include
    LIBS += -L$$DLIB_DIR/lib
}

# Link dlib (adjust if your dlib library name/path differs)
LIBS += -ldlib

# Link BLAS/LAPACK explicitly to satisfy dlib dependencies (OpenBLAS preferred)
# On Debian/Ubuntu systems libopenblas-dev provides libopenblas; otherwise -lblas -llapack can be used.
# Use --no-as-needed to ensure linker keeps these libs when needed.
unix {
    LIBS += -Wl,--no-as-needed -lopenblas -lblas -llapack -Wl,--as-needed
    LIBS += -lpthread
}

# If you prefer system BLAS only, comment the above and uncomment the following:
# unix: LIBS += -Wl,--no-as-needed -lblas -llapack -Wl,--as-needed -lpthread

# Path to shape predictor (optional at qmake time)
SHAPE_MODEL = $$SHAPE_MODEL
DEFINES += SHAPE_MODEL_PATH=\\\"$$SHAPE_MODEL\\\"

# Compiler flags: optimization, debug info level 3, fast-math, march native, warnings, LTO
QMAKE_CXXFLAGS += -O3 -g3 -ffast-math -march=native -Wall -Wextra -flto
QMAKE_CFLAGS   += -O3 -g3 -ffast-math -march=native -Wall -Wextra -flto
QMAKE_LFLAGS   += -flto

# Ensure position-independent code for some static libs if needed
QMAKE_CXXFLAGS += -fPIC
QMAKE_CFLAGS   += -fPIC

# Reduce symbol stripping in debug builds; keep -s for release if desired
# CONFIG(release, debug|release): QMAKE_CXXFLAGS += -s

# Add rpath to find shared libs in DLIB_DIR/lib at runtime (optional)
!isEmpty(DLIB_DIR) {
    unix: QMAKE_RPATHDIR += $$DLIB_DIR/lib
}

# Resources (uncomment if you add a .qrc)
# RESOURCES += resources.qrc

# Install rules (optional)
target.path = $$OUT_PWD
INSTALLS += target

