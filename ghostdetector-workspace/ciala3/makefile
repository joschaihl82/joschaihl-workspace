# Makefile für qgeister1 C/SDL2-Port

# Name der ausführbaren Datei
TARGET = geister

# Quellcode
SRC = main.c 

# Flags für den C-Compiler
CFLAGS = -Wall -Wextra -std=c99 

# Flags für SDL2, SDL2_ttf und ALSA
PKG_CONFIG_PACKAGES = sdl2 SDL2_ttf alsa

# Automatische Erkennung der Compiler- und Linker-Flags
CFLAGS += $(shell pkg-config --cflags $(PKG_CONFIG_PACKAGES))
LDFLAGS = $(shell pkg-config --libs $(PKG_CONFIG_PACKAGES)) -lm

# Regel zum Bauen
$(TARGET): $(SRC)
	$(CC) $(CFLAGS) $< -o $@ $(LDFLAGS)

# Regel zum Aufräumen
clean:
	rm -f $(TARGET)
	rm -f berlin6_log.csv
