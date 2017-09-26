# hashish

## What is hashish?

`hashish` is a Hash Map library for C. It is a `key-value` store that uses `char *` for the keys and `void *` for the values. 

It has a small set of features that give you control over the shape and size of your data.

## How do I build it?

To use `hashish` first you must clone the repo (if you're reading this you're looking at it) and `./configure` it, to download and build the dependencies, and then `make` build it.

Once the library has been built you can include it in your C apps by pointing to the repo when compiling, for example: 

```bash
cc -I./hashish ./hashish/hashish.o src.c -o app
```

There is a bundled example app that you can build by doing a `make dev` - should you be so inclined. Its code in `main.c` may be helpful in getting started with this library.

## How do I get started using it?

Include the library.

```c
#include "hashish.h"
```

The basic type in hashish is an `ish_Map` to create one you need to use the `ish_MapNew` function. See below.

```c
	ish_Map *map = ish_MapNew();
```

If `map` is not `NULL` then congratulations, you've got a freshly allocated `ish_Map`.

To put stuff in `map` we can use the `ish_MapSet` function.

```c
	ish_MapSet(map, "my key", &value);
```

This will set the `my key` in the `map` to the address of `value`. 

All `key-value` pairs in `hashish` are `char *` keys and `void *` values, this is so you can use the a `map` to point to any arbitrary object.


Let's stick a bunch of `key-value pairs` in our `map`.

```c
	ish_MapSet(map, "matthew", "hi");
	ish_MapSet(map, "mark", "hiya");
	ish_MapSet(map, "luke", "hey");
	ish_MapSet(map, "ringo", "hullo");
```

To pull the values out of `map` we use the `ish_MapGet` function.

```c
	char *msg = (char *) ish_MapGet(map, "ringo");
	printf("%s, world", msg);		// prints "hullo, world"
```

If you don't like what you put in the `map` you can always overwrite it with another `ish_MapSet` or get rid of it completely with an `ish_MapRemove`

```c
	puts(ish_MapGet(map, "mark"));		// prints "hiya"
	ish_MapSet(map, mark, "bye");
	puts(ish_MapGet(map, "mark"));		// prints "bye"

	puts(ish_MapGet(map, "matthew"));	// prints "hi"
	ish_MapRemove(map, "matthew");
	puts(ish_MapGet(map, "matthew"));	// prints "(null)"
```

Once you're done with the `map` you can deallocate it and its `key-value pairs` by using the `ish_MapFree` function.

```c
	ish_MapFree(map);
```

## What are Allocator functions?

The library doesn't implement garbage collection, but it provides a pair of callback functions for each `key-value pair` - these are called by `ish_MapGet` and `ish_MapDrop`.

Below is the `typedef` for the `ish_Allocator` signature. They are return a `void *` which should be the value in the `key-value pair`. Their arguments are an `ish_Map *` which is the pointer to the map we're operating on, a `char *` which is the `key` and a `void *` which is the value.

```c
	typedef void *(*ish_Allocator)(ish_Map *map, char *key, void *value);
```

Functions that call the `drop Allocator` include `ish_MapFree`, `ish_MapRemove`, `ish_MapSet` and `ish_MapSetWithAllocators`. 

Only `ish_MapGet` calls the `get Allocator`.

With these primitives you can roll you own garbage collection around the structures in the map.

Here is an example type from `main.c` called `Smartref` which illustrates how something like this could be done.

```c
typedef struct Smartref {
	unsigned int rc;
	char *value;
} Smartref;

```

`Smartref` has two attributes. An `unsigned int` called `rc` which stands for reference count. This is used to keep track of the number of references to this object the program has checked out with `ish_MapGet`. `char *value` represents the value of the object. In this case it's a char array.

When `ish_MapGet` is called we want `rc` to increment, so that the app knows how many instances of the reference are floating in memory somewhere.

```c
void *SmartrefGet(ish_Map *map, char *key, void *value) {
	Smartref *ref = (Smartref *) value;
	ref->rc++;
	printf("%s => %s: %d\n", key, ref->value, ref->rc);

	return value;
}
```

When `ish_MapDrop` is called we want to remove the value and free up the associated memory. Returning `NULL` tells `hashish` the value in the map should be empty, so it ensues to free the `key-value pair from memory.`

```c
void *SmartrefDrop(ish_Map *map, char *key, void *value) {
	Smartref *ref = (Smartref *) value;
	ref->rc--;
	printf("%s => %s: %d\n", key, ref->value, ref->rc);
	if (ref->rc <= 0) {
		printf("Dropped at: %s => %s: %d\n", key, ref->value, ref->rc);
		free(value);
		return NULL;
	}
	
	return value;
}
```

Below is an example constructor which is used to allocate and initialise our `Smartref` type.

