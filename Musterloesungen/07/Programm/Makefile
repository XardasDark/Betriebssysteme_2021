CC = gcc
CFLAGS = -std=c11 -I$(INCLUDE)
CFLAGS2 = -std=c11 -I$(INCLUDE) -c 
INCLUDE = ./include
LIBS = 

all: main


main: main.o methods.o
	$(CC) $(CFLAGS) -o $@ $^ $(LIBS)
	
main.o: main.c
	$(CC) $(CFLAGS2) -o $@ $^ $(LIBS)

methods.o: methods.c
	$(CC) $(CFLAGS2) -o $@ $^ $(LIBS)

run: main
	./main

clean:
	rm -rf main
	rm -rf main.o
	rm -rf methods.o