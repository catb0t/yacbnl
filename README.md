# YACBNL

---


yet another c big number library
YACBiNuLi, "yak-binewly"

a (toy) single-header lightweight C arbitrary precision library using uint8_t* arrays.

the library isn't written as a single header, but is built into `yacbnl.full.h`; just include that. (see `build.sh`).

to compile with slower string operations for primitive conversions rather than floating point math, use `-DPREFER_CHAR_CONV` on the command line or `#define PREFER_CHAR_CONV` before the `#include "yacbnl.h"`.

written in C99 and C11. does not compile in C++; no plans for this currently. you should probably use boost, and in C, you should probably use GMP because this is a toy.

---

the tests need the excellent criterion testing framework http://github.com/snaipe/criterion

binaries I accidentally commit are unoptimised, debug ELFs built on and for Linux x64 kernel 4.x.

list of generated files:

- test_yacbnl.c (by concat_tests.sh)
- yacbnl.full.h (by build.sh)
- yacbnl.min.h  (by build.sh)