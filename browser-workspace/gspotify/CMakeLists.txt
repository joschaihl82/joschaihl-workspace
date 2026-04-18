cmake_minimum_required(VERSION 3.16)
project(MiniGTK4Browser C)

set(CMAKE_C_STANDARD 11)
set(CMAKE_C_STANDARD_REQUIRED ON)

if(NOT CMAKE_BUILD_TYPE)
  set(CMAKE_BUILD_TYPE Release CACHE STRING "Build type" FORCE)
endif()

# -------------------------
# Feste Include-Pfade (anpassen falls nötig)
# -------------------------
# Beispielwerte (häufige Systempfade). Ersetze sie durch die Pfade auf deinem System.
set(FIXED_GTK4_INCLUDE "/usr/include/gtk-4.0")
set(FIXED_WEBKIT_INCLUDE "/usr/include/webkit2gtk-6.0")

# Optional: feste Library-Pfade (falls Bibliotheken in Nicht-Standardpfaden liegen)
# set(FIXED_LIB_DIR "/usr/lib")
# link_directories(${FIXED_LIB_DIR})

# -------------------------
# Minimaler Check: existieren die Header?
# -------------------------
if(NOT EXISTS "${FIXED_GTK4_INCLUDE}/gtk/gtk.h")
  message(FATAL_ERROR "GTK4 header not found at ${FIXED_GTK4_INCLUDE}. Passe FIXED_GTK4_INCLUDE an.")
endif()
if(NOT EXISTS "${FIXED_WEBKIT_INCLUDE}/webkit2/webkit2.h")
  message(FATAL_ERROR "WebKitGTK header not found at ${FIXED_WEBKIT_INCLUDE}. Passe FIXED_WEBKIT_INCLUDE an.")
endif()

# -------------------------
# Quellen und Ziel
# -------------------------
add_executable(browser browser.c)

# Feste Include-Pfade verwenden
target_include_directories(browser PRIVATE
  ${FIXED_GTK4_INCLUDE}
  ${FIXED_WEBKIT_INCLUDE}
)

# Link-Bibliotheken (Namen, nicht Pfade). Passe ggf. an die auf deinem System vorhandenen Namen an.
# Übliche Namen: webkit2gtk-6.0 oder webkit2gtk-4.0; gtk-4; und transitive libs glib-2.0, gobject-2.0, gio-2.0
target_link_libraries(browser PRIVATE
  webkit2gtk-6.0
  gtk-4
  glib-2.0
  gobject-2.0
  gio-2.0
)

# Compiler-Optionen
if (CMAKE_C_COMPILER_ID STREQUAL "GNU" OR CMAKE_C_COMPILER_ID MATCHES "Clang")
  target_compile_options(browser PRIVATE -Wall -Wextra -Wpedantic -pthread -D_REENTRANT)
else()
  target_compile_options(browser PRIVATE -D_REENTRANT)
endif()

install(TARGETS browser RUNTIME DESTINATION bin)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)
