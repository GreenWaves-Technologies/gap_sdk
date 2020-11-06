/* sprintf.c */

/*
 * Copyright (c) 1997-2010, 2013-2014 Wind River Systems, Inc.
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#include <stdarg.h>
#include <stdio.h>
#include <stddef.h>

extern int pos_libc_prf_locked(int (*func)(), void *dest,
                const char *format, va_list vargs);

struct emitter {
    char *ptr;
    int len;
};

static int sprintf_out(int c, struct emitter *p)
{
    if (p->len > 1) { /* need to reserve a byte for EOS */
        *(p->ptr) = c;
        p->ptr += 1;
        p->len -= 1;
    }
    return 0; /* indicate keep going so we get the total count */
}

int snprintf(char *s, size_t len,
         const char *format, ...)
{
    va_list vargs;

    struct emitter p;
    int     r;
    char    dummy;

    if (len == 0) {
        s = &dummy; /* write final NUL to dummy, can't change *s */
    }

    p.ptr = s;
    p.len = (int) len;

    va_start(vargs, format);
    r = pos_libc_prf_locked(sprintf_out, (void *) (&p), format, vargs);
    va_end(vargs);

    *(p.ptr) = 0;
    return r;
}

int sprintf(char *s, const char *format, ...)
{
    va_list vargs;

    struct emitter p;
    int     r;

    p.ptr = s;
    p.len = (int) 0x7fffffff; /* allow up to "maxint" characters */

    va_start(vargs, format);
    r = pos_libc_prf_locked(sprintf_out, (void *) (&p), format, vargs);
    va_end(vargs);

    *(p.ptr) = 0;
    return r;
}

int vsnprintf(char *s, size_t len,
          const char *format, va_list vargs)
{
    struct emitter p;
    int     r;
    char    dummy;

    if (len == 0) {
        s = &dummy; /* write final NUL to dummy, can't change * *s */
    }

    p.ptr = s;
    p.len = (int) len;

    r = pos_libc_prf_locked(sprintf_out, (void *) (&p), format, vargs);

    *(p.ptr) = 0;
    return r;
}

int vsprintf(char *s, const char *format,
         va_list vargs)
{
    struct emitter p;
    int     r;

    p.ptr = s;
    p.len = (int) 0x7fffffff; /* allow up to "maxint" characters */

    r = pos_libc_prf_locked(sprintf_out, (void *) (&p), format, vargs);

    *(p.ptr) = 0;
    return r;
}
