# src

CC=gcc
CFLAGS=-Wall -lncurses
PREFIX=/usr/local/bin/
RM=rm -rf

all: znake

znake: main.o soviet.o
	$(CC) $(CFLAGS) main.o soviet.o -o znake
main.o: main.c
	$(CC) $(CFLAGS) -c main.c
soviet.o: soviet.c
	$(CC) $(CFLAGS) -c soviet.c

clean:
	$(RM) main.o soviet.o
	$(RM) znake

install:
	cp -f znake $(PREFIX)znake

uninstall:
	rm -f $(PREFIX)znake

