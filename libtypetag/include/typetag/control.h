#ifndef TYPETAG_RISCV_H
#define TYPETAG_RISCV_H

#include "typetag/typetag.h"
#include "typetag/exception.h"

/// Enable or disable a particular tagging exception
/// when tt checks or propagation is enabled
void tt_set_exception(tagexception_t exception, TrapMode mode);

/// Enable or disable tag propagation
void tt_set_checks(int enabled);
/// Enable or disable tag propagation
void tt_set_prop(int enabled);

typetag_t tt_get_tag(char* ptr);
void tt_set_tag(char* ptr, typetag_t tag);

#endif // TYPE_TAG_H