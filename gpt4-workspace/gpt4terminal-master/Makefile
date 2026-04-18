INCLUDE_DIR = include
SRC_DIR = src
OBJ_DIR = bin
APP_NAME = gpt4terminal

CFLAGS = -Wall -Wpedantic
LIBS = -lcurl -lcjson
CC = gcc
SRCS = $(wildcard $(SRC_DIR)/*.c)
OBJS = $(patsubst $(SRC_DIR)/%.c, $(OBJ_DIR)/%.o, $(SRCS))
TARGET = main

all: $(OBJS) 
	$(CC) $(OBJS) $(CFLAGS) -o $(OBJ_DIR)/$(TARGET) $(LIBS)

$(OBJ_DIR)/%.o: $(SRC_DIR)/%.c  $(OBJ_DIR) 
	$(CC) -c $(SRC_DIR)/$*.c $(CFLAGS) -o $@ -I$(INCLUDE_DIR)

$(OBJ_DIR):
	mkdir $(OBJ_DIR)

clean:
	rm -fr $(OBJ_DIR) 

install: all
	cp $(OBJ_DIR)/main /usr/bin/$(APP_NAME)

uninstall:
	rm /usr/bin/$(APP_NAME)
    
