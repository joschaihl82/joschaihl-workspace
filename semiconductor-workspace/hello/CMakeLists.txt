cmake_minimum_required(VERSION 3.16)

# Cross-Compiler Einstellungen
set(CMAKE_SYSTEM_NAME Windows)
set(TOOLCHAIN_PREFIX x86_64-w64-mingw32)

set(CMAKE_C_COMPILER ${TOOLCHAIN_PREFIX}-gcc)
set(CMAKE_RC_COMPILER ${TOOLCHAIN_PREFIX}-windres)

project(HelloWin32 C)

# Header-Pfad für windows.h explizit angeben
include_directories("/usr/${TOOLCHAIN_PREFIX}/include")

# Statisches Linken erzwingen
set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -static-libgcc -static")
set(CMAKE_EXE_LINKER_FLAGS "${CMAKE_EXE_LINKER_FLAGS} -Wl,--subsystem,windows -static")

add_executable(hello WIN32 hello.c)

# User32.lib wird für MessageBox benötigt
target_link_libraries(hello PRIVATE user32)