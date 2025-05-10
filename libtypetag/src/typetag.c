#include "typetag/typetag.h"
#include <assert.h>

/// Extracts the bits `offset..(offset + len)` within `tag`
typetag_t get_bits(typetag_t tag, short offset, short len) {
	return (tag >> offset) & (((typetag_t)1 << len) - 1);
}

/// Size of this object in bytes {1, 2, 4, 8}
int tt_get_obj_size(typetag_t tag) {
	// Size in bytes is 2^{bits}
	return (1 << (int)get_bits(tag, 0, 2));
}

/// Size of this object in bytes. {1, 2, 4, 8} are valid
/// sizes.
typetag_t tt_set_obj_size(typetag_t tag, int size) {
	int val = 0;
	switch(size) {
	case 1:
		val = 0b00;
		break;
	case 2:
		val = 0b01;
		break;
	case 4:
		val = 0b10;
		break;
	case 8:
		val = 0b11;
		break;
	default:
		assert(0);
	}

	const unsigned mask = 0b11; // Bits 0,1
	return (tag & ~mask) | val;
}

TagObjectType tt_get_obj_type(typetag_t tag) {
	return (TagObjectType)get_bits(tag, 2, 3);
}

typetag_t tt_set_obj_type(typetag_t tag, TagObjectType objtype) {
	const unsigned mask = 0b111 << 2; // Bits 2,3,4
	return (tag & ~mask) | (objtype << 2);
}

TagRefType tt_get_ref_type(typetag_t tag) {
	return (TagRefType)get_bits(tag, 5, 2);
}

typetag_t tt_set_ref_type(typetag_t tag, TagRefType reftype) {
	const unsigned mask = 0b11 << 5; // Bits 5,6
	return (tag & ~mask) | (reftype << 5);
}

int tt_is_multibyte(typetag_t tag) {
	return get_bits(tag, 7, 1);
}

typetag_t tt_set_multibyte(typetag_t tag, int is_multibyte) {
	const unsigned mask = 1 << 7; // Bit 7
	return (is_multibyte) ? (tag | mask) : (tag & ~mask);
}
