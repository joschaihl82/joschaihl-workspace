# Makefile für qgeister1 - C/V4L2/X11 Version (Ohne pkg-config)

TARGET = qgeister1
SOURCE = main.c

CC = gcc

# Explizite Include-Pfade für Header-Dateien:
# -I/usr/include/X11
# -I/usr/include/alsa
# -I/usr/include/freetype2 (oft für Xft benötigt, da Xft auf FreeType aufbaut)
X11_CFLAGS = -I/usr/include/X11 -I/usr/include/alsa -I/usr/include/freetype2

# Explizite Linker-Flags für Bibliotheken:
# -lX11: Xlib
# -lXext: X Extension (oft für Xft indirekt benötigt)
# -lXft: X FreeType Library
# -lXrender: X Render Extension (für Anti-Aliasing)
# -lfreetype: FreeType Engine (direkte Abhängigkeit von Xft)
# -lasound: ALSA (Audio)
# -lm: Math (für math.h Funktionen wie sin, cos, sqrt)
X11_LDFLAGS = -lX11 -lXext -lXft -lXrender -lfreetype -lasound -lm

# Compiler-Flags (Fügt die Include-Pfade hinzu)
CFLAGS = -Wall -Wextra -std=c99 -O2 $(X11_CFLAGS)

# Linker Flags (Fügt die Bibliotheken hinzu)
LDFLAGS = $(X11_LDFLAGS)

.PHONY: all clean

all: $(TARGET)

$(TARGET): $(SOURCE)
	@echo "Compiling $(SOURCE) to $(TARGET)..."
	$(CC) $(CFLAGS) $(SOURCE) -o $(TARGET) $(LDFLAGS)
	@echo "Compilation successful. Execute with: ./$(TARGET)"

clean:
	@echo "Cleaning up..."
	@rm -f $(TARGET)
