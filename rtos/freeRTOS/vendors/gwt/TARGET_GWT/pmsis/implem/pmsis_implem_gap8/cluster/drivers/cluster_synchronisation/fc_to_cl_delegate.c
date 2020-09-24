#if 1
#include "string.h"

#include "pmsis.h"
#include "pmsis/task.h"
#include "pmsis/rtos/os_frontend_api/os.h"
#include "pmsis/rtos/event_kernel/event_kernel.h"
#include "pmsis/rtos/malloc/cl_l1_malloc.h"
#include "pmsis/cluster/cluster_sync/fc_to_cl_delegate.h"
#include "pmsis/cluster/cluster_sync/cl_to_fc_delegate.h"
#include "pmsis/cluster/cluster_sync/cl_synchronisation.h"
#include "pmsis/cluster/cluster_team/cl_team.h"
#include "pmsis/implem/hal/hal.h"
//#include "pmsis/implem/hal/gap_eu/pmsis_eu.h"
#include "cores/TARGET_RISCV_32/core_utils.h"
#include "cores/TARGET_RISCV_32/core_gap.h"
//#include "pmsis/implem/drivers/pmu/pmsis_pmu.h"

#define NB_CLUSTER                 ( 1 )
#define CL_MASTER_CORE_STACK_SIZE  ( 0x800 ) /*!< Stack size for Cluster Master core, 2kB. */
#define CL_SLAVE_CORE_STACK_SIZE   ( 0x400 ) /*!< Stack size for Cluster Slave cores, 1kB. */

#ifdef DEBUG
#define PRINTF printf
#else
#define PRINTF(...) ((void) 0)
#endif

// spinlock left at the disposal of the os to synchronise printfs with cluster
spinlock_t cluster_printf_spinlock;

// ----------------------------------
// Need to be rewritten with OS agnostic symbols
extern char  __l1_preload_start;
extern char  __l1_preload_start_inL2;
extern char  __l1_preload_size;

extern char  __l1FcShared_start;
extern char  __l1FcShared_size;

extern char  __heapsram_start;
extern char  __heapsram_size;
// ----------------------------------

PI_L1 pi_cl_dma_cmd_t *fifo_first;
PI_L1 pi_cl_dma_cmd_t *fifo_last;

#if 1

struct cluster_driver_data *__per_cluster_data[NB_CLUSTER] = {NULL};

static inline void __cluster_start(struct pi_device *device);
static inline void __cluster_start_async(struct pi_device *device, pi_task_t *async_task);
static inline int __cluster_stop(struct pi_device *device);
static inline int __cluster_stop_async(struct pi_device *device, pi_task_t *async_task);
static inline int __cluster_has_task(struct cluster_driver_data *data);

static void __cluster_free_device_event_func(void *arg);

static inline void pi_push_cluster_task(struct cluster_driver_data *data, struct pi_cluster_task *task);
static inline void cl_pop_cluster_task(struct cluster_driver_data *data);

// L2 data as L1 cluster might disappear, and is shared between cluster and FC

void cl_task_finish(void);

// accessible through second level ptr or through ioctl
static const struct cluster_driver_api __spec_cluster_api = {
    .send_task = pi_cluster_send_task_to_cl,
    .send_task_async = pi_cluster_send_task_to_cl_async,
    .wait_free = pi_cluster_wait_free,
    .wait_free_async = pi_cluster_wait_free_async
};

static const struct pi_device_api __cluster_api = {
    .open = pi_cluster_open,
    .close = pi_cluster_close,
    .open_async = pi_cluster_open_async,
    .close_async = pi_cluster_close_async,
    .ioctl = pi_cluster_ioctl,
    .ioctl_async = pi_cluster_ioctl_async
};

static const struct pi_cluster_conf __cluster_default_conf = {
    .device_type = PI_DEVICE_CLUSTER_TYPE,
    // by default, us cluster 0
    .id = 0,
    // default for heap if no address provided by user.
    .heap_start = (void*)0x10000004,
    .heap_size = 0x400
};

