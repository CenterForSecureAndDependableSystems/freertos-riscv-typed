/*
 * FreeRTOS V202212.00
 * Copyright (C) 2020 Amazon.com, Inc. or its affiliates. All Rights Reserved.
 *
 * Permission is hereby granted, free of charge, to any person obtaining a copy of
 * this software and associated documentation files (the "Software"), to deal in
 * the Software without restriction, including without limitation the rights to
 * use, copy, modify, merge, publish, distribute, sublicense, and/or sell copies of
 * the Software, and to permit persons to whom the Software is furnished to do so,
 * subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included in all
 * copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY, FITNESS
 * FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR
 * COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER
 * IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN
 * CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.
 *
 * https://www.FreeRTOS.org
 * https://github.com/FreeRTOS
 *
 */

/* FreeRTOS kernel includes. */
#include "riscv-virt.h"
#include <FreeRTOS.h>
#include <task.h>
#include <stdio.h>
#include <typetag/control.h>
#include <typetag/typetag.h>

// Including C files is evil, but we need these hook functions to be in the
// same compilation unit as main or weird things happen.
#include "riscv-hooks.c"
#include "typetag/exception.h"

/*
 * Setup the Spike simulator to run this demo.
 */
static void prvSetupSpike( void );

int test_tag_get_set() {
    tt_set_prop(1);

    int val1 = 10;
    typetag_t tag1 = 123;
    typetag_t tag2 = 0;
    tt_set_tag((char*)&val1, tag1);
    tag2 = tt_get_tag((char*)&val1);

    tt_set_prop(0);

    char buf[256];
    sprintf(buf, "  Wrote: %d; Got: %d", tag1, tag2);
    vSendString(buf); 
    return tag1 == tag2;
}

int test_tag_propagation() {
    tt_set_prop(1);

    int val1 = 10;
    typetag_t tag1 = 123;
    tt_set_tag((char*)&val1, tag1);

    int val2 = val1;
    typetag_t tag2 = tt_get_tag((char*)&val2);

    tt_set_prop(0);

    char buf[256];
    sprintf(buf, "  Wrote: %d; Got: %d", tag1, tag2);
    vSendString(buf); 
    return tag1 == tag2;
}

int return_validate_good() {
    return 8;
}

void return_validate_evil() {
    // Overwrite the return address
    __asm__ volatile ("lw t0, 12(sp)");
    __asm__ volatile ("addi t0, t0, 8");
    __asm__ volatile ("sw t0, 12(sp)");
    return_validate_good();
}

int test_return() {
    int i = 0;
    tt_set_prop(1);
    tt_set_exception(TT_EXP_INVALID_RETURN_TAG, TRAP_WARN);
    tt_set_checks(1);
    // vSendString("Should have warning:");
    return_validate_evil();
    i = 1;

    // vSendString("Should have no warning:");
    return_validate_good();
    tt_set_prop(0);
    tt_set_checks(0);

    char buf[64];
    sprintf(buf, "  Expected: 1. Got: %d", i);
    vSendString(buf);
    return i == 1;
}

int run_test(const char* test_name, int (*test_func)()) {
    char buf[256];
    sprintf(buf, "Running Test [%s] ---", test_name);
    vSendString( buf ); 

    int result;
    if((result = test_func())) {
        sprintf(buf, "Test [%s]: Pass\n", test_name);
        vSendString( buf ); 
    }
    else {
        sprintf(buf, "\e[1;31mTest [%s]: Fail\nStopping.\e[0m\n", test_name);
        vSendString( buf );
        // assert(0); // Quit early for now
    }

    // Reset tag flags
    tt_set_prop(0);
    tt_set_checks(0);

    return result;
}

/*-----------------------------------------------------------*/

int main( void )
{
    // Reset without making function calls so we
    // can try to recover debug sessions if we restart
    // the program in the middle of debugging.
    __asm__ volatile ("slti x0, x0, 0");
    __asm__ volatile ("slti x0, x0, 2");
    int tmp = TT_EXP_INVALID_RETURN_TAG;
    __asm__ volatile (
        "sltiu x0, %0, %c1"
        : /* No outputs */
        : "r" (tmp), "i" (0)
    );

    prvSetupSpike();

    run_test("Get/Set", &test_tag_get_set);
    run_test("Basic Propagation", &test_tag_propagation);
    // run_test("Return Addr", &test_return);

    vSendString("Done."); 

    return 0;
}

/*-----------------------------------------------------------*/