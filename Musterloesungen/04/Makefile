CC = gcc
CFLAGS = -Wall -std=c11 -I$(INCLUDE)
CFLAGS2 = -Wall -std=c11 -I$(INCLUDE) -c -g
INCLUDE = ./include

all: miniMinden


miniMinden: miniMinden.o 
	$(CC) $(CFLAGS) -o $@ $^
	
miniMinden.o: miniMinden.c
	$(CC) $(CFLAGS2) -o $@ $^

	

run: miniMinden
	./miniMinden

install:
	

clean:
	rm -rf miniMinden
	rm -rf miniMinden.o