void cl_cluster_exec_loop(void)
{
    /* Cluster Icache Enable*/
    SCBC->ICACHE_ENABLE = 0xFFFFFFFF;

    /* Initialization for the task dispatch loop */
    hal_eu_evt_mask_set((1 << CL_IRQ_DISPATCH_EVT) |
                        (1 << CL_IRQ_HW_MUTEX_EVT) |
                        (1 << CL_IRQ_BARRIER_EVT));

    asm volatile ("add    s1,  x0,  %0" :: "r" (CORE_EU_DISPATCH_DEMUX_BASE));
    asm volatile ("add    s2,  x0,  %0" :: "r" (CORE_EU_BARRIER_DEMUX_BASE));
    asm volatile ("add    s3,  x0,  %0" :: "r" (CORE_EU_CORE_DEMUX_BASE));

    if(__native_core_id())
    {
        while(1) {
            asm volatile (
                    "1:\n\t"
                    "p.elw  t0,  0(s1)         \n\t"
                    "p.elw  a0,  0(s1)         \n\t"
                    "andi   t1,  t0,   1       \n\t"
                    "bne    t1,  zero, 2f      \n\t"
                    "jalr   t0                 \n\t"
                    "p.elw  t0,  0x1c(s2)         \n\t"
                    "j      3f                 \n"
                    "2:\n\t"
                    "p.bclr t0, t0, 0,0        \n\t"
                    "jalr   t0                 \n"
                    "3:"
                    );
        } // slaveloop
    } else {
        /*
         * Core 0 will wait for tasks from FC side
         */
        /* Enable IRQ for DMA on Core 0. */
        hal_eu_irq_mask_set(1 << CL_IRQ_DMA1);
        __enable_irq();

        asm volatile("add   s4,  x0,  %0\n\t"
                :
                : "r"(__per_cluster_data)
                );
        while(1)
        {
            // MasterLoop
            asm volatile (
                    "1:\n\t"
                    // EU_EVT_MaskWaitAndClr(1 << FC_NOTIFY_CLUSTER_EVENT);
                    "p.bset  t1, x0, 0, %0 \n\t"
                    "sw      t1, 0x8(s3)   \n\t"
                    "p.elw   t0, 0x3C(s3)  \n\t"
                    "sw      t1, 0x4(s3)   \n\t"
                    // check whether driver is init, sleep otherwise
                    "lw      s5, (s4)      \n\t"
                    "beqz    s5, 4f        \n\t"
                    // task check
                    "lw      s0, (s5)      \n\t"
                    "beqz    s0, 4f        \n\t"
                    // stack init
                    "lw      a0, 8(s0)     \n\t"
                    "lw      a1, 12(s0)    \n\t"
                    "add     sp, a0, a1    \n\t"
                    "mv      a0, s0        \n\t"
                    "jal     cl_stack_init \n\t"
                    // task execution
                    "lw      a1, (s0)       \n\t"
                    "lw      a0, 4(s0)      \n\t"
                    "jalr    a1             \n\t"
                    "jal     cl_task_finish \n\t"
                    "4:"
                    :
                    : "I" (FC_NOTIFY_CLUSTER_EVENT)
                    );
        }
    } // masterloop
}

/*!
 * @brief All cores set stack.
 *
 * @param  stacksPtr     Overall stack pointer for all cores.
 * @param  coreStackSize Each core's stack size.
 * @note .
 */
static void cl_set_core_stack(void *arg)
{
    struct pi_cluster_task *task = arg;
    uint32_t cid = __native_core_id();
    if (cid)
    {
        // slaves
        uint32_t core_stack_ptr = (uint32_t) task->stacks + task->stack_size;
        core_stack_ptr += task->slave_stack_size * cid;

        /* Update sp */
        asm ("add  sp, x0, %0" :: "r" (core_stack_ptr) );
    }
}

/*!
 * @brief All cores do task init.
 * Master core 0 do stacks initilization.
 * @param.
 * @note .
  */
void cl_stack_init(struct pi_cluster_task *task)
{
    PRINTF("cl_stack_init: going in\n");
    pi_cl_team_fork(task->nb_cores, cl_set_core_stack, (void *)task);
    PRINTF("cl_stack_init: going out\n");
    hal_eu_mutex_init(0);
}


