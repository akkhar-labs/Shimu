CC=gcc
CFLAGS=-Iinclude

all:
	$(CC) $(CFLAGS) src/*.c src/utils/*.c -o bin/shimu
