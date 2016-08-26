#include <stdio.h>
#include "hashish.h"

int printkv(char *key, void *value, void *probe) {
	printf("'%s'=>'%s'\n", key, (char *)value);
	return 0;
}

int main(int argc, char *argv) {
	ish_Map *map = ish_MapNew();
	ish_MapSet(map, "hello world", "nah not really");
	ish_MapSet(map, "myke", "Myke");
	ish_MapSet(map, "is", "waaaagh");
	ish_MapSet(map, "great", "woohoo");
	ish_MapForPairs(map, printkv);
	return 0;
}