void cl_task_finish(void)
{
    int id = __native_cluster_id();

    // TODO: send callback if it exists
    // -----
    struct cluster_driver_data *data = __per_cluster_data[id];
    struct pi_cluster_task *task = data->task_first;

    PRINTF("cl_task_finish: task=%p\n",task);

    if(task->completion_callback)
    {
        pi_cl_send_task_to_fc(task->completion_callback);
    }
    // -----
    // clean up finished cluster task
    cl_pop_cluster_task(data);
    //PRINTF("cl_task_finish: data=%p\n",data);
}

/**
 * \brief open the cluster described by device with given conf
 */
int pi_cluster_open(struct pi_device *device)
{
    void *conf = device->config;
    PRINTF("OPEN_W_CONF: device=%p\n",device);
    device->api = (struct pi_device_api*)pi_default_malloc(sizeof(struct pi_device_api));

    memcpy(device->api, &__cluster_api, sizeof(struct pi_device_api));
    if(conf == NULL)
    {
        // In this case, heap will be at the first address of L1 cluster
        device->config = pi_default_malloc(sizeof(struct pi_cluster_conf));
        PRINTF("device->config=%p\n",device->config);
        memcpy(device->config, &__cluster_default_conf, sizeof(struct pi_cluster_conf));
    }

    struct pi_cluster_conf *_conf = (struct pi_cluster_conf *)device->config;
    device->data = __per_cluster_data[_conf->id];
    PRINTF("OPEN---precheck: device->data=%p\n",device->data);
    // if device data has not yet been populated
    if(device->data == NULL)
    {
        device->data = pi_default_malloc(sizeof(struct cluster_driver_data));
        PRINTF("OPEN--post-check: device->data=%p\n",device->data);
        struct cluster_driver_data *_data = (struct cluster_driver_data *)device->data;
        memset(_data, 0, sizeof(struct cluster_driver_data));
        pmsis_mutex_init(&_data->task_mutex);
        pmsis_mutex_init(&_data->powerstate_mutex);
        // fill the per cluster data ptr
        __per_cluster_data[_conf->id] = _data;
        PRINTF("per cluster data[%lx]=%lx\n",_conf->id,__per_cluster_data[_conf->id]);

        // create an event task to manage cluster driver
        struct pmsis_event_kernel_wrap *wrap = NULL;
        pmsis_event_kernel_init(&wrap, pmsis_event_kernel_main);
        _data->event_kernel = wrap;
    }
    PRINTF("near start: device->config=%p\n",device->config);
    PRINTF("near start: _conf:=%p, device->config->heap_start=%p\n",_conf, _conf->heap_start);

    _conf->heap_start = (void*)&__heapsram_start;
    _conf->heap_size = (uint32_t)&__heapsram_size;

    __cluster_start(device);
    mc_fc_delegate_init(NULL);
    return 0;
}

int pi_cluster_open_async(struct pi_device *device,
        pi_task_t *async_task)
{
    void *conf = device->config;

    // if device has not yet been populated
    if(device->data == NULL)
    {
        device->api = (struct pi_device_api*)pi_default_malloc(sizeof(struct pi_device_api));
        memcpy(device->api, &__cluster_api, sizeof(struct pi_device_api));
        if(conf == NULL)
        {
            // In this case, heap will be at the first address of L1 cluster
            device->config = pi_default_malloc(sizeof(struct pi_cluster_conf));
            memcpy(device->config, &__cluster_default_conf, sizeof(struct pi_cluster_conf));
        }
        device->data = (void*) pi_default_malloc(sizeof(struct cluster_driver_data));
        struct cluster_driver_data *_data = (struct cluster_driver_data *)device->data;
        memcpy(device->config, &__cluster_default_conf, sizeof(struct pi_cluster_conf));
    }
    __cluster_start_async(device->data, async_task);
    return 0;
}

