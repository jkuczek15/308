CC=gcc 
CFLAGS=-Wall

all: appserver
appserver: appserver.o utils.o
appserver.o: appserver.c appserver.h 
utils.o: utils.h utils.c

clean:
	-rm -f appserver appserver.o utils.o
run: project2
	./appserver

