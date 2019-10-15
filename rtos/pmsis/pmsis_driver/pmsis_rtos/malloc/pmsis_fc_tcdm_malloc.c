#include "pmsis.h"
#include "pmsis/rtos/os_frontend_api/pmsis_task.h"
#include "pmsis/rtos/malloc/pmsis_fc_tcdm_malloc.h"

/*******************************************************************************
 * Definitions
 ******************************************************************************/

/*******************************************************************************
 * Driver data
 *****************************************************************************/

#if (__FC_MALLOC_NATIVE__ == 0)
malloc_t __fc_tcdm_malloc;
pmsis_spinlock_t __fc_tcdm_malloc_spinlock;

/*******************************************************************************
 * API implementation
 ******************************************************************************/

void *pi_fc_tcdm_malloc(uint32_t size)
{
    void *ret_ptr;
    int irq = disable_irq();
    ret_ptr = __malloc(&__fc_tcdm_malloc, size);
    restore_irq(irq);
    return ret_ptr;
}

void pi_fc_tcdm_free(void *_chunk, int size)
{
    int irq = disable_irq();
    __malloc_free(&__fc_tcdm_malloc, _chunk, size);
    restore_irq(irq);
}

void *pi_fc_tcdm_malloc_align(int size, int align)
{
    void *ret_ptr;
    pmsis_spinlock_take(&__fc_tcdm_malloc_spinlock);
    ret_ptr = __malloc_align(&__fc_tcdm_malloc, size, align);
    pmsis_spinlock_release(&__fc_tcdm_malloc_spinlock);
    return ret_ptr;
}

/** \brief malloc init: initialize mutex and heap
 * Warning: thread unsafe by nature
 * \param heapstart heap start address
 * \param heap_size size of the heap
 */
void pmsis_fc_tcdm_malloc_init(void *heapstart, uint32_t heap_size)
{
    __malloc_init(&__fc_tcdm_malloc, heapstart, heap_size);
    pmsis_spinlock_init(&__fc_tcdm_malloc_spinlock);
}

void pmsis_fc_tcdm_malloc_set_malloc_struct(malloc_t malloc_struct)
{
    pmsis_spinlock_take(&__fc_tcdm_malloc_spinlock);
    memcpy(&__fc_tcdm_malloc,&malloc_struct, sizeof(malloc_t));
    pmsis_spinlock_release(&__fc_tcdm_malloc_spinlock);
}

malloc_t pmsis_fc_tcdm_malloc_get_malloc_struct(void)
{
    malloc_t malloc_struct;
    pmsis_spinlock_take(&__fc_tcdm_malloc_spinlock);
    memcpy(&malloc_struct,&__fc_tcdm_malloc,sizeof(malloc_t));   
    pmsis_spinlock_release(&__fc_tcdm_malloc_spinlock);
    return malloc_struct;
}
#endif
