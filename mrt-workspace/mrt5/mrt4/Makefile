# Makefile for radionullon_viewer (SDL2 + V4L2)
# Usage:
#   make            # build
#   make run        # run ./radionullon_viewer
#   make clean      # remove build artifacts

APP     := radionullon
SRC     := radionullon.c
OBJ     := $(SRC:.c=.o)

# Default compiler and flags
CC      := gcc
CSTD    := -std=c11
CFLAGS  := -O2 -Wall -Wextra -pedantic $(CSTD)
LDFLAGS := -lm

# Detect platform for SDL2 linkage
UNAME_S := $(shell uname -s)

# SDL2 flags via pkg-config if available
SDL2_CFLAGS := $(shell pkg-config --cflags sdl2 2>/dev/null)
SDL2_LIBS   := $(shell pkg-config --libs sdl2 2>/dev/null)

# Fallback if pkg-config not present
ifeq ($(SDL2_CFLAGS),)
	# Try sdl2-config as fallback
	SDL2_CFLAGS := $(shell sdl2-config --cflags 2>/dev/null)
	SDL2_LIBS   := $(shell sdl2-config --libs 2>/dev/null)
endif

# Final flags
CFLAGS += $(SDL2_CFLAGS)
LDFLAGS += $(SDL2_LIBS)

# Linux needs V4L2
ifeq ($(UNAME_S),Linux)
	CFLAGS  += -D_GNU_SOURCE
	LDFLAGS +=
endif

# macOS note: V4L2 is not available; this target is Linux-oriented.
# On macOS, consider using AVFoundation instead (not covered here).

.PHONY: all run clean

all: $(APP)

$(APP): $(OBJ)
	$(CC) $(OBJ) -o $@ $(LDFLAGS)

%.o: %.c
	$(CC) $(CFLAGS) -c $< -o $@

clean:
	$(RM) $(OBJ) $(APP)

