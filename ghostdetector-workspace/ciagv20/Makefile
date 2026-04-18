# Makefile for v4l2_ghost_cia_dynamic (with Xft)
CC = gcc
CFLAGS = -O2 -std=gnu11 -Wall -Wextra -march=native
# Fallback if pkg-config not available / packages not found
PKG_CFLAGS = -I/usr/include -I/usr/include/X11 -I/usr/include/X11/Xft -I/usr/include/freetype2
PKG_LIBS = -lXft -lX11

LDLIBS = $(PKG_LIBS) -lm -lpthread

SRC = xghost.c
BIN = xghost

.PHONY: all clean run deps

all: deps $(BIN)

$(BIN): $(SRC)
	$(CC) $(CFLAGS) $(PKG_CFLAGS) -o $@ $^ $(LDLIBS)

clean:
	-rm -f $(BIN) *.o

run: $(BIN)
	./$(BIN)

# Convenience target to hint how to install dependencies on Debian/Ubuntu
deps:
	@echo "Ensure X11 and Xft dev packages are installed (example for Debian/Ubuntu):"
	@echo "  sudo apt update && sudo apt install build-essential libx11-dev libxft-dev pkg-config"

