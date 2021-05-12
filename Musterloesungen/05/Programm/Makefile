CC = gcc
CFLAGS = -Wall -std=c11 -I$(INCLUDE)
CFLAGS2 = -Wall -std=c11 -I$(INCLUDE) -c 
INCLUDE = ./include
LIBS = -lpthread -D_GNU_SOURCE

all: scheduler


scheduler: scheduler.o global.o lists.o
	$(CC) $(CFLAGS) -o $@ $^ $(LIBS)
	
scheduler.o: scheduler.c
	$(CC) $(CFLAGS2) -o $@ $^ $(LIBS)

global.o: global.c
	$(CC) $(CFLAGS2) -o $@ $^ $(LIBS)
	
lists.o: lists.c
	$(CC) $(CFLAGS2) -o $@ $^ $(LIBS)

run: scheduler
	./scheduler

install:
	

clean:
	rm -rf scheduler
	rm -rf scheduler.o
	rm -rf global.o
	rm -rf lists.o