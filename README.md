nanbox
======

A type that can store various types of data in 64-bits using NaN-boxing.

The header file `nanbox.h` defines a type `nanbox_t` which can be used to store either a double, a 32-bit integer, a pointer, a boolean, null or one of a few additional values named 'undefined', 'empty' and 'deleted' plus five additional 'auxillary' types of data of up to 48 bits. The encoding scheme differs between 32-bit and 64-bit platforms but the size of `nanbox_t` is always 64 bits.

How does it work?
-----------------

NaN-boxing is a way to store various information in unused NaN-space in the IEEE754 representation.

Any value with the top 13 bits set represents a *quiet NaN*. The remaining bits are called the 'payload'. NaNs produced by hardware and C-library functions typically produce a payload of zero. We assume that all quiet NaNs with a non-zero payload can be used to encode whatever we want.

On 64-bit platforms, unused bits in pointers are also used to encode various information. The representation is inspired by that used by Webkit's JavaScriptCore. It *should work* on most 32-bit and 64-bit little endian and big endian machines. (See Testing below.)

Functions
---------

A number of very short functions functions, all declared `static inline`, are defined to encode values as `nanbox_t`:

```c
nanbox_t nanbox_from_double(double d);
nanbox_t nanbox_from_int(int32_t i);
nanbox_t nanbox_from_pointer(void* pointer);
nanbox_t nanbox_from_boolean(bool b);
nanbox_t nanbox_null(void);
nanbox_t nanbox_undefined(void);
nanbox_t nanbox_empty(void);
nanbox_t nanbox_deleted(void);
nanbox_t nanbox_true(void);   /* the same as nanbox_from_boolean(true) */
nanbox_t nanbox_false(void);  /* the same as nanbox_from_boolean(false) */
```

... to check the type:

```c
bool nanbox_is_double(nanbox_t value);
bool nanbox_is_int(nanbox_t value);
bool nanbox_is_pointer(nanbox_t value);
bool nanbox_is_boolean(nanbox_t value);
bool nanbox_is_null(nanbox_t value);
bool nanbox_is_undefined(nanbox_t value);
bool nanbox_is_empty(nanbox_t value);
bool nanbox_is_deleted(nanbox_t value);
bool nanbox_is_true(nanbox_t value);
bool nanbox_is_false(nanbox_t value);
bool nanbox_is_number(nanbox_t value);  /* either int or double */
bool nanbox_is_undefined_or_null(nanbox_t value); /* either */
bool nanbox_is_aux(nanbox_t value);     /* auxillary space */
```

... and to decode the value:

```c
double nanbox_to_double(nanbox_t value);
int32_t nanbox_to_int(nanbox_t value);
void* nanbox_to_pointer(nanbox_t value);
bool nanbox_to_boolean(nanbox_t value);
double nanbox_to_number(nanbox_t value); /* value can be int or double */
```

Before fetching the value using these functions, you should make sure the nanbox is holdig a value of the correct type, e.g. using the corresponding `nanbox_is_...` function. If the encoded value is not of the correct type, the results of the `nanbox_to_...` functions are undefined. If compiled with assertions, you will get a failed assertion when trying to fetch a value of the wrong type.

The 'empty' value
-----------------

The 'empty' value is designed to used to represent empty slots in e.g. a hashtable. It is guarranteed to consist of a single repeated byte. This is to make sure `memset` can be used to set all the elements in an array of nanboxes to 'empty'. The macro `NANBOX_EMPTY_BYTE` represents the byte that, when repeated 8 times (64 bits), makes up an 'empty' value.

```c
void foo(void) {
	nanbox_t boxes[100];
	// Initialize the boxes to empty values
	memset(boxes, NANBOX_EMPTY_BYTE, sizeof(nanbox_t) * 100);
	// ...
}
```

User-defined prefix instead of 'nanbox'
---------------------------------------

You can define `NANBOX_PREFIX` to the prefix you want, before including
`nanbox.h`. Then, the functions and types will be e.g.
`bool myprefix_is_double(myprefix_t value)`, etc. By undefining `NANBOX_H` and
redefining `NANBOX_PREFIX` (and possibly some of the other macros such as
`NANBOX_POINTER_TYPE`) you can include `nanbox.h` multiple times to create
multiple instances of nanbox type.

User-defined pointer type
-------------------------

When encoding and decoding pointers to/from a nanbox, the pointer type `void*` is used by default. This can be changed by defining `NANBOX_POINTER_TYPE` to the pointer type of choice, before including `nanbox.h`. The type must be a pointer type, because unused bits in the pointers are used to encode various data.

Auxillary data
--------------

Apart from doubles, pointers, ints, booleans, null, etc. there are still some bits left to store even more types of data in a nanbox. We call this 'auxillary space'. To check if the type of data in a nanbox is 'auxillary data', the function `nanbox_is_aux` can be used, but accessing the data itself requires some insight into the internal representation of the nanbox. `nanbox_h` is a union type, which means it can be accessed in multiple ways. The easiest way is to access the nanbox raw data is as a 64-bit integer using `nanbox.as_int64`. You can only store 64-bit integer value in the range `NANBOX_MIN_AUX`..`NANBOX_MAX_AUX`, which is the 'auxillary space'. You can store 5 * 2<sup>48</sup> distinct values in this range, or equivallently, 5 types of 48-bit values.

Another way to access the data is to use `tag` and `payload`. These each represent 32 bits of the nanbox data. If the a nanbox has its tag (`nanbox.as_bits.tag`) in the range `NANBOX_MIN_AUX_TAG`..`NANBOX_MAX_AUX_TAG` and a payload `nanbox.as_bits.payload` being any 32-bit integer value, then the nanbox data is in auxillary space.

Short strings
-------------

As an example of what auxillary data can be used for, the file `nanbox_shortstring.h` is included, which implements a scheme to store strings of up to 6 bytes in the auxillary space of a nanbox. The functions `nanbox_is_shortstring`, `nanbox_shortstring_create`, etc. are defined and a small demo program is included in `shortstring_demo.c`.

Testing
-------

Tested on
  * x86-64 (Intel Core 2 Duo), Mac OS X (Darwin 10.0.8) in 64-bit and 32-bit mode, using gcc version 4.2.1 (Apple Inc. build 5664).

I would like to add more architectures to the above list, especially non-Intel ones such as ARM and big endian systems such as SPARC. If you test this on another architecture or with another compiler, please drop me a line!

To test with gcc, use the command `gcc -std=c99 -Wall -pedantic -o test test.c` to compile the test. It should produce no warnings. The executable `test` should run without outputting any errors.

On x86-64 platforms, it is also possible to test in 32-bit mode using the -m32 flag as in `gcc -m32 -std=c99 -Wall -pedantic -o test test.c`.
