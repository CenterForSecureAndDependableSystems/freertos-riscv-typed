#ifndef TYPETAG_RISCV_H
#define TYPETAG_RISCV_H

#include "typetag/typetag.h"

// Enable or disable a particular tagging exception
// when tt checks or propagation is enabled
void tt_set_exception(TagException exception, int enabled);

// TODO TAG: Need hint instruction. Necessary?
// int tt_checks_enabled();

// Enable or disable tag propagation
void tt_set_checks(int enabled);

// TODO TAG: Need hint instruction. Necessary?
// int tt_prop_enabled();

// Enable or disable tag propagation
void tt_set_prop(int enabled);

TypeTag tt_get_tag(void* ptr);
void tt_set_tag(void* ptr, TypeTag tag);

#endif // TYPE_TAG_H