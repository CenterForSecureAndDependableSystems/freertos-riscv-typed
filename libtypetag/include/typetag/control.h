#ifndef TYPETAG_RISCV_H
#define TYPETAG_RISCV_H

#include "typetag/typetag.h"

// Enable or disable a particular tagging exception
// when tt checks or propagation is enabled
void tt_set_exception(tagexception_t exception, int enabled);

// TODO [TAG]: Need hint instruction. Necessary?
// int tt_checks_enabled();

// Enable or disable tag propagation
void tt_set_checks(int enabled);

// TODO [TAG]: Need hint instruction. Necessary?
// int tt_prop_enabled();

// Enable or disable tag propagation
void tt_set_prop(int enabled);

typetag_t tt_get_tag(char* ptr);
void tt_set_tag(char* ptr, typetag_t tag);

#endif // TYPE_TAG_H