#include <stdio.h>
#include "hashish.h"

int printkv(char *key, void *value, void *probe) {
	printf("'%s'=>'%s'\n", key, (char *)value);
	return 0;
}

int main(int argc, char *argv[]) {
	ish_Map *map = ish_MapNew();
	ish_MapSet(map, "hello world", "nah not really");
	ish_MapSet(map, "myke", "Myke");
	ish_MapSet(map, "is", "waaaagh");
	ish_MapSet(map, "1", "john");
	ish_MapSet(map, "2", "paul");
	ish_MapSet(map, "3", "george");
	ish_MapSet(map, "4", "ringo");
	ish_MapSet(map, "5", "taxman");
	ish_MapSet(map, "6", "strawberry fields");
	ish_MapSet(map, "7", "eleanor rigby");
	ish_MapSet(map, "8", "octopus's garden");
	ish_MapSet(map, "9", "penny lane");
	ish_MapSet(map, "10", "yellow submarine");
	ish_MapSet(map, "11", "revolver");
	ish_MapSet(map, "12", "sergeant pepper's");
	ish_MapSet(map, "14", "magical mystery tour");
	ish_MapSet(map, "15", "the beatles (white album)");
	ish_MapSet(map, "16", "hard day's night");
	ish_MapSet(map, "17", "help");

	ish_MapDelete(map, "7");
	ish_MapDelete(map, "8");
	ish_MapDelete(map, "9");
	
	printf("what in 7? \"%s\"\n", (char *) ish_MapGet(map, "7"));
	printf("what's in 17? \"%s\"\n", (char *) ish_MapGet(map, "17"));
	printf("what's in 12? \"%s\"\n", (char *) ish_MapGet(map, "12"));

	ish_MapForPairs(map, printkv);
	return 0;
}
