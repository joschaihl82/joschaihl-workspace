# Makefile für die Konsolen-Dosimeter-Anwendung
# Quellcode: dosimeter_console_alsa_double.c

# =======================================================================
# Variablen
# =======================================================================
TARGET    = dosi
SOURCE    = $(TARGET).c

# Compiler (GCC)
CC        = gcc

# Standard-Compiler-Flags
# -std=c99: Verwende den C99-Standard
# -Wall: Aktiviere alle Warnungen
CFLAGS    = -std=c99 -Wall -Wextra -O3 -march=native

# Bibliotheken (Libraries)
# -lpthread: POSIX-Threads für die Alarm- und Update-Logik
# -lasound:  ALSA-Bibliothek für Audio-I/O
# -lm:       Mathematische Funktionen (z.B. sin, log, exp)
LDLIBS    = -lpthread -lasound -lm -lX11 -lXext

# =======================================================================
# Targets
# =======================================================================

# Standard-Target: Kompiliert das Programm
.PHONY: all
all: $(TARGET)

$(TARGET): $(SOURCE)
	@echo "--- Kompiliere $(TARGET) ---"
	$(CC) $(CFLAGS) $< -o $@ $(LDLIBS)
	@echo "Kompilierung erfolgreich! Ausführbare Datei: $(TARGET)"

# Clean-Target: Entfernt die kompilierte Binärdatei und temporäre Dateien
.PHONY: clean
clean:
	@echo "--- Bereinige Projektdateien ---"
	rm -f $(TARGET)
