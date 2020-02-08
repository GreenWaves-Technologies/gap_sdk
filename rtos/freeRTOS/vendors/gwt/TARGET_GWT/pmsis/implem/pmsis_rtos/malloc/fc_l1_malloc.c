#include "pmsis.h"
#include "pmsis/rtos/os_frontend_api/os.h"
#include "pmsis/rtos/malloc/fc_l1_malloc.h"

/*******************************************************************************
 * Definitions
 ******************************************************************************/

/*******************************************************************************
 * Driver data
 *****************************************************************************/

#if (__FC_MALLOC_NATIVE__ == 0)
malloc_t __fc_l1_malloc;
pmsis_spinlock_t __fc_l1_malloc_spinlock;

/*******************************************************************************
 * API implementation
 ******************************************************************************/

void *pi_fc_l1_malloc(int size)
{
    void *ret_ptr;
    int irq = disable_irq();
    ret_ptr = __malloc(&__fc_l1_malloc, size);
    restore_irq(irq);
    return ret_ptr;
}

void pi_fc_l1_free(void *_chunk, int size)
{
    int irq = disable_irq();
    __malloc_free(&__fc_l1_malloc, _chunk, size);
    restore_irq(irq);
}

void *pi_fc_l1_malloc_align(int size, int align)
{
    void *ret_ptr;
    pmsis_spinlock_take(&__fc_l1_malloc_spinlock);
    ret_ptr = __malloc_align(&__fc_l1_malloc, size, align);
    pmsis_spinlock_release(&__fc_l1_malloc_spinlock);
    return ret_ptr;
}

void pi_fc_l1_malloc_init(void *heapstart, uint32_t heap_size)
{
    __malloc_init(&__fc_l1_malloc, heapstart, heap_size);
    pmsis_spinlock_init(&__fc_l1_malloc_spinlock);
}

void pi_fc_l1_malloc_struct_set(malloc_t malloc_struct)
{
    pmsis_spinlock_take(&__fc_l1_malloc_spinlock);
    memcpy(&__fc_l1_malloc, &malloc_struct, sizeof(malloc_t));
    pmsis_spinlock_release(&__fc_l1_malloc_spinlock);
}

malloc_t pi_fc_l1_malloc_struct_get(void)
{
    malloc_t malloc_struct;
    pmsis_spinlock_take(&__fc_l1_malloc_spinlock);
    memcpy(&malloc_struct,&__fc_l1_malloc, sizeof(malloc_t));
    pmsis_spinlock_release(&__fc_l1_malloc_spinlock);
    return malloc_struct;
}

#endif  /* (__FC_MALLOC_NATIVE__ == 0) */
