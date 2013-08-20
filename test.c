#include <assert.h>
#include <math.h>
#include <stdio.h>

#include "nanbox.h"

// This macro stores a value VALUE of type TYPE in a nanbox, checks the type,
// converts back and checks that we got the value back. It also checks that the
// nanbox is not of any other type.
//
// The only NaN expression that is possible to test with this expression is
// 0.0/0.0. (There is some special logic to NaN, because NaN != NaN.
#define TO_NANBOX_AND_BACK(TYPE, VALUE) do {                                  \
	nanbox_t x = nanbox_from_##TYPE(VALUE);                               \
	assert(nanbox_is_##TYPE(x));                                          \
	/* decode and test == to the original, except for NaN */              \
	if (!strcmp(#TYPE, "double") && !strcmp(#VALUE, "0.0/0.0"))           \
		assert(VALUE != nanbox_to_##TYPE(x));                         \
	else                                                                  \
		assert(VALUE == nanbox_to_##TYPE(x));                         \
	assert(nanbox_is_double(x)  == !strcmp(#TYPE, "double"));             \
	assert(nanbox_is_int(x)     == !strcmp(#TYPE, "int"));                \
	assert(nanbox_is_pointer(x) == !strcmp(#TYPE, "pointer"));            \
	assert(nanbox_is_boolean(x) == !strcmp(#TYPE, "boolean"));            \
	assert(nanbox_is_number(x)  == (!strcmp(#TYPE, "double") ||           \
	                                !strcmp(#TYPE, "int")));              \
	assert(!nanbox_is_null(x));                                           \
	assert(!nanbox_is_undefined(x));                                      \
	assert(!nanbox_is_empty(x));                                          \
	assert(!nanbox_is_deleted(x));                                        \
	assert(!nanbox_is_aux(x));                                            \
	assert(nanbox_is_true(x)    == !strcmp(#VALUE, "true"));              \
	assert(nanbox_is_false(x)   == !strcmp(#VALUE, "false"));             \
} while(0)

// Use this to create and check a nanbox of null, undefined, empty, deleted,
// true or false. It tests that it is of the correct type and no other type.
#define TO_NANBOX_AND_CHECK(VALUE) do {                                       \
	nanbox_t x = nanbox_##VALUE();                                        \
	assert(!nanbox_is_double(x));                                         \
	assert(!nanbox_is_int(x));                                            \
	assert(!nanbox_is_pointer(x));                                        \
	assert(!nanbox_is_number(x));                                         \
	assert(!nanbox_is_aux(x));                                            \
	assert(nanbox_is_boolean(x)   == (!strcmp(#VALUE, "true") ||          \
	                                  !strcmp(#VALUE, "false")));         \
	assert(nanbox_is_undefined_or_null(x) ==                              \
	       (!strcmp(#VALUE, "undefined") || !strcmp(#VALUE, "null")));    \
	assert(nanbox_is_null(x)      == !strcmp(#VALUE, "null"));            \
	assert(nanbox_is_undefined(x) == !strcmp(#VALUE, "undefined"));       \
	assert(nanbox_is_empty(x)     == !strcmp(#VALUE, "empty"));           \
	assert(nanbox_is_deleted(x)   == !strcmp(#VALUE, "deleted"));         \
} while(0)

// Definded below. Called from main.
void test_nan(void);

int main() {
	// Size should be 16 bits
	assert(sizeof(nanbox_t) == 8);

	// Test storing various doubles, including NaN and infinity.
	TO_NANBOX_AND_BACK(double, -0.0);
	TO_NANBOX_AND_BACK(double, 3.14);
	TO_NANBOX_AND_BACK(double, 1.0/0.0);
	TO_NANBOX_AND_BACK(double, -1.0/0.0);
	TO_NANBOX_AND_BACK(double, 0.0/0.0);

	// Test storing int, pointer and boolean
	TO_NANBOX_AND_BACK(int, 42);
	TO_NANBOX_AND_BACK(pointer, &x);
	TO_NANBOX_AND_BACK(boolean, true);
	TO_NANBOX_AND_BACK(boolean, false);

	// The remaining types/values
	//TO_NANBOX_AND_CHECK(null);
	//TO_NANBOX_AND_CHECK(undefined);
	TO_NANBOX_AND_CHECK(empty);
	TO_NANBOX_AND_CHECK(deleted);
	TO_NANBOX_AND_CHECK(true);
	TO_NANBOX_AND_CHECK(false);

	test_nan();

	return 0;
}

// A macro to check that a double is a cannonical NaN, i.e. one that we accept.
// Also, nanboxes it and checks that it identified as a double.
#define ASSERT_CANNONICAL_NAN(VALUE) do {                                         \
	double d = VALUE;                                                         \
	uint64_t n = *(uint64_t*)&d;                                              \
	assert((n | 0x8000000000000000llu) == 0xfff8000000000000llu);             \
	assert(nanbox_is_double(nanbox_from_double(VALUE)));                      \
} while(0)

void test_nan(void) {
	double nan = 0.0/0.0, inf = 1.0/0.0, ninf = -1.0/0.0;
	assert(nan != nan);
	ASSERT_CANNONICAL_NAN(0.0/0.0);
	ASSERT_CANNONICAL_NAN(nan);
	ASSERT_CANNONICAL_NAN(nan + 42);
	ASSERT_CANNONICAL_NAN(-inf * nan);

	ASSERT_CANNONICAL_NAN(inf/inf);
	ASSERT_CANNONICAL_NAN(ninf/inf);
	ASSERT_CANNONICAL_NAN(0 * inf);
	ASSERT_CANNONICAL_NAN(0 * ninf);
	ASSERT_CANNONICAL_NAN(inf * 0);
	ASSERT_CANNONICAL_NAN(inf + ninf);
	ASSERT_CANNONICAL_NAN(ninf + inf);

	ASSERT_CANNONICAL_NAN(pow(-1.0, 3.14));
	ASSERT_CANNONICAL_NAN(sqrt(-1.0));
	ASSERT_CANNONICAL_NAN(log(-1.0));
	ASSERT_CANNONICAL_NAN(asin(2.0));
	ASSERT_CANNONICAL_NAN(acos(2.0));
}
