# Makefile for qgeister (single-file main.c)
# Builds executable "qgeister" using libxcb, XSHM, FreeType, Fontconfig and ALSA.
# Adjust CC/LD_FLAGS as needed for your system.

CC := gcc
SRC := main.c
BIN := qgeister

# Compiler flags
CFLAGS := -std=gnu11 -O2 -Wall -Wextra -fPIC -I/usr/include/freetype2
# For faster debug builds set -g and lower optimization:
# CFLAGS := -std=gnu11 -g -O0 -Wall -Wextra

# Libraries (linker order matters)
LIBS := -lxcb -lxcb-shm -lxcb-keysyms -lfreetype -lfontconfig -lasound -lm

# If your system supplies pkg-config entries for xcb and friends, you can
# uncomment the next line and remove the manual LIBS above.
# PKG_LIBS := $(shell pkg-config --cflags --libs xcb xcb-shm xcb-keysyms freetype2 fontconfig alsa)
# and then use: LDLIBS := $(PKG_LIBS)

LDFLAGS := $(LIBS)

# Installation prefix (optional)
PREFIX ?= /usr/local
BINDIR ?= $(PREFIX)/bin

# Targets
.PHONY: all clean install uninstall run

all: $(BIN)

$(BIN): $(SRC)
	@echo "Compiling $@ from $<"
	$(CC) $(CFLAGS) -o $@ $< $(LDFLAGS)

run: $(BIN)
	./$(BIN)

install: $(BIN)
	install -d $(BINDIR)
	install -m 0755 $(BIN) $(BINDIR)/$(BIN)
	@echo "Installed $(BIN) to $(BINDIR)/$(BIN)"

uninstall:
	-rm -f $(BINDIR)/$(BIN)
	@echo "Removed $(BINDIR)/$(BIN) if present"

clean:
	-rm -f $(BIN)
	@echo "Cleaned build artifacts"

# Optional: show compiler and linker flags for debugging
print-%:
	@echo '$*=$($*)'

