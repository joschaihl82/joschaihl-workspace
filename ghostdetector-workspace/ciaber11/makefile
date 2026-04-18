# Makefile für qgeister1 - C/V4L2/SDL2 Version

# Quell- und Zielnamen
TARGET = qgeister1
SOURCE = main.c

# C Compiler
CC = gcc

# Compiler-Flags
CFLAGS = -Wall -Wextra -std=c99 -O2

# SDL2-Flags (Verwendet pkg-config für die automatische Erkennung der Flags)
# Fallback, falls pkg-config fehlschlägt, ist -lSDL2 -lm
SDL_CFLAGS := $(shell pkg-config --cflags sdl2 2>/dev/null)
SDL_LDFLAGS := $(shell pkg-config --libs sdl2 2>/dev/null) -lm

# Fallback für die LDFLAGS, wenn pkg-config nicht funktioniert oder SDL2 nicht findet
# Beinhaltet die SDL2-Bibliothek und die Math-Bibliothek (-lm)
ifeq ($(strip $(SDL_LDFLAGS)),)
    SDL_LDFLAGS = -lSDL2 -lm
    # Wenn Sie eine Fehlermeldung erhalten, müssen Sie möglicherweise den Pfad zu SDL2 hier hinzufügen:
    # CFLAGS += -I/usr/include/SDL2
    # LDFLAGS += -L/usr/lib
endif

# Linker Flags (Beinhaltet V4L2 und SDL2)
LDFLAGS = $(SDL_LDFLAGS)

.PHONY: all clean

all: $(TARGET)

$(TARGET): $(SOURCE)
	@echo "Compiling $(SOURCE) to $(TARGET)..."
	$(CC) $(CFLAGS) $(SOURCE) -o $(TARGET) $(LDFLAGS) $(SDL_CFLAGS)
	@echo "Compilation successful. Execute with: ./$(TARGET)"

clean:
	@echo "Cleaning up..."
	@rm -f $(TARGET)
