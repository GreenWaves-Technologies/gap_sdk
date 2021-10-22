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
    return __os_native_api_create_task(entry, arg, name, 2*configMINIMAL_STACK_SIZE, priority);
}

static inline void *pi_task_create(pi_task_entry_t func, void *arg, char *name, uint32_t stack_size,
        int priority)
{
    return __os_native_api_create_task(func, arg, name, stack_size, priority);
}

static inline void *pi_user_task_create(pi_task_entry_t func,
        void *arg,
        char *name,
        uint32_t stack_size,
        int priority)
{
    return __os_native_api_create_user_task(func, arg, name, stack_size, priority);
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

static inline int pi_irq_disable(void)
{
    hal_compiler_barrier();
    return __os_native_api_disable_irq();
}

static inline void pi_irq_enable()
{
    hal_compiler_barrier();
    __os_native_api_enable_irq();
}

static inline void pi_irq_restore(int irq_enable)
{
    hal_compiler_barrier();
    __os_native_api_restore_irq(irq_enable);
}

static inline int disable_irq(void)
{
    return pi_irq_disable();
}

static inline void restore_irq(int irq_enable)
{
    pi_irq_restore(irq_enable);
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


static inline int pi_sync_obj_init(void *sync_obj)
{
    hal_compiler_barrier();
    return __os_native_api_sync_obj_init(sync_obj);
}

static inline int pi_sync_obj_deinit(void *sync_obj)
{
    hal_compiler_barrier();
    return __os_native_api_sync_obj_deinit(sync_obj);
}

static inline void pi_sync_obj_take(void *sync_obj)
{
    hal_compiler_barrier();
    __os_native_api_sync_obj_take(sync_obj);
}

static inline void pi_sync_obj_release(void *sync_obj)
{
    hal_compiler_barrier();
    __os_native_api_sync_obj_release(sync_obj);
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

#if !defined(__GAP8__)// && !defined(__VEGA__)
static inline void pi_irq_handler_set(int irq, void (*handler)())
{
    // use a wrapper to create C context
    __os_native_irq_wrapper_handler_set(irq, handler);
}

static inline void pi_irq_handler_fast_set(int irq, void (*handler)())
{
    // jump directly
    __os_native_irq_handler_set(irq, handler);
}

static inline void pi_irq_exception_handler_set(int cause, void (*handler)())
{
    // use a wrapper to create C context
    __os_native_exception_handler_set(cause, handler);
}

static inline void pi_irq_mask_enable(int irq)
{
    __os_native_irq_unmask(irq);
}

static inline void pi_irq_mask_disable(int irq)
{
    __os_native_irq_mask(irq);
}

#if !defined(__VEGA__)
static inline void pi_os_reboot(void)
{
    hal_soc_ctrl_sw_reset();
}
#endif  /* __VEGA__ */

#endif  /* __GAP8__ */

#endif  /* __PI_RTOS_IMPLEM_OS_H__ */
