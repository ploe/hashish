#ifdef __cplusplus
extern "C" {
#endif

#ifndef ISH_PUBLIC
#define ISH_PUBLIC

#include <stdio.h>
#include <stdint.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

#include "murmur3.h"


#define ish_UINT128_LENGTH (sizeof(uint64_t)*2)
#define ish_DEFAULT_MASK 0x0F

/*	ish_KVPair:
	struct that represents a key/value pair in the Map. A key value
	pair has 3 attributes associated with it.

	[key] - the key to the KVPair, this is a char array that
	represents the name of the KVPair. Only one key can exist per Map.

	[value] - void * pointing to the object we want to store in the Map. 

	[destruct] - function pointer to a destructor function. The 
	function takes a void * as a param. This will be [value] in the 
	KVPair. 	*/

typedef struct ish_KVPair {
	uint64_t hash[2];
	char *key;
	void *value;
	int (*destruct)(void *);
	struct ish_KVPair *prev, *next;	
} ish_KVPair;

/*	ish_Map:
	A Map is the fundamental datatype in hashish. These entire module
	is geared around manipulating these bad boys.

	A Map is an array of linked lists that are KVPairs.	*/

typedef struct ish_Map {
	ish_KVPair **buckets;
	uint64_t mask;
} ish_Map;

/*	ish_Map methods.	*/
ish_Map *ish_MapNewWithMask(uint64_t mask);
#define ish_MapNew() ish_MapNewWithMask(ish_DEFAULT_MASK) 
void ish_MapFree(ish_Map *map);

int ish_MapRemove(ish_Map *map, char *key);

int ish_MapSetWithDestruct(ish_Map *map, char *key, void *value, int (*destruct)(void *));
#define ish_MapSet(map, key, value) ish_MapSetWithDestruct(map, key, value, NULL)

void *ish_MapGet(ish_Map *map, char *key);

void ish_MapProbePairs(ish_Map *map, int (*func)(char *, void *, void *), void *probe);
#define ish_MapForPairs(key, value) ish_MapProbePairs(key, value, NULL)


ish_Map *ish_MapGrow(ish_Map *old);
ish_Map *ish_MapShrink(ish_Map *old);

#endif

#ifdef __cplusplus
}
#endif
