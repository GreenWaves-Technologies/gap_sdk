#include <stdlib.h>
#include <string.h>
#include "generic.h"
#include "generic_hog.h"

/* The current type used for the malloc*/
vl_mem_type VL_MEM_TYPE = -1;


void * vl_calloc (size_t n, size_t size) {

    void *addr = vl_malloc(n*size);
    return (void*) memset(addr, 0, n*size);
}

void vl_set_mem_type(vl_mem_type type) {

    VL_MEM_TYPE = type ;
}


void * vl_malloc (size_t n) {

    if (VL_MEM_TYPE == L1)
        return (void*) L1_Malloc(n);
    else
        return (void*) malloc(n);
}


void vl_free (void *ptr) {

    if (VL_MEM_TYPE == L1)
        L1_Free(ptr);
    else
        free(ptr) ;
}
