#include <stdlib.h>
#include <stdio.h>
#include "../stdio/tinyprintf/tinyprintf.h"

static void tfa_error_vfprintf(const char * format, va_list arg) {
    char buffer[128];
    int size = vsnprintf(buffer, 128, format, arg);
    if (size > 0) {
        buffer[size] = 0;
        puts(buffer);
    }
}

static void tfa_error_printf(const char* format, ...) {
    va_list arg;
    va_start(arg, format);
    tfa_error_vfprintf(format, arg);
    va_end(arg);
}

void __assert_func (const char *file, int line, const char *func, const char *expr)
{
    tfa_error_printf("Assert failed: %s, file: %s, line %d \n", expr, file, line);
    exit(-1);
}
