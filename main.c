#include <stdio.h>
#include "hashish.h"

int printkv(char *key, void *value, void *probe) {
	printf("'%s'=>'%s'\n", key, (char *)value);
	return 0;
}

void *saybye(ish_Map *map, char *key, void *value) {
	printf("%s says goodbye\n", (char *) value);
	puts(key);
	return NULL;
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

	ish_MapRemove(map, "7");
	ish_MapRemove(map, "8");
	ish_MapRemove(map, "9");

	ish_MapOnRemove(map, "14", saybye);
	
	printf("what in 7? \"%s\"\n", (char *) ish_MapGet(map, "7"));
	printf("what's in 17? \"%s\"\n", (char *) ish_MapGet(map, "17"));
	printf("what's in 12? \"%s\"\n", (char *) ish_MapGet(map, "12"));

	ish_MapForPairs(map, printkv);
	
	int i;
	for (i = 1; i <= 5; i++) {
		map = ish_MapGrow(map);
		printf("\nMap grown to %04X...\n", (unsigned int) map->mask);
		ish_MapForPairs(map, printkv);
	}

	for (i = 1; i <= 5; i++) {
		map = ish_MapShrink(map);
		printf("\nMap shrunk to %04X...\n", (unsigned int) map->mask);
		ish_MapForPairs(map, printkv);
	}

	ish_MapFree(map);
	return 0;
}
