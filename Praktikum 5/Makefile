CC = gcc
CFLAGS = -I$(INCLUDE)
CFLAGS2 = -I$(INCLUDE) -c 
INCLUDE = ./include
LIBS = -lpthread -D_GNU_SOURCE

all: scheduling

scheduling: scheduling.o global.o lists.o
	$(CC) $(CFLAGS) -o $@ $^ $(LIBS)
	
scheduling.o: scheduling.c
	$(CC) $(CFLAGS2) -o $@ $^ $(LIBS)

global.o: global.c
	$(CC) $(CFLAGS2) -o $@ $^ $(LIBS)
	
lists.o: lists.c
	$(CC) $(CFLAGS2) -o $@ $^ $(LIBS)

run: scheduling
	./scheduling

install:

clean:
	rm -rf scheduling
	rm -rf scheduling.o
	rm -rf global.o
	rm -rf lists.o