```c
Smartref *SmartrefNew(ish_Map *map, char *key, void *value) {
	Smartref *ref = calloc(1, sizeof(Smartref));
	ref->rc = 1;
	ref->value = value;
	printf("%s => %s: %d\n", key, ref->value, ref->rc);

	ish_MapSetWithAllocators(map, key, (void *) ref, SmartrefGet, SmartrefDrop);

	return ref;
}

```

In then gets called as below:

```c
	SmartrefNew(map, "smartref", ";)");
	ish_MapGet(map, "smartref");
	ish_MapGet(map, "smartref");
	ish_MapGet(map, "smartref");
	ish_MapDrop(map, "smartref");
	ish_MapDrop(map, "smartref");
	ish_MapDrop(map, "smartref");
	ish_MapDrop(map, "smartref");
```

Which gives us the following output.

```c
smartref => ;): 1
smartref => ;): 2
smartref => ;): 3
smartref => ;): 4
smartref => ;): 3
smartref => ;): 2
smartref => ;): 1
smartref => ;): 0
Dropped at: smartref => ;): 0
```

These functions allow you degree of control over whether you want memory freeing up or retaining when using `hashish`.

## Methods

### void ish_MapForPairs(ish_Map \*map, int (\*func)(char \*, void \*, void \*));

In **map** we iterate over all of the `key-value pairs` and call **func** on each.

The signature for **func** is `int func(char *[key], void *[value], void *[probe])`
   - **key** is the key of the `key-value pair`.
   - **value** is the value of the `key-value pair`.
   - **probe** is a pointer to an object that you want passing when **func** is called.

This is a macro for `ish_MapProbePairs` but it sets the probe to NULL.

### void ish_MapFree(ish_Map \*map);

Purges all the `key-value pairs` from the **map** and then deallocates it.

If a destructor is set for the `key-value pair` it will be called before being deallocated.

```c
	ish_Map *map = ish_MapNew();
	ish_MapSet(map, "1", "foo");
	ish_MapSet(map, "2", "bar");
	ish_MapFree(map);
	map = NULL;
```

### void \*ish_MapGet(ish_Map \*map, char \*key);

Returns the value from **key** in the **map**.

```c
	ish_Map *map = ish_MapNew();
	ish_MapSet(map, "1", "foo");
	char *msg = (char *) ish_MapGet(map, "1");
	ish_MapSet(map, "2", "bar");
	puts(msg);	//prints "foo"
```

### ish_Map \*ish_MapGrow(ish_Map \*old);

Rehash the **map** with a larger array of internal buckets. Returns the address of the new `map` if it was successful, otherwise it will return the address of the old one.

This is typically done to speed up access to the `key-value` pairs if you have a lot floating around in memory.

```c
	ish_Map *map = ish_MapNew();
	ish_MapGrow(map);
```

### ish_Map \*ish_MapNew();

We allocate, initialise and return an addres to the new ish_Map if successful. Otherwise we return `NULL`.

```c
	ish_Map *map = ish_MapNew();
```

### void ish_MapProbePairs(ish_Map \*map, int (\*func)(char \*, void \*, void \*), void \*probe);

In **map** we iterate over all of the `key-value pairs` and call **func** on each.

The signature for **func** is `int func(char *[key], void *[value], void *[probe])`
   - **key** is the key of the `key-value pair`.
   - **value** is the value of the `key-value pair`.
   - **probe** is a pointer to an object that you want passing when **func** is called.

### int ish_MapRemove(ish_Map \*map, char \*key);

Deletes (i.e completely deallocates) the `key-value pair`.

```c
	puts(ish_MapGet(map, "matthew"));	// prints "hi"
	ish_MapRemove(map, "matthew");
	puts(ish_MapGet(map, "matthew"));	// prints "(null)"
```

### int ish_MapSet(ish_Map \*map, char \*key, void \*value);

In **map** we set `key-value pair` **key** to have **value** as its value.

This is a macro for `ish_MapSetWithDestruct` with **destruct** set to `NULL`.

```c
	ish_Map *map = ish_MapNew();
	ish_MapSet(map, "1", "foo");
```

### ish_Map \*ish_MapShrink(ish_Map \*old);

Rehash the **old** with a smaller array of internal buckets. Returns the address of the new `map` if successful, otherwise it will return the address of the old one.

```c
	ish_Map *map = ish_MapNew();
	ish_MapShrink(map);
```

## License

```
Copyright (c) 2016-2017, Myke Atkinson
All rights reserved.

Redistribution and use in source and binary forms, with or without
modification, are permitted provided that the following conditions are met:

1. Redistributions of source code must retain the above copyright notice, this
   list of conditions and the following disclaimer.
2. Redistributions in binary form must reproduce the above copyright notice,
   this list of conditions and the following disclaimer in the documentation
   and/or other materials provided with the distribution.

THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND
ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED
WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT OWNER OR CONTRIBUTORS BE LIABLE FOR
ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES
(INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES;
LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND
ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
(INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS
SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
```
