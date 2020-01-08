/*
 * Copyright (C) 2018 GreenWaves Technologies
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *     http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

#ifndef __PMSIS_PMSIS_TASK_H__
#define __PMSIS_PMSIS_TASK_H__

// used to avoid OS dependency or in bare metal mode
//#define __NO_NATIVE_MUTEX__

#include "pmsis.h"
#include "pmsis_backend/pmsis_backend_native_task_api.h"

static inline int disable_irq(void);

static inline void restore_irq(int irq_enable);

#ifdef PMSIS_DRIVERS
static inline void pi_sem_take(pi_sem_t *mutex);

static inline void pi_sem_give(pi_sem_t *mutex);

static inline int pi_sem_init(pi_sem_t *mutex);

static inline int pi_sem_deinit(pi_sem_t *mutex);

static inline void pmsis_mutex_take(pmsis_mutex_t *mutex);

static inline void pmsis_mutex_release(pmsis_mutex_t *mutex);

static inline int pmsis_mutex_init(pmsis_mutex_t *mutex);

static inline int pmsis_mutex_deinit(pmsis_mutex_t *mutex);

static inline void pmsis_spinlock_init(pmsis_spinlock_t *spinlock);

static inline void pmsis_spinlock_take(pmsis_spinlock_t *spinlock);

static inline void pmsis_spinlock_release(pmsis_spinlock_t *spinlock);

static inline void *pmsis_task_create(void (*entry)(void*),
        void *arg,
        char *name,
        int priority);

static inline void pmsis_task_suspend(__os_native_task_t *task);

#endif

static inline void pmsis_exit(int err);

static inline void pi_yield();

#ifdef PMSIS_DRIVERS

/*
 * Disable IRQs while saving previous irq state
 */
static inline int disable_irq(void)
{
    hal_compiler_barrier();
    return __os_native_api_disable_irq();
}

/*
 * restore IRQs state
 */
static inline void restore_irq(int irq_enable)
{
    hal_compiler_barrier();
    __os_native_api_restore_irq(irq_enable);
}

/*
 * lock a mutex and begin critical section
 */
static inline void pmsis_mutex_take(pmsis_mutex_t *mutex)
{
    hal_compiler_barrier();
#ifdef __NO_NATIVE_MUTEX__
    int irq_enabled;
    volatile int mutex_free=0;
    while(!mutex_free)
    {
        irq_enabled = disable_irq();
        hal_compiler_barrier();
        mutex_free = !((volatile uint32_t)mutex->mutex_object);
        hal_compiler_barrier();
        restore_irq(irq_enabled);
    }
    irq_enabled = disable_irq();
    mutex->mutex_object = (void*)1;
    restore_irq(irq_enabled);
#else
    mutex->take(mutex->mutex_object);
#endif
}

/*
 * unlock mutex and end critical section
 */
static inline void pmsis_mutex_release(pmsis_mutex_t *mutex)
{
    hal_compiler_barrier();
#ifdef __NO_NATIVE_MUTEX__
    int irq_enabled = disable_irq();
    hal_compiler_barrier();
    mutex->mutex_object = (void*)0;
    hal_compiler_barrier();
    restore_irq(irq_enabled);
#else
    mutex->release(mutex->mutex_object);
    hal_compiler_barrier();
#endif
}

/*
 * init a mutex, return non zero in case of failure
 */
static inline int pmsis_mutex_init(pmsis_mutex_t *mutex)
{
    hal_compiler_barrier();
#ifdef __NO_NATIVE_MUTEX__
    mutex->mutex_object = (void*)0;
    return 0;
#else
    return  __os_native_api_mutex_init(mutex);
#endif
}

/*
 * deinit a mutex, return non zero in case of failure
 */
static inline int pmsis_mutex_deinit(pmsis_mutex_t *mutex)
{
    hal_compiler_barrier();
#ifdef __NO_NATIVE_MUTEX__
    mutex->mutex_object = (void*)0;
    return 0;
#else
    return  __os_native_api_mutex_deinit(mutex);
#endif
}
/*
 * decrement a semaphore
 */
static inline void pi_sem_take(pi_sem_t *sem)
{
    hal_compiler_barrier();
    sem->take(sem->sem_object);
}

/*
 * increment a semapphore
 */
static inline void pi_sem_give(pi_sem_t *sem)
{
    sem->give(sem->sem_object);
    hal_compiler_barrier();
}

/*
 * init a semaphore, return non zero in case of failure
 */
static inline int pi_sem_init(pi_sem_t *sem)
{
    hal_compiler_barrier();
    return  __os_native_api_sem_init(sem);
}

/*
 * deinit a semaphore, return non zero in case of failure
 */
static inline int pi_sem_deinit(pi_sem_t *sem)
{
    hal_compiler_barrier();
    return  __os_native_api_sem_deinit(sem);
}

static inline void pmsis_spinlock_init(pmsis_spinlock_t *spinlock)
{
    hal_compiler_barrier();
    spinlock->lock = 0;
}

static inline void pmsis_spinlock_take(pmsis_spinlock_t *spinlock)
{
    int irq_enabled = disable_irq();
    hal_compiler_barrier();
    spinlock->lock = 1;
    hal_compiler_barrier();
    restore_irq(irq_enabled);
}

static inline void pmsis_spinlock_release(pmsis_spinlock_t *spinlock)
{
    int irq_enabled = disable_irq();
    hal_compiler_barrier();
    spinlock->lock = 0;
    hal_compiler_barrier();
    restore_irq(irq_enabled);
}

static inline void *pmsis_task_create(void (*entry)(void*),
        void *arg,
        char *name,
        int priority)
{
    return __os_native_api_create_task(entry, arg, name, priority);
}

static inline void pmsis_task_suspend(__os_native_task_t *task)
{
    __os_native_task_suspend(task);
}

static inline void pmsis_exit(int err)
{
    __os_native_exit(err);
}

static inline void pi_yield()
{
    __os_native_yield();
}

#endif  /* __PMSIS_TASK_H__ */

#endif
