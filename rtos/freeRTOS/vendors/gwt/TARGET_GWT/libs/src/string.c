/*
 * Copyright (c) 2019, GreenWaves Technologies, Inc.
 * All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without modification,
 * are permitted provided that the following conditions are met:
 *
 * o Redistributions of source code must retain the above copyright notice, this list
 *   of conditions and the following disclaimer.
 *
 * o Redistributions in binary form must reproduce the above copyright notice, this
 *   list of conditions and the following disclaimer in the documentation and/or
 *   other materials provided with the distribution.
 *
 * o Neither the name of GreenWaves Technologies, Inc. nor the names of its
 *   contributors may be used to endorse or promote products derived from this
 *   software without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND
 * ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED
 * WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
 * DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE FOR
 * ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES
 * (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES;
 * LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON
 * ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
 * (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS
 * SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 */

#include "string.h"

int32_t memcmp(const void *str1, const void *str2, size_t n)
{
    uint8_t *s1 = (uint8_t *) str1;
    uint8_t *s2 = (uint8_t *) str2;
    for (uint32_t i=0; i<n; i++)
    {
        if (*s1 != *s2)
        {
            return *s1 - *s2;
        }
        s1++;
        s2++;
    }
    return 0;
}

void *memcpy(void *str1, const void *str2, size_t n)
{
    uint8_t *s1 = (uint8_t *) str1;
    uint8_t *s2 = (uint8_t *) str2;
    for (uint32_t i=0; i<n; i++)
    {
        *(s1 + i) = *(s2 + i);
    }
    return (void *) str1;
}

void *memset(void *str, int32_t c, size_t n)
{
    uint8_t *s = (uint8_t *) str;
    for (uint32_t i=0; i<n; i++)
    {
        *(s + i) = c;
    }
    return (void *) str;
}

int32_t strcmp(const char *str1, const char *str2)
{
    uint8_t *s1 = (uint8_t *) str1;
    uint8_t *s2 = (uint8_t *) str2;
    for (; (*s1 != '\0') && (*s1 == *s2);)
    {
        s1++;
        s2++;
    }
    return (*s1 - *s2);
}

int strncmp(const char *str1, const char *str2, size_t n)
{
    uint8_t *s1 = (uint8_t *) str1;
    uint8_t *s2 = (uint8_t *) str2;

    for (; (n > 0) && (*s1 != '\0') && (*s1 == *s2); n--)
    {
        s1++;
        s2++;
    }
    if (!n)
    {
        return 0;
    }
    else
    {
        return (*s1 - *s2);
    }
}

int str_starts_with(const char *str1, const char *str2)
{
    return (strncmp(str1, str2, strlen(str2)) == 0);
}

int str_ends_with(const char *str1, const char *str2)
{
    uint8_t *s1 = (uint8_t *) str1;
    uint8_t *s2 = (uint8_t *) str2;
    int32_t size_s1 = strlen(str1);
    int32_t size_s2 = strlen(str2);
    if (size_s1 < size_s2)
    {
        return -1;
    }
    s1 = s1 + sizeof(uint8_t) * size_s1 - 1;
    s2 = s2 + sizeof(uint8_t) * size_s2 - 1;
    while (size_s2 && (*s1 == *s2))
    {
        size_s2--;
        s1--;
        s2--;
    }
    if (!size_s2)
    {
        return 1;
    }
    else
    {
        return 0;
    }

}

size_t strlen(const char *str)
{
    uint8_t *s1 = (uint8_t *) str;
    uint32_t len = 0;
    for (len = 0; *s1 != '\0'; len++)
    {
        s1++;
    }
    return len;
}

char *strcat(char *str1, const char *str2)
{
    char *ptr = str1 + strlen(str1);

    for (; *str2 != '\0';)
    {
        *ptr++ = *str2++;
    }
    *ptr = '\0';

    return str1;
}

char *strcpy(char *str1, const char *str2)
{
    char *ptr = str1;
    for (; *str2 != '\0';)
    {
        *ptr++ = *str2++;
    }
    *ptr = '\0';

    return str1;
}

size_t strspn( const char * s1, const char * s2 )
{
    size_t len = 0;
    const char * p;
    while ( s1[ len ] )
    {
        p = s2;
        while ( *p )
        {
            if ( s1[len] == *p )
            {
                break;
            }
            ++p;
        }
        if ( ! *p )
        {
            return len;
        }
        ++len;
    }
    return len;
}

size_t strcspn( const char * s1, const char * s2 )
{
    size_t len = 0;
    const char * p;
    while ( s1[len] )
    {
        p = s2;
        while ( *p )
        {
            if ( s1[len] == *p++ )
            {
                return len;
            }
        }
        ++len;
    }
    return len;
}

char * strchr( const char * s, int c )
{
    do
    {
        if ( *s == (char) c )
        {
            return (char *) s;
        }
    } while ( *s++ );
    return NULL;
}

