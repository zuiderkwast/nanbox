#ifndef NANBOX_SHORTSTRING_H
#define NANBOX_SHORTSTRING_H
/*
 * Short strings
 * -------------
 * Strings of up to 6 bytes can be stored in a NANBOX_T in so called 'auxillary
 * space'. The space used is NANBOX_MIN_AUX..(NANBOX_MIN_AUX + 3 * 2^48 - 1).
 */

#include "nanbox.h"

static inline bool NANBOX_NAME(_is_shortstring)(NANBOX_T val) {
	return val.as_bits.tag >= NANBOX_MIN_AUX_TAG &&
	       val.as_bits.tag <= NANBOX_MIN_AUX_TAG + 0x0002ffff;
}
static inline char* NANBOX_NAME(_shortstring_chars)(NANBOX_T* val) {
	assert(NANBOX_NAME(_is_shortstring)(*val));
	#ifdef NANBOX_BIG_ENDIAN
	if (val->as_bits.tag & 0xffff0000 == NANBOX_MIN_AUX_TAG)
		return (char*)val + 4; /* skip tag and length */
	else
		return (char*)val + 2; /* skip tag */
	#else
	return (char*)val;
	#endif
}

static inline unsigned NANBOX_NAME(_shortstring_length)(NANBOX_T val) {
	assert(NANBOX_NAME(_is_shortstring)(val));
	if (val.as_bits.tag <= NANBOX_MIN_AUX_TAG + 4)
		return val.as_bits.tag - NANBOX_MIN_AUX_TAG;
	else
		return ((val.as_bits.tag - NANBOX_MIN_AUX_TAG) >> 16) + 4;
}

// creates a short string of length byts with undefined contents
static inline NANBOX_T NANBOX_NAME(_shortstring_create_undef)(unsigned length) {
	NANBOX_T val;
	assert(length <= 6);
	if (length <= 4)
		val.as_bits.tag = NANBOX_MIN_AUX_TAG + length;
	else
		val.as_bits.tag = NANBOX_MIN_AUX_TAG + ((length - 4) << 16);
	val.as_bits.payload = 0;
	return val;
}

// copies length bytes of chars. (nul bytes are copied like any other byte)
static inline NANBOX_T
NANBOX_NAME(_shortstring_create)(const char *chars, unsigned length) {
	NANBOX_T val = NANBOX_NAME(_shortstring_create_undef)(length);
	memcpy(NANBOX_NAME(_shortstring_chars)(&val), chars, length);
	assert(NANBOX_NAME(_is_shortstring)(val));
	return val;
}
#endif
