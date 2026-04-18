# --- Variablen ---
CC = gcc
TARGET = qgeister1
SRCS = main.c

# Compiler Flags
# -Wall -Wextra: Aktiviert alle wichtigen Warnungen
# -std=c99: Erforderlich für die 'variably modified' Arrays und 'auto' Funktionen
# -O2: Optimierungslevel
CFLAGS = -Wall -Wextra -std=c99 -O2

# Include Paths
# Fügt die Header-Pfade für X11, ALSA und FreeType hinzu
INCLUDES = -I/usr/include/X11 -I/usr/include/alsa -I/usr/include/freetype2

# Libraries (basierend auf der Nutzung von X11, Xft, ALSA und Math)
LIBS = -lX11 -lXext -lXft -lXrender -lfreetype -lasound -lm


# --- Regeln ---

.PHONY: all clean

# Standardregel: Erstellt das Ziel (qgeister1)
all: $(TARGET)

$(TARGET): $(SRCS)
	@echo "Compiling $< to $@"
	# Der Kompilierungsbefehl, der alle Flags und Bibliotheken kombiniert
	$(CC) $(CFLAGS) $(INCLUDES) $< -o $@ $(LIBS)

# Regel zum Aufräumen: Entfernt die erstellte ausführbare Datei
clean:
	@echo "Removing executable and object files..."
	@rm -f $(TARGET) *.o
