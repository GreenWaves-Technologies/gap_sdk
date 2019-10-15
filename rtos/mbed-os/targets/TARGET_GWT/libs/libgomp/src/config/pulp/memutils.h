/* Copyright 2014 DEI - Universita' di Bologna
   author       Alessandro Capotondi - alessandro.capotondi@unibo.it
   info         PULP memory allocation */

#ifndef __MEMUTILS_H__
#define __MEMUTILS_H__

#include <stdint.h>
#include <malloc.h>

static inline void *
shmalloc(uint32_t size)
{
    return (void*)L1_Malloc(size);
}

static inline void
shfree(void *address)
{
    L1_Free(address);
}


#endif
