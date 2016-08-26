compile = cc -g -c -O2 -I./murmur3

all: library example

library:
	$(compile) hashish.c
	mv hashish.o hashish-unlinked.o
	ld -r -o hashish.o hashish-unlinked.o ./murmur3/murmur3.o

example: 
	cc -g -O2 -I./murmur3  hashish.o main.c -o main
