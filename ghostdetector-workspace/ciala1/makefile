# --- Konfiguration ---
TARGET = qgeister
SRC = main.c

# SDL2-Konfiguration (wird benötigt)
SDL_CFLAGS := $(shell sdl2-config --cflags)
SDL_LIBS := $(shell sdl2-config --libs)

# Zusätzliche Compiler-Flags und Linker-Bibliotheken
CFLAGS = -Wall -Wextra -std=c99 -pedantic -O2
LDFLAGS = -lm -lpthread -lrt

# --- Befehle ---

$(TARGET): $(SRC)
	@echo "--- Kompiliere $(TARGET) ---"
	gcc $(CFLAGS) $(SRC) -o $(TARGET) $(SDL_CFLAGS) $(LDFLAGS) $(SDL_LIBS)

.PHONY: clean run

run: $(TARGET)
	@echo "--- Starte $(TARGET) ---"
	./$(TARGET)

clean:
	@echo "--- Lösche Build-Dateien ---"
	rm -f $(TARGET)
