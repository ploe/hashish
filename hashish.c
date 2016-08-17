#include "hashish.h"

/*	KVPair methods	*/

/*	SetHash.
	Uses Murmur3 to convert a [key] in to a new [hash]	*/

static void SetHash(char *key, void *out) {
	MurmurHash3_x64_128(key, strlen(key), getpid(), out);
}

/*	FindPair:
	Finds the KVPair with [key] in [map]	*/
	

static ish_KVPair *FindPair(ish_Map *map, char *key) {
	uint8_t hash[ish_UINT128_LENGTH];
	SetHash(key, hash);

	ish_KVPair *top = map->buckets[hash[0]], *pair;
	for (pair = top; pair != NULL; pair = pair->next) {
		if (memcmp(hash, pair->hash, sizeof(uint8_t) * ish_UINT128_LENGTH) == 0)
			if (strcmp(key, pair->key) == 0) return pair;
	}
	return NULL;
}

/*	NewKVPair:
	Creates a new KVPair at [key] on the [map]	*/

static ish_KVPair *NewKVPair(ish_Map *map, char *key) {
	ish_KVPair *pair = calloc(1, sizeof(ish_KVPair));
	if (pair) {
		SetHash(key, pair->hash);

		pair->key = calloc(strlen(key) + 1, sizeof(char));
		if (!pair->key) {
			free(pair);
			return NULL;
		}
		strcpy(pair->key, key);


		ish_KVPair *top = map->buckets[pair->hash[0]];
		if (top) top->prev = pair;
		pair->next = top;

		map->buckets[pair->hash[0]] = pair;
	}

	return pair;
}

/*	ish_Map methods.	*/

/*	ish_MapSetWithDestruct (public):
	In [map] we set KVPair [key] to have [value] as its value and
	destruct as its destruct.

	As part of the library there is a macro ish_SetMap, which passes a NULL 
	destructor.	*/

int ish_MapSetWithDestruct(ish_Map *map, char *key, void *value, int (*destruct)(void *)) {
	ish_KVPair *pair;
	if ((pair = FindPair(map, key)) == NULL) pair = NewKVPair(map, key);

	if (pair) {
		pair->value = value;
		pair->destruct = destruct;
		return 1;

	}
	return 0;
}

/*	ish_MapProbePairs (public):
	In [map] we iterate over all of the KVPairs and call [func] on each.

	The signature for [func] is:
	'int func(char *[key], void *[value], void *[probe])'

	- [key] is the key of the KVPair.
	- [value] is the value of the KVPair.
	- [probe] is a pointer to an object that you want passing 
	when [func] is called.	

	There is a macro called ish_MapForPairs which calls this but 
	probe is set to NULL	*/

void ish_MapProbePairs(ish_Map *map, int (*func)(char *, void *, void *), void *probe) {
	if (!func) return;

	int i;
	for (i = 0; i < UINT8_MAX; i++) {
		ish_KVPair *pair;
		for (pair = map->buckets[i]; pair != NULL; pair = pair->next)
			func(pair->key, pair->value, probe);
	}
}


#undef ish_UINT128_LENGTH