int pi_cluster_close(struct pi_device *device)
{
    int ret = __cluster_stop(device);
    if(!ret)
    {
        struct cluster_driver_data *_data = (struct cluster_driver_data *) device->data;
        // if no task has an active handle on device
        // --> clean up data as everything in L1 is going to be lost
        PRINTF("Cluster clean ups\n");
        /* Suspend and destroy event kernel task. */
        pmsis_event_kernel_destroy(&(_data->event_kernel));
        struct pi_cluster_conf *_conf = (struct pi_cluster_conf *) device->config;
        /* Clean used mutexes. */
        pmsis_mutex_deinit(&(_data->task_mutex));
        pmsis_mutex_deinit(&(_data->powerstate_mutex));
        // free all structures
        pi_default_free(device->api, sizeof(struct pi_device_api));
        pi_default_free(device->data, sizeof(struct cluster_driver_data));
        __per_cluster_data[_conf->id] = NULL; // pointer makes no sense anymore, reset it
    }
    return ret;
}

int pi_cluster_close_async(struct pi_device *device, pi_task_t *async_task)
{
#if 0
    __cluster_stop_async(device, async_task);

    pi_task_t free_task;
    free_task.id = FC_TASK_CALLBACK_ID;
    free_task.arg[0] = (uint32_t)__cluster_free_device_event_func;
    free_task.arg[1] = (uint32_t)device;
    pi_event_append(async_task, free_task);
#endif
    return 0;
}

int pi_cluster_ioctl(struct pi_device *device, uint32_t func_id, void *arg)
{
    struct pi_device_api *api = device->api;
    struct cluster_driver_api *cluster_api = api->specific_api;
    uint32_t ret = 0;
    switch(func_id)
    {
        case SEND_TASK_ID:
            {
                struct pi_cluster_task *cl_task = (struct pi_cluster_task*)arg;
                cluster_api->send_task(device,cl_task);
            }
            break;
        case WAIT_FREE_ID:
            {
                cluster_api->wait_free(device);
            }
            break;
        case OPEN_ID:
            {
                device->api->open(device);
            }
            break;
        case CLOSE_ID:
            {
                device->api->close(device);
            }
            break;
        default:
            break;
    }

    return ret;
}

int pi_cluster_ioctl_async(struct pi_device *device, uint32_t func_id,
        void *arg, pi_task_t *async_task)
{
    struct pi_device_api *api = device->api;
    struct cluster_driver_api *cluster_api = api->specific_api;
    uint32_t ret = 0;
    switch(func_id)
    {
        case SEND_TASK_ASYNC_ID:
            {
                struct pi_cluster_task *cl_task = (struct pi_cluster_task*)arg;
                cluster_api->send_task_async(device,cl_task,async_task);
            }
            break;
        case WAIT_FREE_ASYNC_ID:
            {
                cluster_api->wait_free_async(device,async_task);
            }
            break;
        case OPEN_ASYNC_ID:
            {
                api->open_async(device,async_task);
            }
            break;
        case CLOSE_ASYNC_ID:
            {
                api->close_async(device,async_task);
            }
            break;
        default:
            break;
    }
    return ret;
}


static inline void __cluster_free_device_event_func(void *arg)
{
    struct pi_device *device = (struct pi_device *)arg;
    // free all structures
    //free(device->data, sizeof(struct cluster_driver_data));
    //free(device->api->specific_api, sizeof(struct cluster_driver_api));
    //free(device->api, sizeof(struct pi_device_api));
    //free(device, sizeof(struct pi_device));
}

