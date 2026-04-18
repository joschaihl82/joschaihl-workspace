# Makefile für qgeister1 - C/V4L2/X11 Version (Ohne XShm)

TARGET = qgeister1
SOURCE = main.c

CC = gcc

# X11-Flags (Verwendet pkg-config für X11)
X11_CFLAGS := $(shell pkg-config --cflags x11)
X11_LDFLAGS := $(shell pkg-config --libs x11)

# Compiler-Flags
CFLAGS = -Wall -Wextra -std=c99 -O2 $(X11_CFLAGS)

# Linker Flags (X11 und Math)
LDFLAGS = $(X11_LDFLAGS) -lm -lasound

.PHONY: all clean

all: $(TARGET)

$(TARGET): $(SOURCE)
	@echo "Compiling $(SOURCE) to $(TARGET)..."
	$(CC) $(CFLAGS) $(SOURCE) -o $(TARGET) $(LDFLAGS)
	@echo "Compilation successful. Execute with: ./$(TARGET)"

clean:
	@echo "Cleaning up..."
	@rm -f $(TARGET)
