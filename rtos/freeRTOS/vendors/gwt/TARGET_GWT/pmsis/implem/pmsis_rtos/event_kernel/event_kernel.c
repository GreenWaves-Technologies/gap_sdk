#if 1
#include <stdlib.h>
#include <stdio.h>

#include "pmsis.h"
#include "pmsis/task.h"
#include "pmsis/rtos/os_frontend_api/os.h"
#include "pmsis/rtos/event_kernel/event_kernel.h"
#include "pmsis/rtos/malloc/cl_l1_malloc.h"
#include "pmsis/cluster/cluster_sync/cl_synchronisation.h"
#include "pmsis/cluster/cluster_sync/fc_to_cl_delegate.h"
#include "cores/TARGET_RISCV_32/core_utils.h"
#include "pmsis/implem/hal/hal.h"

//#define DEBUG
#ifndef DEBUG
#define DBG_PRINTF(...) ((void)0)
#else
#define DBG_PRINTF printf
#endif

struct pmsis_event_kernel_wrap *default_sched;

/** Internal structs for the event kernel **/
/*typedef struct pmsis_event {
    struct pmsis_event *next;
    struct pmsis_event_scheduler *sched;
    pi_task_t *fc_task;
} pmsis_event_t;*/

/**
 * event scheduler structure:
 * give a fifo for active events and a list of free events which can be used
 * by event push at any time
 **/
typedef struct pmsis_event_scheduler {
    struct pi_task *first;
    struct pi_task *last;
} pmsis_event_scheduler_t;

struct pmsis_event_kernel
{
    spinlock_t cl_to_fc_spinlock;
    pi_sem_t event_sched_sem;
    struct pmsis_event_scheduler *scheduler;
    int running;
};

static inline int pmsis_event_lock_cl_to_fc(struct pmsis_event_kernel *evt_kernel)
{
    int irq = 0;
    irq = disable_irq();
    return irq;
}

static inline void pmsis_event_unlock_cl_to_fc(struct pmsis_event_kernel *evt_kernel,
        int irq_prev)
{
    restore_irq(irq_prev);
}

static inline struct pmsis_event_kernel *pmsis_event_wrap_get_kernel(
        struct pmsis_event_kernel_wrap *wrap)
{
    struct pmsis_event_kernel *kern = (struct pmsis_event_kernel*)wrap->priv;
    return kern;
}


void pmsis_event_lock_cl_to_fc_init(struct pmsis_event_kernel_wrap *wrap)
{
    struct pmsis_event_kernel *evt_kernel = pmsis_event_wrap_get_kernel(wrap);
    cl_sync_init_spinlock(&(evt_kernel->cl_to_fc_spinlock), pmsis_l1_malloc(sizeof(uint32_t)));
    hal_compiler_barrier();
}

static inline void pmsis_event_wrap_set_kernel(
        struct pmsis_event_kernel_wrap *wrap,
        struct pmsis_event_kernel *event_kernel)
{
    wrap->priv = (void*)event_kernel;
}

static inline struct pmsis_event_scheduler *pmsis_event_wrap_get_scheduler(
        struct pmsis_event_kernel_wrap *wrap)
{
    struct pmsis_event_kernel *kern = pmsis_event_wrap_get_kernel(wrap);
    return kern->scheduler;
}

static inline void pmsis_event_wrap_set_scheduler(
        struct pmsis_event_kernel_wrap *wrap,
        struct pmsis_event_scheduler *sched)
{
    struct pmsis_event_kernel *kern = pmsis_event_wrap_get_kernel(wrap);
    kern->scheduler = sched;
}

void pmsis_event_kernel_mutex_release(struct pmsis_event_kernel_wrap *wrap)
{
    struct pmsis_event_kernel *kern = pmsis_event_wrap_get_kernel(wrap);
    pi_sem_t *sched_sem = &(kern->event_sched_sem);
    pi_sem_give(sched_sem);
}

/**
 * Release an active event from the FIFO list
 * and push it on the free list
 * if allocated_event_nb > max_event_nb the event will be freed instead
 */
static inline void pmsis_event_release(struct pi_task *event);

/**
 * pop an event from the event FIFO, returns NULL if none available
 * MAY SLEEP
 */
static inline void pmsis_event_pop(struct pmsis_event_kernel *event_kernel,
        struct pi_task **event);

/**
 * Get an event from the scheduler free list
 * Returns NULL if none available
 */

/**** Actual implementations ****/

static inline void pmsis_event_release(struct pi_task *event)
{
    DBG_PRINTF("pmsis_event_release: event_ptr is %p\n",event);
    int irq = disable_irq();
    pi_task_release(event);
    restore_irq(irq);
}

static inline void pmsis_event_pop(struct pmsis_event_kernel *event_kernel,
        struct pi_task **event)
{
    // Cluster will have to go through IRQ, so locking irq is the easiest/most efficient synchro
    struct pmsis_event_scheduler *sched = event_kernel->scheduler;
    pi_sem_take(&event_kernel->event_sched_sem);
    
    DBG_PRINTF("scheduler %p took sem %p\n",event_kernel, event_kernel->event_sched_sem.sem_object);
    int irq = __disable_irq();
    *event = sched->first;
    sched->first = sched->first->next;
    if(sched->first == NULL)
    {// if we're at the end, reset last also
        sched->last=NULL;
    }
    __restore_irq(irq);
}

