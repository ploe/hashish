#ifdef __cplusplus
extern "C" {
#endif

#ifndef ISH_PUBLIC
#define ISH_PUBLIC

#include <stdio.h>
#include <stdint.h>
#include <stdlib.h>
#include <string.h>

#include "murmur3.h"

#define ish_UINT128_LENGTH (sizeof(uint64_t)*2)
#define ish_DEFAULT_MASK 0x0F

typedef struct ish_Map ish_Map;

typedef void *(*ish_Allocator)(ish_Map *map, char *key, void *value);

/*	ish_KVPair:
	struct that represents a key/value pair in the Map. A key value
	pair has 3 attributes associated with it.

	[key] - the key to the KVPair, this is a char array that
	represents the name of the KVPair. Only one key can exist per Map.

	[value] - void * pointing to the object we want to store in the Map. 

	[remove] - function pointer to the remove method.
*/

typedef struct ish_KVPair {
	uint64_t hash[2];
	char *key;
	void *value;
	ish_Allocator get, drop, remove;
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

enum {
	ish_SUCCESS = -1,
	ish_FAIL
};

/*	ish_Map methods.	*/
ish_Map *ish_MapNewWithMask(uint64_t mask);
#define ish_MapNew() ish_MapNewWithMask(ish_DEFAULT_MASK) 
void ish_MapFree(ish_Map *map);

int ish_MapRemove(ish_Map *map, char *key);

int ish_MapSetWithAllocators(ish_Map *map, char *key, void *value, ish_Allocator get, ish_Allocator drop, ish_Allocator remove);
#define ish_MapSet(map, key, value) ish_MapSetWithAllocators(map, key, value, NULL, NULL, NULL)

void *ish_MapGet(ish_Map *map, char *key);

void ish_MapProbePairs(ish_Map *map, int (*func)(char *, void *, void *), void *probe);
#define ish_MapForPairs(key, value) ish_MapProbePairs(key, value, NULL)

ish_Map *ish_MapGrow(ish_Map *old);
ish_Map *ish_MapShrink(ish_Map *old);

int ish_MapOnGet(ish_Map *map, char *key, ish_Allocator get);
int ish_MapOnDrop(ish_Map *map, char *key, ish_Allocator drop);
int ish_MapOnRemove(ish_Map *map, char *key, ish_Allocator remove);

#endif

#ifdef __cplusplus
}
#endif
