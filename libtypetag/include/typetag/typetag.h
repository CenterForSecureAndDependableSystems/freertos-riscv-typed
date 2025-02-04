#ifndef TYPETAG_H
#define TYPETAG_H

#include <stdint.h>

#ifdef __cplusplus
extern "C" {
#endif

typedef uint8_t typetag_t;
typedef uint32_t tagexception_t;

enum TagObjectType {
	OBJ_SPECIAL = 0,
	OBJ_CODE,
	OBJ_COLLECTION,
	OBJ_RAW,
	OBJ_UINT,
	OBJ_INT,
	OBJ_FLOAT,
	OBJ_REF
};

enum TagRefType {
	REF_NONE = 0,
	REF_BASIC,
	REF_COLLECTION,
	REF_COLLECTION_REF
};

/// Size of this object in bytes {1, 2, 4, 8}
void tt_test();
int tt_get_obj_size(typetag_t tag);
enum TagObjectType tt_get_tag_type(typetag_t tag);
enum TagRefType tt_get_ref_type(typetag_t tag);
int tt_get_is_multibyte(typetag_t tag);

#ifdef __cplusplus
} // extern "C"
#endif

#endif // TYPETAG_H