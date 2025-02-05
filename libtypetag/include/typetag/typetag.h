#ifndef TYPETAG_H
#define TYPETAG_H

#include <stdint.h>

#define TT_EXP_INT_OVERFLOW         0x401
#define TT_EXP_INT_DIV_ZERO         0x402
#define TT_EXP_INVALID_CALL_TAG     0x403
#define TT_EXP_INVALID_INDIRECT_TAG 0x404
#define TT_EXP_INVALID_RETURN_TAG   0x405

#ifdef __cplusplus
extern "C" {
#endif

typedef uint8_t  typetag_t;
typedef uint32_t tagexception_t;

typedef enum {
	TT_OBJ_SPECIAL = 0,
	TT_OBJ_CODE,
	TT_OBJ_COLLECTION,
	TT_OBJ_RAW,
	TT_OBJ_UINT,
	TT_OBJ_INT,
	TT_OBJ_FLOAT,
	TT_OBJ_RETURN
} TagObjectType;

typedef enum {
	TT_REF_NONE = 0,
	TT_REF_BASIC,
	TT_REF_COLLECTION,
	TT_REF_COLLECTION_REF
} TagRefType;

/// Size of this object in bytes {1, 2, 4, 8}
int 		  tt_get_obj_size(typetag_t tag);
TagObjectType tt_get_obj_type(typetag_t tag);
TagRefType 	  tt_get_ref_type(typetag_t tag);
int 		  tt_get_is_multibyte(typetag_t tag);

typetag_t  tt_set_obj_size(typetag_t tag, int size);
typetag_t  tt_set_obj_type(typetag_t tag, TagObjectType objtype);
typetag_t  tt_set_ref_type(typetag_t tag, TagRefType reftype);
typetag_t  tt_set_multibyte(typetag_t tag, int is_multibyte);

#ifdef __cplusplus
} // extern "C"
#endif

#endif // TYPETAG_H