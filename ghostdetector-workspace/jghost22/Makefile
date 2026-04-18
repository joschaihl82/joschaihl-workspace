# Makefile für den C Ghost Filter mit X11 und OpenCV C-API
# Achtung: Dieser Makefile ist nicht so flexibel wie die Pkg-Config-Version
# und erfordert die typische Installation von OpenCV und Xlib.

CC := gcc
# -I/usr/include/ ermöglicht dem Compiler, Header wie <opencv2/core/core_c.h> zu finden
CFLAGS := -g3 -O2 -Wall -Wextra -std=gnu11 -g

# Bibliotheken: Xlib, Math (-lm) und alle benötigten OpenCV C-API Bibliotheken
# Die Reihenfolge der OpenCV-Bibliotheken ist hier wichtig.
LDFLAGS := -lX11 -lm -g3

# Name der Quell- und Binärdatei
SRC := xghost.c
BIN := ghost_filter

.PHONY: all clean run

all: $(BIN)

# Die Kompilierungsregel
$(BIN): $(SRC)
	$(CC) $(CFLAGS) -o $@ $< $(LDFLAGS)

run: $(BIN)
	./$(BIN)

clean:
	rm -f $(BIN) *.o

