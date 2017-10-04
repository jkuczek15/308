CC=gcc 
CFLAGS=-Wall

all: shell
shell: shell.o shell_builtins.o utils.o shell_functions.o
utils.o: utils.c utils.h
shell_functions.o: shell_functions.c shell_functions.h
shell_builtins.o: shell_builtins.c shell_builtins.h
shell.o: shell.c shell.h

clean:
	-rm -f shell.o shell_builtins.o shell_functions.o utils.o shell
run: project1
	./shell

