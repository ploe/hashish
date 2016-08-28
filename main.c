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
	ish_MapSet(map, "1", "woohoo");
	ish_MapSet(map, "2", "woohoo");
	ish_MapSet(map, "3", "woohoo");
	ish_MapSet(map, "4", "woohoo");
	ish_MapSet(map, "5", "woohoo");
	ish_MapSet(map, "6", "woohoo");
	ish_MapSet(map, "7", "woohoo");
	ish_MapSet(map, "8", "woohoo");
	ish_MapSet(map, "9", "woohoo");
	ish_MapSet(map, "10", "woohoo");
	ish_MapSet(map, "11", "woohoo");
	ish_MapSet(map, "12", "woohoo");
	ish_MapSet(map, "14", "woohoo");
	ish_MapSet(map, "15", "woohoo");
	ish_MapSet(map, "16", "woohoo");
	ish_MapSet(map, "17", "woohoo");

	ish_MapDelete(map, "7");
	ish_MapDelete(map, "8");
	ish_MapDelete(map, "9");
	ish_MapForPairs(map, printkv);
	return 0;
}
