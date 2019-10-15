/*
 * C library entry point. Expects a normal C environment (valid stack)
 * then goes on and initialises newlib stuff, then calls main.
 *
 * This is a drastically cut down equivalent of newlib's normal entry point.
 *
 *
 * Copyright 2008 Torne Wuff
 *
 * This file is part of Pycorn.
 *
 * Pycorn is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 */

#include <stdlib.h>
#include <string.h>

// Linker provided symbols
extern char __bss_start__ asm ("__bss_start__");
extern char __bss_end__ asm ("__bss_end__");

extern void __libc_init_array(void);
extern void __libc_fini_array(void);

extern void software_init_hook (void);
extern int __wrap_atexit(void (*func)());
extern int __wrap_main(void);
extern int __wrap_exit(void);

// Declare that we don't return.
void _mainCRTStartup(void) __attribute__((noreturn));

void _mainCRTStartup(void)
{
    // Clear .bss
    /* size_t bss_size = &__bss_end__ - &__bss_start__; */
    /* memset(&__bss_start__, 0, bss_size); */

    // software_init_hook
    software_init_hook();

    // register destructors to be called at exit time
    __wrap_atexit((void*) 0);

    // call constructors
    __libc_init_array();

    asm volatile("mv a0, x0");
    asm volatile("mv a1, x0");

    // call main
    __wrap_main();

    // exit, returning whatever main returns. We have no args.
    __wrap_exit();
}

// Define an empty _init and _fini since arm-eabi-gcc never generates
// code into them, but newlib expects them to exist. Silly newlib.
void _init(void)
{
}

void _fini(void)
{
}
