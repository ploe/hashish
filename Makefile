all:
	cc -c -O2 -I./murmur3 hashish.c
	mv hashish.o hashish-unlinked.o
	ld -r -o hashish.o hashish-unlinked.o ./murmur3/murmur3.o

gdb-enabled:
	cc -g -c -O2 -I./murmur3 hashish.c
	mv hashish.o hashish-unlinked.o
	ld -r -o hashish.o hashish-unlinked.o ./murmur3/murmur3.o

dev: gdb-enabled example

example: 
	cc -g -O2 -I./murmur3  hashish.o main.c -o main