/** Might be called from cluster FC code, or ISR **/
int pmsis_event_push(struct pmsis_event_kernel_wrap *wrap, pi_task_t *event)
{
    int irq = __disable_irq();
    struct pmsis_event_kernel *event_kernel = pmsis_event_wrap_get_kernel(wrap);
    // Critical section, we are either on fc or in irq
    struct pmsis_event_scheduler* sched = pmsis_event_wrap_get_scheduler(wrap);
    //printf("pushing event: %p in scheduler %p\n",event,event_kernel);

    event->next = NULL;

    if(sched->last)
    {
        sched->last->next = event;
        sched->last = event;
    }
    else
    {
        sched->last = event;
        sched->first = event;
    }

    // restore irqs
    // signal event kernel that a task is available
    //printf("scheduler %p gives sem %p\n",event_kernel, event_kernel->event_sched_sem.sem_object);
    pi_sem_give(&event_kernel->event_sched_sem);
    DBG_PRINTF("%s:%d\n",__func__,__LINE__);
    restore_irq(irq);
    DBG_PRINTF("%s:%d\n",__func__,__LINE__);
    return 0;
}

static char g_event_kernel_name_s[] = "event_kernel";
/**
 * Prepare the event kernel structure and task
 * In particular, create inner private structure
 * And setup synchronization mutexes
 */
int pmsis_event_kernel_init(struct pmsis_event_kernel_wrap **wrap,
        void (*event_kernel_entry)(void*))
{
    *wrap = pi_default_malloc(sizeof(struct pmsis_event_kernel_wrap));
    struct pmsis_event_kernel_wrap *event_kernel_wrap = *wrap;
    if(!event_kernel_wrap)
    {
        return -1;
    }

    // allocate private struct and priv pointer to it
    struct pmsis_event_kernel *priv = pi_default_malloc(sizeof(struct pmsis_event_kernel));
    event_kernel_wrap->priv = priv;
    if(!priv)
    {
        return -1;
    }

    struct pmsis_event_scheduler *sched = pi_default_malloc(sizeof(struct pmsis_event_scheduler));
    if(!sched)
    {
        return -1;
    }

    sched->first=NULL;
    sched->last=NULL;

    pmsis_event_wrap_set_scheduler(event_kernel_wrap, sched);
    // TODO: check name for native handle
    event_kernel_wrap->__os_native_task = pmsis_task_create(event_kernel_entry,
            event_kernel_wrap,
            g_event_kernel_name_s,
            PMSIS_TASK_EVENT_KERNEL_PRIORITY);

    if(!event_kernel_wrap->__os_native_task)
    {
        printf("Error: task is not correctly initialized\n");
        return -1;
    }
    return 0;
}

static void pmsis_event_kernel_exec_event(struct pmsis_event_kernel *kernel,
        struct pi_task *event)
{
    switch(event->id)
    {
        case PI_TASK_CALLBACK_ID :
            {
                // release event before exec, to match pulpos behaviour
                // This should however not be abused.
                // In particular the user must ensure the potential waiting thread is using an OS
                // provided mechanism to synchronize (mutex/semaphore/signals) and avoid "yield loops"
                pmsis_event_release(event);
                event->done = 1;
                callback_t callback_func = (callback_t)event->arg[0];
                callback_func((void*)event->arg[1]);
            }
            break;
        default: // unimplemented or mutex only -- just release the event
            pmsis_event_release(event);
            break;
    }
}

void pmsis_event_kernel_main(void *arg)
{
    struct pmsis_event_kernel_wrap *wrap = (struct pmsis_event_kernel_wrap*)arg;
    struct pmsis_event_kernel *event_kernel = pmsis_event_wrap_get_kernel(wrap);
    // finally, initialize the mutex we'll use
    if(pi_sem_init(&event_kernel->event_sched_sem))
    {
        printf("EVENT_KERNEL: can't init mutex\n");
        return;
    }
    event_kernel->running = 1;

    pi_task_t *event;
    while(event_kernel->running)
    {
        pmsis_event_pop(event_kernel,&event);
        //printf("event kernel %p poped event %p, is_running? %x\n",event_kernel, event, event_kernel->running);
        if(event)
        {
            pmsis_event_kernel_exec_event(event_kernel,event);
        }
    }
}

void pmsis_event_kernel_destroy(struct pmsis_event_kernel_wrap **wrap)
{
    struct pmsis_event_kernel *event_kernel = pmsis_event_wrap_get_kernel(*wrap);
    DBG_PRINTF("destroying scheduler %p\n",event_kernel);
    int irq = disable_irq();
    event_kernel->running = 0;
    pmsis_task_suspend((*wrap)->__os_native_task);
    pi_sem_deinit(&event_kernel->event_sched_sem);
    // ----------------- freeing whatever was dyn alloc'd
    pi_default_free(pmsis_event_wrap_get_scheduler(*wrap),sizeof(struct pmsis_event_scheduler));
    pi_default_free((*wrap)->priv, sizeof(struct pmsis_event_kernel));
    pi_default_free(*wrap,sizeof(struct pmsis_event_kernel_wrap));
    restore_irq(irq);
}

struct pmsis_event_kernel_wrap *pmsis_event_get_default_scheduler(void)
{
    DBG_PRINTF("%s:%d default_sched:%p\n",__func__,__LINE__,default_sched);
    return default_sched;
}

void pmsis_event_set_default_scheduler(struct pmsis_event_kernel_wrap *wrap)
{
    default_sched = wrap;
    DBG_PRINTF("%s:%d default_sched:%p\n",__func__,__LINE__,default_sched);
}

void pmsis_event_destroy_default_scheduler(struct pmsis_event_kernel_wrap *wrap)
{
    DBG_PRINTF("%s:%d\n",__func__,__LINE__);
    pmsis_event_kernel_destroy(&wrap);
}
#endif
