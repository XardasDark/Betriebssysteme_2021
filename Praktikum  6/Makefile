CC = gcc
CFLAGS = -std=c11 -I$(INCLUDE)
CFLAGS2 = -std=c11 -I$(INCLUDE) -c 
INCLUDE = ./include
LIBS = -lpthread -D_GNU_SOURCE

all: main

main: main.o miniz.o
	$(CC) $(CFLAGS) -o $@ $^ $(LIBS)
	
main.o: main.c
	$(CC) $(CFLAGS2) -o $@ $^ $(LIBS)

miniz.o: miniz.c
	$(CC) $(CFLAGS2) -o $@ $^ $(LIBS)

	
run: main
	./main ./documents

clean:
	rm -rf main
	rm -rf main.o
	rm -rf miniz.o