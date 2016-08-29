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

## Methods

### ish_MapForPairs(map, func)

In **map** we iterate over all of the `key-value pairs` and call **func** on each.

The signature for **func** is `int func(char *[key], void *[value], void *[probe])`
   - **key** is the key of the `key-value pair`.
   - **value** is the value of the `key-value pair`.
   - **probe** is a pointer to an object that you want passing when **func** is called.

This is a macro for `ish_MapProbePairs` but it sets the probe to NULL.

### ish_MapFree(map)

Purges all the `key-value pairs` from the **map** and then deallocates it.

If a destructor is set for the `key-value pair` it will be called before being deallocated.

```c
	ish_Map *map = ish_MapNew();
	ish_MapSet(map, "1", "foo");
	ish_MapSet(map, "2", "bar");
	ish_MapFree(map);
	map = NULL;
```

### ish_MapGet(map, key)

Returns the value from **key** in the **map**.

```c
	ish_Map *map = ish_MapNew();
	ish_MapSet(map, "1", "foo");
	char *msg = (char *) ish_MapGet(map, "1");
	ish_MapSet(map, "2", "bar");
	puts(msg);	//prints "foo"
```

### ish_MapGrow(map)

Rehash the **map** with a larger array of internal buckets. Returns the address of the new `map` if it was successful, otherwise it will return the address of the old one.

This is typically done to speed up access to the `key-value` pairs if you have a lot floating around in memory.

```c
	ish_Map *map = ish_MapNew();
	ish_MapGrow(map);
```

### ish_MapNew()

We allocate, initialise and return an addres to the new ish_Map if successful. Otherwise we return `NULL`.

```c
	ish_Map *map = ish_MapNew();
```

### ish_MapProbePairs(map, func, probe)

In **map** we iterate over all of the `key-value pairs` and call **func** on each.

The signature for **func** is `int func(char *[key], void *[value], void *[probe])`
   - **key** is the key of the `key-value pair`.
   - **value** is the value of the `key-value pair`.
   - **probe** is a pointer to an object that you want passing when **func** is called.

### ish_MapRemove(map, key)

Deletes (i.e completely deallocates) the `key-value pair`.

```c
	puts(ish_MapGet(map, "matthew"));	// prints "hi"
	ish_MapRemove(map, "matthew");
	puts(ish_MapGet(map, "matthew"));	// prints "(null)"
```

### ish_MapSet(map, key, value)

In **map** we set `key-value pair` **key** to have **value** as its value.

This is a macro for `ish_MapSetWithDestruct` with **destruct** set to `NULL`.

```c
	ish_Map *map = ish_MapNew();
	ish_MapSet(map, "1", "foo");
```

### ish_MapSetWithDestruct(map, key, value, destruct)

In **map** we set `key-value pair` **key** to have **value** as its value and **destruct** as its `destructor`.

When `ish_MapSet`, `ish_MapRemove` and `ish_MapFree` are called on a **key** with its `destructor` set, then **destruct** will be called on **value**.

The signature for **destruct** is `int func(void *)`

### ish_MapShrink(map)

Rehash the **old** with a smaller array of internal buckets. Returns the address of the new `map` if successful, otherwise it will return the address of the old one.

```c
	ish_Map *map = ish_MapNew();
	ish_MapShrink(map);
```



## License

```
Copyright (c) 2016, Myke Atkinson
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
