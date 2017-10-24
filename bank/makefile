CC=gcc -lpthread
CFLAGS=-Wall 

all: appserver
appserver: appserver.o utils.o Bank.o bank_functions.o queue.o
appserver.o: appserver.c appserver.h
queue.o: queue.c queue.h
bank_functions.o: bank_functions.c bank_functions.h 
Bank.o: Bank.c Bank.h
utils.o: utils.c utils.h

clean:
	-rm -f appserver appserver.o utils.o Bank.o bank_functions.o queue.o
run: project2
	./appserver

