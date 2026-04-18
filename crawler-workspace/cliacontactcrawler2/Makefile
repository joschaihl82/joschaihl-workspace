# Makefile for addresscrawler / crawler
CC = gcc
CFLAGS = -std=c11 -O2 -Wall -Wextra -I/usr/include/libxml2
LDFLAGS = -lcurl -lsqlite3 -lcrypto -lxml2 -pthread
SRC = crawler.c
BIN = addresscrawler_cli

.PHONY: all clean

all: $(BIN)

$(BIN): $(SRC)
	$(CC) $(CFLAGS) -o $@ $< $(LDFLAGS)

clean:
	rm -f $(BIN) *.o

