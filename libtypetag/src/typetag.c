#include "typetag/typetag.h"

typetag_t get_bits(typetag_t tag, short offset, short len) {
	return (tag >> offset) & (((typetag_t)1 << len) - 1);
}

/// Size of this object in bytes {1, 2, 4, 8}
int tt_get_obj_size(typetag_t tag) {
	// Size in bytes is 2^{bits}
	return (1 << (int)get_bits(tag, 0, 2));
}

enum TagObjectType tt_get_tag_type(typetag_t tag) {
	return (enum TagObjectType)get_bits(tag, 2, 3);
}

enum TagRefType tt_get_ref_type(typetag_t tag) {
	return (enum TagRefType)get_bits(tag, 5, 2);
}

int tt_is_multibyte(typetag_t tag) {
	return get_bits(tag, 7, 1);
}


// typetag_t tt_set_obj_size(typetag_t tag, uint8_t size) {
// 	// Size in bytes is 2^{bits}
// 	return tag | 
// 	return (1 << (int)get_bits(tag, 0, 2));
// }

// typetag_t tt_set_tag_type(typetag_t tag, enum TagObjectType t) {
// 	return (ObjType)get_bits(tag, 2, 3);
// }

// typetag_t tt_set_ref_type(typetag_t tag, enum TagRefType t) {
// 	return (RefType)get_bits(tag, 5, 2);
// }

// typetag_t tt_set_is_multibyte(typetag_t tag, int multibyte) {
// 	return tag | ((multibyte) ? 1 : 0) << 7;
// }
