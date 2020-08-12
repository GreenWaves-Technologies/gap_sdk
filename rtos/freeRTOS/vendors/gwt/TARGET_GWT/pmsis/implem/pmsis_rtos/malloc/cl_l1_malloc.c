#include "pmsis.h"
#include "pmsis/rtos/os_frontend_api/os.h"
#include "pmsis/rtos/malloc/cl_l1_malloc.h"

#if defined(FEATURE_CLUSTER)

/*******************************************************************************
 * Definitions
 ******************************************************************************/

/*******************************************************************************
 * Driver data
 *****************************************************************************/

malloc_t __cl_l1_malloc;
pmsis_mutex_t __cl_l1_malloc_mutex;

/*******************************************************************************
 * API implementation
 ******************************************************************************/

void *pi_cl_l1_malloc(struct pi_device *device, uint32_t size)
{
    void *ret_ptr;
    uint32_t irq = __disable_irq();
    ret_ptr = __malloc(&__cl_l1_malloc, size);
    __restore_irq(irq);
    return ret_ptr;
}

void pi_cl_l1_free(struct pi_device *device, void *_chunk, int size)
{
    uint32_t irq = __disable_irq();
    __malloc_free(&__cl_l1_malloc, _chunk, size);
    __restore_irq(irq);
}

void *pi_cl_l1_malloc_align(struct pi_device *device, int size, int align)
{
    void *ret_ptr;
    ret_ptr = __malloc_align(&__cl_l1_malloc, size, align);
    return ret_ptr;
}

void pi_cl_l1_malloc_init(void *heapstart, uint32_t heap_size)
{
    __malloc_init(&__cl_l1_malloc, heapstart, heap_size);
}

void pi_cl_l1_malloc_struct_set(malloc_t malloc_struct)
{
    memcpy(&__cl_l1_malloc, &malloc_struct, sizeof(malloc_t));
}

malloc_t pi_cl_l1_malloc_struct_get(void)
{
    malloc_t malloc_struct;
    memcpy(&malloc_struct, &__cl_l1_malloc, sizeof(malloc_t));
    return malloc_struct;
}

#endif  /* FEATURE_CLUSTER */
