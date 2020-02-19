/*
 * Copyright (c) 2019, GreenWaves Technologies, Inc.
 * All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without modification,
 * are permitted provided that the following conditions are met:
 *
 * o Redistributions of source code must retain the above copyright notice, this list
 *   of conditions and the following disclaimer.
 *
 * o Redistributions in binary form must reproduce the above copyright notice, this
 *   list of conditions and the following disclaimer in the documentation and/or
 *   other materials provided with the distribution.
 *
 * o Neither the name of GreenWaves Technologies, Inc. nor the names of its
 *   contributors may be used to endorse or promote products derived from this
 *   software without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND
 * ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED
 * WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
 * DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE FOR
 * ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES
 * (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES;
 * LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON
 * ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
 * (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS
 * SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 */

#ifndef __PI_RTOS_IMPLEM_OS_H__
#define __PI_RTOS_IMPLEM_OS_H__

#include "pmsis/rtos/os_frontend_api/os.h"
#include "pmsis/backend/implementation_specific_defines.h"
#include "pmsis/backend/pmsis_backend_native_task_api.h"

/*******************************************************************************
 * Definitions
 ******************************************************************************/

/*******************************************************************************
 * Function declaration
 ******************************************************************************/

/*******************************************************************************
 * API implementation
 ******************************************************************************/

static inline int pmsis_kickoff(void *arg)
{
    return __os_native_kickoff(arg);
}

static inline void pmsis_exit(int err)
{
    __os_native_exit(err);
}

static inline void *pmsis_task_create(void (*entry)(void*),
        void *arg,
        char *name,
        int priority)
{
    return __os_native_api_create_task(entry, arg, name, priority);
}

static inline void pmsis_task_suspend(void *task_handler)
{
    __os_native_task_t *task = (__os_native_task_t *) task_handler;
    __os_native_task_suspend(task);
}

static inline void pi_yield()
{
    __os_native_yield();
}

static inline int disable_irq(void)
{
    hal_compiler_barrier();
    return __os_native_api_disable_irq();
}

static inline void restore_irq(int irq_enable)
{
    hal_compiler_barrier();
    __os_native_api_restore_irq(irq_enable);
}

static inline int pi_sem_init(pi_sem_t *sem)
{
    hal_compiler_barrier();
    return  __os_native_api_sem_init(sem);
}

static inline int pi_sem_deinit(pi_sem_t *sem)
{
    hal_compiler_barrier();
    return  __os_native_api_sem_deinit(sem);
}

static inline void pi_sem_take(pi_sem_t *sem)
{
    hal_compiler_barrier();
    sem->take(sem->sem_object);
}

static inline void pi_sem_give(pi_sem_t *sem)
{
    sem->give(sem->sem_object);
    hal_compiler_barrier();
}

static inline void pmsis_mutex_take(pmsis_mutex_t *mutex)
{
    hal_compiler_barrier();
#ifdef __NO_NATIVE_MUTEX__
    int irq_enabled;
    volatile int mutex_free=0;
    while (!mutex_free)
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

#endif  /* __PI_RTOS_IMPLEM_OS_H__ */