static inline void __cluster_start(struct pi_device *device)
{
    PRINTF("__cluster_start: device=%p\n",device);
    struct cluster_driver_data *data = (struct cluster_driver_data *)device->data;
    PRINTF("__cluster_start: data=%p\n",data);
    struct pi_cluster_conf *conf = (struct pi_cluster_conf *)device->config;
    pmsis_mutex_take(&data->powerstate_mutex);
    // --- critical zone start ---
    if(!data->cluster_is_on)
    {
        /* Turn cluster power on. */
        __pi_pmu_cluster_power_on();
        PRINTF("poweron is done\n");

        for (uint32_t i = 0; i < (uint32_t) ARCHI_CLUSTER_NB_PE; i++)
        {
            extern uint8_t __irq_vector_base_m__;
            SCB->BOOT_ADDR[i] = (uint32_t) &__irq_vector_base_m__;
        }
        SCB->FETCH_EN = 0xFFFFFFFF;

        /* In case the chip does not support L1 preloading, the initial L1 data are in L2, we need to copy them to L1 */
        memcpy((char *)GAP_CLUSTER_TINY_DATA(0, (int)&__l1_preload_start), &__l1_preload_start_inL2, (size_t)&__l1_preload_size);

        /* Copy the FC / clusters shared data as the linker can only put it in one section (the cluster one) */
        memcpy((char *)GAP_CLUSTER_TINY_DATA(0, (int)&__l1FcShared_start), &__l1FcShared_start, (size_t)&__l1FcShared_size);

        PRINTF("conf:%p, heap_start:%p, heap_size:%lx\n",conf, conf->heap_start, conf->heap_size);
        pmsis_l1_malloc_init(conf->heap_start,conf->heap_size);

        int32_t *lock_addr = pmsis_l1_malloc(sizeof(int32_t));
        cl_sync_init_spinlock(&data->fifo_access, lock_addr);
        cl_sync_init_spinlock(&cluster_printf_spinlock, pmsis_l1_malloc(sizeof(uint32_t)));
        //pmsis_event_lock_cl_to_fc_init(data->event_kernel);
    }
    data->cluster_is_on++;
    pmsis_mutex_release(&data->powerstate_mutex);
    PRINTF("started cluster\n");
    PRINTF("data ptr: %p\n",data);
}

static inline int __cluster_stop(struct pi_device *device)
{
    struct cluster_driver_data *data = (struct cluster_driver_data *)device->data;
    pmsis_mutex_take(&data->powerstate_mutex);
    // decrement usage counter
    data->cluster_is_on--;

    // if no other tasks are using the cluster, begin poweroff procedure
    if(!data->cluster_is_on)
    {
        // wait for potential remaining tasks
        //__cluster_wait_termination(device);

        /* Turn cluster power off. */
        __pi_pmu_cluster_power_off();
    }
    pmsis_mutex_release(&data->powerstate_mutex);
    return data->cluster_is_on;
}

static inline int __cluster_has_task(struct cluster_driver_data *data)
{
    volatile int has_task = 0;
    cl_sync_spinlock_take(&data->fifo_access);
    struct pi_cluster_task *task = data->task_first;
    has_task = !(data->task_first == NULL);
    cl_sync_spinlock_release(&data->fifo_access);
    return has_task;
}

static inline void pi_push_cluster_task(struct cluster_driver_data *data, struct pi_cluster_task *task)
{
    // critical section for cluster and fc
    // Use spinlock as it is pretty much the only reliable synchronisation we have
    cl_sync_spinlock_take(&data->fifo_access);
    task->next=NULL;

    PRINTF("task=%p, data->task_first=%lx\n",task, data->task_first);
    if(data->task_last == NULL)
    {
        data->task_first = task;
        data->task_last = task;
    }
    else
    {
        data->task_last->next = task;
    }
    PRINTF("task=%p, data->task_first=%lx\n",task, data->task_first);
    cl_sync_spinlock_release(&data->fifo_access);
}

static inline void cl_pop_cluster_task(struct cluster_driver_data *data)
{
    // critical section for cluster and fc
    // Uses spinlock as it is pretty much the only reliable synchronisation we have
    cl_sync_spinlock_take(&data->fifo_access);
    struct pi_cluster_task *task = data->task_first;
    data->task_first = task->next;
    if(data->task_first == NULL)
    {
        data->task_last = NULL;
    }
    cl_sync_spinlock_release(&data->fifo_access);
    if(task->stack_allocated)
    {
        // put everything back as it was before
        uint32_t stack_size = task->slave_stack_size * ((uint32_t) ARCHI_CLUSTER_NB_PE - 1);
        stack_size += task->stack_size;
        pi_cl_l1_free(NULL, task->stacks, stack_size);
        PRINTF("Free %p %d\n", task->stacks, stack_size);
        task->stacks = NULL;
        task->stack_size = 0;
        task->stack_allocated = 0;
    }
}

static inline void __cluster_start_async(struct pi_device *device, pi_task_t *async_task)
{
    struct cluster_driver_data *cluster_data = (struct cluster_driver_data *)device->data;
}

