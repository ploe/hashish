#include "hashish.h"

#include "murmur3.h"
/*	KVPair methods	*/

/*	SetHash.
	Uses Murmur3 to convert a [key] in to a new [hash]	*/

static void SetHash(char *key, void *out) {
	MurmurHash3_x64_128(key, strlen(key), 0, out);
}

static uint64_t GetIndex(uint64_t hash[2], uint64_t mask) {
	return hash[0] & mask;
}

/*	FindPair:
	Finds the KVPair with [key] in [map]	*/
	
static ish_KVPair *FindPair(ish_Map *map, char *key) {
	uint64_t hash[2], index;
	SetHash(key, hash);
	index = GetIndex(hash, map->mask);

	ish_KVPair *top = map->buckets[index], *pair;
	for (pair = top; pair != NULL; pair = pair->next) {
		if (memcmp(hash, pair->hash, ish_UINT128_LENGTH) == 0)
			if (strcmp(key, pair->key) == 0) return pair;
	}
	return NULL;
}

/*	KVPairNew:
	Creates a new KVPair at [key] on the [map]	*/

static ish_KVPair *KVPairNew(ish_Map *map, char *key) {
	ish_KVPair *pair = calloc(1, sizeof(ish_KVPair));
	if (pair) {
		SetHash(key, pair->hash);

		pair->key = calloc(strlen(key) + 1, sizeof(char));
		if (!pair->key) {
			free(pair);
			return NULL;
		}
		strcpy(pair->key, key);

		uint64_t index = GetIndex(pair->hash, map->mask);
		ish_KVPair *top = map->buckets[index];

		if (top) top->prev = pair;
		pair->next = top;

		map->buckets[index] = pair;
	}

	return pair;
}

static void KVPairFree(ish_Map *map, ish_KVPair *pair) {
	/*	destroy the key	*/
	free(pair->key);


	/*	rewire the pairs in the map	*/
	if (pair->prev) pair->prev->next = pair->next;
	else {
       	/*      if it's the top node we have to rewire the bucket to point to it        */
		uint64_t index = GetIndex(pair->hash, map->mask);
		map->buckets[index] = pair->next;
	}

	if (pair->next) pair->next->prev = pair->prev;

	free(pair);
}

/*	ish_Map methods.	*/

/*	ish_MapNewWithMask (public):
 	We allocate and initialise a new ish_Map using [mask] for the number of 
	buckets.  If it can't be allocated the method returns NULL.

	Even though it's public I'd recommend using the macro ish_MapNew as it 
	calls the same, but with the [mask] set to the default value.	*/

ish_Map *ish_MapNewWithMask(uint64_t mask) {
	ish_Map *map = calloc(1, sizeof(ish_Map));
	if (map) {
		map->mask = mask;
		map->buckets = calloc(mask + 1, sizeof(ish_KVPair));
		if (!map->buckets) {
			free(map);
			return NULL;
		}
	}
	return map;
}

/*	ish_CopyMap (public):
	Copy the contents on one [old] in to [new].	

	Any values that are in [new] that are overwritten will have
	remove called for them. Any values that aren't will remain
	in memory, untouched.	*/

int ish_CopyMap(ish_Map *old, ish_Map *new) {
	int i;
	for (i = 0; i <= old->mask; i++) {
		ish_KVPair *pair;
		for (pair = old->buckets[i]; pair != NULL; pair = pair->next) {
			if(!ish_MapSetWithAllocators(new, pair->key, pair->value, pair->get, pair->drop)) return 0;
		}
	}
	return 1;
}

/*	ish_MapFree (public):
	Purges all the KVPairs from the [map] and then deallocates it.	
	Calls drop on every element. */
void ish_MapFree(ish_Map *map) {
	int i;
	for (i = 0; i <= map->mask; i++) {
		ish_KVPair *pair, *next;
		for (pair = map->buckets[i]; pair != NULL; pair = next) {
			next = pair->next;
			if (pair->drop) pair->drop(map, pair->key, pair->value);
			KVPairFree(map, pair);
		}
	}
	free(map->buckets);
	free(map);
}

static ish_Map *MapRehash(ish_Map *old, uint64_t mask) {
	ish_Map *new = ish_MapNewWithMask(mask);
	if (!new) return old;

	if(!ish_CopyMap(old, new)) {
		ish_MapFree(new);
		return old;
	}

	ish_MapFree(old);
	return new;
}

ish_Map *ish_MapGrow(ish_Map *old) {
	uint64_t mask = old->mask;
	mask <<= 1;
	mask += 1;

	return MapRehash(old, mask);
}

ish_Map *ish_MapShrink(ish_Map *old) {
	uint64_t mask = old->mask;
	mask >>= 1;

	return MapRehash(old, mask);
}

/*	ish_MapRemove (public):
	Deletes (i.e completely deallocates) the KVPair. */

int ish_MapRemove(ish_Map *map, char *key) {
	ish_KVPair *pair = FindPair(map, key);
	if (!pair) return ish_FAIL;

	if (pair->drop) {
		if (pair->drop(map, pair->key, pair->value)) KVPairFree(map, pair);
	}

	return ish_SUCCESS;
}

/*	ish_MapSetWithAllocators (public):
	In [map] we set KVPair [key] to have [value] as its value and
	[get] is its get method.
	[drop] is its drop method.
	[remove] is its remove method.

	As part of the library there is a macro ish_SetMap, which passes 
	NULL to all the methods.	*/

int ish_MapSetWithAllocators(ish_Map *map, char *key, void *value, ish_Allocator get, ish_Allocator drop) {
	ish_KVPair *pair;
	if ((pair = FindPair(map, key)) == NULL) pair = KVPairNew(map, key);
	if (pair) {
		if (pair->value && pair->drop) pair->drop(map, key, pair->value);

			
		pair->value = value;
		pair->get = get;
		pair->drop = drop;
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

void ish_MapProbePairs(ish_Map *map, ish_Iterator func, void *probe) {
	if (!func) return;

	int i;
	for (i = 0; i <= map->mask; i++) {
		ish_KVPair *pair;
		for (pair = map->buckets[i]; pair != NULL; pair = pair->next)
			func(map, pair->key, pair->value, probe);
	}
}

/*	ish_MapGet (public):
	Returns the value from [key] in the [map].
	The callback [get] for that KVPair will be called.

	This can be used to implement reference counting.	*/

void *ish_MapGet(ish_Map *map, char *key) {
	ish_KVPair *pair = FindPair(map, key);

	if (pair) {
		if (pair->get) return pair->get(map, key, pair->value);
		return pair->value;
	}

	return NULL;
}

/*      ish_MapDrop (public):
        Returns the value from [key] in the [map].
        The callback [drop] for that KVPair will be called.

        This can be used to implement reference counting.       */

void *ish_MapDrop(ish_Map *map, char *key) {
        ish_KVPair *pair = FindPair(map, key);

        if (pair) {
                if (!pair->drop) return pair->value;

		void *alive = pair->drop(map, key, pair->value);
		if (alive) return alive;

		KVPairFree(map, pair);
        }

        return NULL;
}

int ish_MapOnGet(ish_Map *map, char *key, ish_Allocator get) {
	ish_KVPair *pair = FindPair(map, key);
	
	if (pair) {
		pair->get = get;
		return ish_SUCCESS;
	}

	return ish_FAIL;
}

int ish_MapOnDrop(ish_Map *map, char *key, ish_Allocator drop) {
	ish_KVPair *pair = FindPair(map, key);
	
	if (pair) {
		pair->drop = drop;
		return ish_SUCCESS;
	}

	return ish_FAIL;
}

