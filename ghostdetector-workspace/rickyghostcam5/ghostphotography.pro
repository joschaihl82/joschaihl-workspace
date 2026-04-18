# ----------------------------------------------------
# Project file for the Single-Image Reflection Removal App
# ----------------------------------------------------

# --- Application Configuration ---
# Your project target name seems to be ghostphotography based on the output
TARGET = ghostphotography
TEMPLATE = app

# Ensure you are compiling the correct file!
SOURCES += main.cpp

# --- Qt Modules ---
QT += core gui widgets

# --- C++ Standards ---
CONFIG += c++11

# --- OpenCV Configuration (Recommended via pkg-config) ---
# Use 'pkg-config' to find the paths for headers (INCLUDEPATH) and libraries (LIBS).
# This is generally more reliable than hardcoded paths.

CONFIG += link_pkgconfig

# IMPORTANT: Choose the correct package name for your system.
# Try 'opencv4' first, as it's common for modern installs.
PKGCONFIG += opencv4

# If the above fails, uncomment the lines below and comment out the lines above:
# PKGCONFIG += opencv

# ----------------------------------------------------
# Alternative: Manual Path Configuration (Use ONLY if pkg-config fails)
# ----------------------------------------------------
# INCLUDEPATH += /usr/local/include/opencv4 \
#                /usr/local/include

# LIBS += -L/usr/local/lib \
#         -lopencv_core \
#         -lopencv_imgproc \
#         -lopencv_imgcodecs \
#         -lopencv_highgui \
#         -lopencv_videoio
