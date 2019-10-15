#include "pmsis.h"
#include "pmsis/rtos/os_frontend_api/pmsis_task.h"
#include "pmsis/rtos/malloc/pmsis_l1_malloc.h"

/*******************************************************************************
 * Definitions
 ******************************************************************************/

/*******************************************************************************
 * Driver data
 *****************************************************************************/

malloc_t __l1_malloc;
pmsis_mutex_t __l1_malloc_mutex;

/*******************************************************************************
 * API implementation
 ******************************************************************************/

void *pi_l1_malloc(struct pi_device *device, uint32_t size)
{
    void *ret_ptr;
    ret_ptr = __malloc(&__l1_malloc, size);
    return ret_ptr;
}

void pi_l1_free(struct pi_device *device, void *_chunk, int size)
{
    __malloc_free(&__l1_malloc, _chunk, size);
}

void *pi_l1_malloc_align(struct pi_device *device, int size, int align)
{
    void *ret_ptr;
    ret_ptr = __malloc_align(&__l1_malloc, size, align);
    return ret_ptr;
}

/** \brief malloc init: initialize mutex and heap
 * Warning: thread unsafe by nature
 * \param heapstart heap start address
 * \param heap_size size of the heap
 */
void pmsis_l1_malloc_init(void *heapstart, uint32_t heap_size)
{
    __malloc_init(&__l1_malloc, heapstart, heap_size);
}

void pmsis_l1_malloc_set_malloc_t(malloc_t malloc_struct)
{
    memcpy(&__l1_malloc,&malloc_struct,sizeof(malloc_t));
}

malloc_t pmsis_l1_malloc_get_malloc_t(void)
{
    malloc_t malloc_struct;
    memcpy(&malloc_struct,&__l1_malloc,sizeof(malloc_t));
    return malloc_struct;
}
