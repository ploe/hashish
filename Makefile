compile = cc -g -c -O2 -I./murmur3 ./murmur3/murmur3.o

all:
	$(compile) hashish.c -o hashish.o
