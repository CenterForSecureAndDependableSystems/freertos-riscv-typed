// Implementation of control.h functions for riscv

#include "typetag/control.h"
#include "typetag/typetag.h"
#include <stdint.h>

// int tt_checks_enabled() { }

void tt_set_checks(int enabled) {
	if(enabled == 0) {
		__asm__ volatile ("slti x0, x0, 2");
	}
	else {
		__asm__ volatile ("slti x0, x0, 3");
	}
}

void tt_set_exception(tagexception_t exception, int enabled) {
	if(enabled == 1) {
		__asm__ volatile (
			"sltiu x0, %0, %c1"
			: /* No outputs */
			: "r" (exception), "i" (1)
		);
	}
	else {
		__asm__ volatile (
			"sltiu x0, %0, %c1"
			: /* No outputs */
			: "r" (exception), "i" (0)
		);
	}
}

// int tt_prop_enabled() { }

void tt_set_prop(int enabled) {
	if(enabled == 0) {
		__asm__ volatile ("slti x0, x0, 0");
	}
	else {
		__asm__ volatile ("slti x0, x0, 1");
	}
}

typetag_t tt_get_tag(char* ptr) {
	typetag_t tag; 
	__asm__ volatile (
		"sll x0, %1, %0"
		: "=r" (tag)
		: "r" (ptr)
	);

	return tag;
}

void tt_set_tag(char* ptr, typetag_t tag) {
	__asm__ volatile (
		"slt x0, %1, %0"
		: /* No outputs */
		: "r" (tag), "r" (ptr)
	);
}