static inline int __cluster_stop_async(struct pi_device *device, pi_task_t *async_task)
{
    struct cluster_driver_data *cluster_data = (struct cluster_driver_data *)device->data;
    return 0;
}

static inline int __pi_send_task_to_cl(struct pi_device *device, struct pi_cluster_task *task)
{
    struct cluster_driver_data *data = (struct cluster_driver_data *)device->data;
    pmsis_mutex_take(&data->task_mutex);

    if(!data->cluster_is_on)
    {// cluster is not on, can not reliably use L1 ram
        pmsis_mutex_release(&data->task_mutex);
        return -1;
    }

    /* If Cluster has not finished previous task, wait */

    if (task->nb_cores == 0)
    {
        task->nb_cores = (uint32_t) pi_cl_cluster_nb_pe_cores();
    }
    task->cluster_team_mask = ((1 << task->nb_cores) - 1);
    //task->nb_cores = task->nb_cores ? task->nb_cores : ARCHI_CLUSTER_NB_PE;

    PRINTF("task->nb_cores:%lx\n", task->nb_cores);

    if (task->stacks == NULL)
    {
        if (task->stack_size == 0)
        {
            task->stack_size = (uint32_t) CL_MASTER_CORE_STACK_SIZE;
        }
        if (task->slave_stack_size == 0)
        {
            task->slave_stack_size = (uint32_t) CL_SLAVE_CORE_STACK_SIZE;
        }
        uint32_t stack_size = task->slave_stack_size * ((uint32_t) ARCHI_CLUSTER_NB_PE - 1);
        stack_size += task->stack_size;
        task->stacks = pi_cl_l1_malloc(NULL, stack_size);
        PRINTF("Malloc stack %p %d\n", task->stacks, stack_size);
        if (task->stacks == NULL)
        {
            pmsis_mutex_release(&(data->task_mutex));
            return -1;
        }
        task->stack_allocated = 1; // to remember freeing memory if allocated by us
    }
    else
    {
        task->stack_allocated = 0;
    }

    pi_push_cluster_task(data, task);

    // Wake cluster up
    hal_eu_cluster_evt_trig_set(FC_NOTIFY_CLUSTER_EVENT, 0);
    pmsis_mutex_release(&data->task_mutex);
    return 0;
}

int pi_cluster_send_task_to_cl(struct pi_device *device, struct pi_cluster_task *task)
{
    int32_t ret_send_task = 0;
    pi_task_t task_block;
    PRINTF("creating task\n");
    pi_task_block(&task_block);
    task->completion_callback = &task_block;
    PRINTF("going to send\n");
    ret_send_task = __pi_send_task_to_cl(device, task);
    if (ret_send_task)
    {
        return ret_send_task;
    }
    PRINTF("going to block on %p\n", &task_block);
    pi_task_wait_on(&task_block);
    PRINTF("block is done\n");
    pi_task_destroy(&task_block);
    PRINTF("destroy is done\n");
    return ret_send_task;
}

int pi_cluster_send_task_to_cl_async(struct pi_device *device, struct pi_cluster_task *task, pi_task_t *fc_task)
{
    task->completion_callback = fc_task;
    while(__pi_send_task_to_cl(device,task))
    {
        __os_native_yield();
    }
    return 0;
}

void pi_cluster_wait_free(struct pi_device *device)
{
    /* If Cluster has not finished previous task, wait */
    while(__cluster_has_task((struct cluster_driver_data *)device->data))
    {
        //__os_native_yield();
    }
}

void pi_cluster_wait_free_async(struct pi_device *device, pi_task_t *async_task)
{

}

uint8_t pi_cluster_is_on(void)
{
    for(int i=0; i<NB_CLUSTER; i++)
    {
        if(__per_cluster_data[i] && __per_cluster_data[i]->cluster_is_on)
        {
            return 1;
        }
    }
    return 0;
}


void pi_cluster_conf_init(struct pi_cluster_conf *conf)
{
    conf->device_type = PI_DEVICE_CLUSTER_TYPE;
    conf->id = 0;
}


#endif
#endif
