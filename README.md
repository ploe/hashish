# hashish

## What is hashish?

`hashish` is a Hash Map library for C, and it has a small set of features that give you control over the shape and size of your data.

## How do I build it?

To use `hashish` first you must clone the repo (iif you're reading this you're looking at it) and `./configure` it (to download the dependencies) and then `make` it.

Once the library has been built you can include it in your C apps by pointing to the repo when compiling, for example: 

```
cc -O2 -I./murmur3 hashish/hashish.o main.c -o main
```

There is a bundled example app that you can build by doing a `make dev` - should you be so inclined.

## How do I use it?

Make sure you include the library with an `#include "hashish"` directive.

The basic type in hashish is an `ish_Map` to create one you need to use the `ish_MapNew` function. See below.

```
...
	ish_Map *map = ish_NewMap();
...
```

If `map` is not `NULL` then congratulations, you've got a freshly allocated `ish_Map`.

To put stuff in `map` we can do use the `ish_MapSet` function.

```
...
	ish_MapSet(map, "my key", &value);
...
```

This will set the `my key` in the `map` to the address of `value`. All `key-value` pairs in `hashish` are `char *` keys and `void *` values, this is so you can use the a `map` to point to any arbitrary object. This is in the interest of keeping the same basic framework flexible.
