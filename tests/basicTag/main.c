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
#include <FreeRTOS.h>
#include <task.h>
#include <stdio.h>
#include <typetag/control.h>
#include <typetag/typetag.h>

void vApplicationStackOverflowHook( TaskHandle_t pxTask,
                                    char * pcTaskName );
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
    volatile int _tmp1 = 5; // Try to mess with the stack a little
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

int run_test(const char* test_name, int (*test_func)()) {
    char buf[256];
    sprintf(buf, "Running Test [%s] ---", test_name);
    vSendString( buf ); 

    if(test_func() == 0) {
        sprintf(buf, "Test [%s]: Fail\n", test_name);
        vSendString( buf ); 
        return 0;
    }
    else {
        sprintf(buf, "Test [%s]: Pass\n", test_name);
        vSendString( buf ); 
        return 1;
    }
}

/*-----------------------------------------------------------*/

int main( void )
{
    prvSetupSpike();

    run_test("Get/Set", &test_tag_get_set);
    run_test("Basic Propagate", &test_tag_propagation);

    return 0;
}
/*-----------------------------------------------------------*/
extern void freertos_risc_v_trap_handler( void );
static void prvSetupSpike( void )
{
    __asm__ volatile ( "csrw mtvec, %0" : : "r" ( freertos_risc_v_trap_handler ) );
}
