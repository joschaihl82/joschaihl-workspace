# Name der ausführbaren Datei
TARGET = radionullon

# Quelldateien
SRCS = main.c

# Compiler (Standardmäßig GCC)
CC = gcc

# Compiler-Flags
CFLAGS = -Wall -O2 -std=c99 -lm

# Paketkonfigurationen für die Bibliotheken
# SDL2 ENTFERNT, ALSA HINZUGEFÜGT
PKG_CONFIG_LIBS = gtk+-3.0 gdk-pixbuf-2.0 libv4l2 alsa

# Abrufen der notwendigen Compiler- und Linker-Flags
GTK_CFLAGS = $(shell pkg-config --cflags $(PKG_CONFIG_LIBS))
GTK_LIBS = $(shell pkg-config --libs $(PKG_CONFIG_LIBS))

# Verknüpfungsflags
LDFLAGS = $(GTK_LIBS)

# --- Hauptziel ---
$(TARGET): $(SRCS)
	$(CC) $(SRCS) $(CFLAGS) $(GTK_CFLAGS) $(LDFLAGS) -o $(TARGET)

# --- Hilfsziele ---
clean:
	rm -f $(TARGET)

run: $(TARGET)
	./$(TARGET)

.PHONY: all clean run
