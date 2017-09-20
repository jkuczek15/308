CC=gcc 
CFLAGS=-Wall

all: shell
lab1: shell.o
lab1.o: shell.c

clean:
	-rm -f shell.o shell
run: project1
	./shell

