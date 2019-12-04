#include "pmsis.h"
#include "pmsis/task.h"
#include "pmsis/rtos/pmsis_os.h"
#include "pmsis/rtos/os_frontend_api/pmsis_time.h"

pi_task_t *pi_task_callback(pi_task_t *callback_task, void (*func)(void *), void *arg)
{
    callback_task->id = PI_TASK_CALLBACK_ID;
    callback_task->arg[0] = (uintptr_t)func;
    callback_task->arg[1] = (uintptr_t)arg;
    callback_task->done = 0;
    callback_task->wait_on.sem_object = (void*)NULL;
    callback_task->destroy = 0;
    return callback_task;
}

pi_task_t *pi_task_callback_no_mutex(pi_task_t *callback_task, void (*func)(void *), void *arg)
{
    return pi_task_callback(callback_task, func, arg);
}

pi_task_t *__pi_task_block(pi_task_t *callback_task)
{
    callback_task->id = PI_TASK_NONE_ID;
    callback_task->done = 0;
    pi_sem_init(&(callback_task->wait_on));
    // lock the mutex so that task may be descheduled while waiting on it
    callback_task->destroy = 1;
    return callback_task;
}

pi_task_t *pi_task_block_no_mutex(pi_task_t *callback_task)
{
    callback_task->id = PI_TASK_NONE_ID;
    callback_task->done = 0;
    callback_task->wait_on.sem_object = (void*)NULL;
    callback_task->destroy = 0;
    // lock the mutex so that task may be descheduled while waiting on it
    return callback_task;
}

void pi_task_release(pi_task_t *task)
{
    DEBUG_PRINTF("[%s] releasing task %p\n",__func__,task);
    // if the mutex is only virtual (e.g. wait on soc event)
    // if the sched support semaphore/mutexes
    if(task->wait_on.sem_object)
    {
        DEBUG_PRINTF("[%s] sem give %p\n",__func__,task->wait_on);
        pi_sem_give(&(task->wait_on));
    }
    hal_compiler_barrier();
    task->done = 1;
    hal_compiler_barrier();
}

void pi_task_wait_on(pi_task_t *task)
{
    // FIXME: workaround for gcc bug
    hal_compiler_barrier();
    while (!task->done)
    {
        // if the underlying scheduler support it, deschedule the task
        if (task->wait_on.sem_object != NULL)
        {
            pi_sem_take(&task->wait_on);
        }
        DEBUG_PRINTF("[%s] waited on sem\n", __func__);
    }
    hal_compiler_barrier();
    __pi_task_destroy(task);
}

void pi_task_wait_on_no_mutex(pi_task_t *task)
{
    // if the mutex is only virtual (e.g. wait on soc event)
    while(!task->done)
    {
        hal_compiler_barrier();
        asm volatile ("nop");
        // FIXME: workaround for gcc bug
        hal_compiler_barrier();
    }
}

void __pi_task_destroy(pi_task_t *task)
{
    if (task->destroy)
    {
        task->destroy = 0;
        // if the mutex is only virtual (e.g. wait on soc event)
        hal_compiler_barrier();
        // if the sched support semaphore/mutexes
        if(task->wait_on.sem_object)
        {
            pi_sem_deinit(&task->wait_on);
            task->wait_on.sem_object = (void*)NULL;
        }
        hal_compiler_barrier();
    }
}

void pi_task_push(pi_task_t *task)
{
    pi_task_push_delayed_us(task, 0);
}

void pi_task_push_delayed_us(pi_task_t *task, uint32_t delay)
{
    pi_time_wait_us(delay);
    uint32_t irq = disable_irq();
    pmsis_event_push(pmsis_event_get_default_scheduler(), task);
    restore_irq(irq);
}
