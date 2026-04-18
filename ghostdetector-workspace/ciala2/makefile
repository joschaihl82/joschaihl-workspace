# Variablen definieren
TARGET = radionullon
SRC = main.c 
# Alternativ: SRC = main.c

# Compiler und Flags
CC = gcc
CFLAGS = -Wall -Wextra -std=c99
LDFLAGS = -lm

# GTK3, ALSA und V4L2 Flags abrufen (mit pkg-config)
GTK_FLAGS = $(shell pkg-config --cflags --libs gtk+-3.0)
ALSA_FLAGS = $(shell pkg-config --cflags --libs alsa)

# Verknüpfungsbefehl (LIBS)
LIBS = $(GTK_FLAGS) $(ALSA_FLAGS) $(LDFLAGS)

# --- Standard-Targets ---

# Standardziel: Kompiliert das Programm
$(TARGET): $(SRC)
	@echo "--- Kompiliere $(TARGET) mit GTK3, ALSA und V4L2 ---"
	$(CC) $(CFLAGS) $(SRC) -o $(TARGET) $(LIBS)

# Führt das Programm aus
run: $(TARGET)
	@echo "--- Starte $(TARGET) ---"
	./$(TARGET)

# Entfernt kompilierte Dateien
clean:
	@echo "--- Lösche kompilierte Dateien ---"
	rm -f $(TARGET)
	rm -f *.o

.PHONY: all clean run
