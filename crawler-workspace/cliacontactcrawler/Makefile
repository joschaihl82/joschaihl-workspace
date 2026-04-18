# Simple Makefile for addresscrawler_cli.c
# Usage:
#   make        # build
#   make run    # run with args: make run ARGS="https://example.com 200 3"
#   make clean  # remove binary and object files

CC      := gcc
CFLAGS  := -std=c11 -O2 -Wall -Wextra -pipe
LDLIBS  := -lcurl -lsqlite3 -lcrypto -pthread
SRC     := crawler.c
BIN     := crawler

.PHONY: all clean run

all: $(BIN)

$(BIN): $(SRC)
	$(CC) $(CFLAGS) -o $@ $^ $(LDLIBS)

run: $(BIN)
	./$(BIN) $(ARGS)

clean:
	-rm -f $(BIN) *.o

