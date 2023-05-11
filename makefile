CC = gcc
CFLAGS = -Wall -g -m32

all: myshell mypipe looper

myshell: LineParser.o myshell.o 
	$(CC) $(CFLAGS) -o myshell myshell.o LineParser.o

looper: looper.o
	$(CC) $(CFLAGS) -o looper looper.o

mypipe: mypipe.o
	$(CC) $(CFLAGS) -o mypipe mypipe.o
	
LineParser.o: LineParser.c LineParser.h
	$(CC) $(CFLAGS) -c LineParser.c -o LineParser.o

myshell.o: myshell.c 
	$(CC) $(CFLAGS) -c myshell.c -o myshell.o

looper.o: looper.c
	$(CC) $(CFLAGS) -c looper.c -o looper.o

mypipe.o: mypipe.c
	$(CC) $(CFLAGS) -c mypipe.c -o mypipe.o

.PHONY: clean
	
clean:
	rm -f mypipe myshell looper *.o