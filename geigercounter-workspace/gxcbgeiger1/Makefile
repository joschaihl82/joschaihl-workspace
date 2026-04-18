# Makefile for geiger (pure xcb version)
CC ?= clang 
SRC := geiger.c
BIN := geiger

CFLAGS ?= -std=c99 -O3 -march=native -Wall -Wextra -Wno-unused-result -pthread
# If you need debugging, override CFLAGS when calling make:
# make CFLAGS="-std=c99 -g -O0 -Wall -Wextra -pthread"

# Libraries commonly needed for this project
LIBS   ?= -lxcb -lxcb-image -lasound -ljpeg -lm -lxcb-keysyms
LDFLAGS ?=

# Additional include dirs or libs can be provided via environment:
#   make EXTRA_CFLAGS="-I/opt/local/include" EXTRA_LIBS="-L/opt/local/lib -lcustom"
EXTRA_CFLAGS ?= -I/usr/include/freetype2
EXTRA_LIBS   ?=

CFLAGS += $(EXTRA_CFLAGS)
LIBS   += $(EXTRA_LIBS)

.PHONY: all clean run

all: $(BIN)

$(BIN): $(SRC)
	@echo "Kompiliere $< -> $@"
	$(CC) $(CFLAGS) $< -o $@ $(LDFLAGS) $(LIBS)

run: $(BIN)
	./$(BIN)

clean:
	-rm -f $(BIN) *.o


