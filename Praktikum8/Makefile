CC = gcc
CFLAGS = -std=c11 -I$(INCLUDE)
CFLAGS2 = -std=c11 -I$(INCLUDE) -c 
INCLUDE = ./include

all: dateisysteme

dateisysteme: dateisysteme.o
	$(CC) $(CFLAGS) -o $@ $^ $(LIBS)
	
dateisysteme.o: dateisysteme.c
	$(CC) $(CFLAGS2) -o $@ $^ $(LIBS)

run: dateisysteme
	./dateisysteme

clean:
	rm -rf dateisysteme
	rm -rf dateisysteme.o