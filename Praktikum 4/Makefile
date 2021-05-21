CC = gcc
CFLAGS = -Wall -std=c11 -I$(INCLUDE)
CFLAGS2 = -Wall -std=c11 -I$(INCLUDE) -c -g
INCLUDE = ./include

all: myShell

myShell: myShell.o 
	$(CC) $(CFLAGS) -o $@ $^
	
myShell.o: myShell.c
	$(CC) $(CFLAGS2) -o $@ $^

run: myShell
	./myShell

clean:
	rm -rf myShell
	rm -rf myShell.o