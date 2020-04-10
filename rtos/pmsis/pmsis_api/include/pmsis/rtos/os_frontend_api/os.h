/*
 * Copyright (C) 2019 GreenWaves Technologies
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

#ifndef __PMSIS_RTOS_OS_H__
#define __PMSIS_RTOS_OS_H__

#include "pmsis/pmsis_types.h"

/**
 * @ingroup groupRTOS
 *
 * @defgroup Task Task management
 *
 * \brief OS tasks management.
 *
 * This part details tasks management along with semaphores and mutexes.
 *
 * @addtogroup Task
 * @{
 */

/**
 * \brief Function type.
 *
 * Function type taking a pointer to void and returning void.
 */
typedef void (*func_t)(void *arg);

/**
 * \brief Open OS.
 *
 * \param device         Pointer to pi_device.
 *
 * \return xxx.
 */
int pi_os_open(struct pi_device *device);

/**
 * \brief Close OS.
 *
 * \param device        Pointer to pi_device.
 */
void pi_os_close(struct pi_device *device);

/**
 * \brief Kickoff the system.
 *
 * This function start the system, prepares the event kernel, IRQ,...
 * Completely OS dependant might do anything from a function call to main task
 * creation.
 *
 * \param arg            Parameter given to main task/thread.
 *
 * \retval 0             If operation is successful.
 * \retval ERRNO         An error code otherwise.
 *
 * \note This function must be called in the main in order to launch the event
 *       kernel, enable IRQ, create the main task and start the scheduler.
 */
static inline int pmsis_kickoff(void *arg);

/**
 * \brief Stop runtime and exit platform.
 *
 * This function stops runtime and exits platform.
 * Once it is called, there is no return.
 *
 * \param err            Exit status.
 */
static inline void pmsis_exit(int err);


#if defined(PMSIS_DRIVERS)

/**
 * \deprecated
 * \brief Create a task.
 *
 * This function creates an OS task.
 *
 * \param entry          Task function.
 * \param arg            Arguments to task's function.
 * \param name           Name of the task.
 * \param priority       Task priority.
 *
 * \retval task_handler  Pointer to created task.
 * \retval NULL          If task has not been created.
 */
static inline void *pmsis_task_create(void (*entry)(void*), void *arg,
                                      char *name, int priority);

/**
 * \brief Create a task.
 *
 * This function creates an OS task.
 *
 * \param func           Task function.
 * \param arg            Arguments to task's function.
 * \param name           Name of the task.
 * \param stack_size     Stack size given to the task.
 * \param priority       Task priority.
 *
 * \retval task_handler  Pointer to created task.
 * \retval NULL          If task has not been created.
 */
void *pi_task_create(func_t func, void *arg, char *name, uint32_t stack_size,
                     int priority);

/**
 * \brief Resume a task.
 *
 * Calling this function resumes a suspended task.
 *
 * \param task_handler   Pointer to the task to suspend.
 */
void pi_task_resume(void *task_handler);

/**
 * \deprecated
 * \brief Suspend a task.
 *
 * Calling this function suspends a task.
 *
 * \param task_handler   Pointer to the task to suspend.
 */
static inline void pmsis_task_suspend(void *task_handler);

/**
 * \brief Suspend a task.
 *
 * Calling this function suspends a task.
 * If given parameter task_handler is NULL, then calling task is suspended.
 *
 * \param task_handler   Pointer to the task to suspend.
 */
void pi_task_suspend(void *task_handler);

/**
 * \brief Terminate a task.
 *
 * This function suspends a task and remove it from scheduling list.
 * Once called, the task can not run.
 *
 * \param task_handler   Pointer to the task to suspend.
 */
void pi_task_terminate(void *task_handler);

/**
 * \brief Call OS to switch context.
 *
 * This function can be called when a task should let another task run on CPU.
 * In case of waiting for the end of some transfers, synchronisation,...
 */
static inline void pi_yield();

/**
 * @} addtogroup Task
 */

/**
 * \brief Disable IRQ.
 *
 * Calling this function disables IRQ, ie CPU will not receive IRQ.
 *
 * \return IRQ           Current IRQ mode.
 */
static inline int disable_irq(void);

/**
 * \brief Restore IRQ.
 *
 * This function will or will not enable IRQ depending on given parameter.
 *
 * \param irq_enable     IRQ mode.
 *
 * \note This function should be used in pair with disable_irq().
 */
static inline void restore_irq(int irq_enable);

/**
 * @ingroup groupRTOS
 *
 * @defgroup Semaphore Semaphore management
 *
 * \brief Semaphore management.
 *
 * This part details OS semaphores management.
 *
 * @addtogroup Semaphore
 * @{
 */

/**
 * \brief Initialize a semaphore.
 *
 * This function creates and intializes a semaphore.
 *
 * \param semaphore      Pointer to struct pi_sem.
 *
 * \retval 0             If operation is successful.
 * \retval ERRNO         An error code otherwise.
 */
static inline int pi_sem_init(pi_sem_t *semaphore);

/**
 * \brief Delete a semaphore.
 *
 * This function deletes a semaphore.
 *
 * \param semaphore      Pointer to struct pi_sem.
 *
 * \retval 0             If operation is successful.
 * \retval ERRNO         An error code otherwise.
 */
static inline int pi_sem_deinit(pi_sem_t *semaphore);

/**
 * \brief Take a semaphore.
 *
 * This function is used to take a sempahore.
 *
 * \param semaphore      Pointer to struct pi_sem.
 *
 * \note The behaviour when the semaphore is not taken depend on OS
 *       implementation.
 */
static inline void pi_sem_take(pi_sem_t *semaphore);

/**
 * \brief Give a semaphore.
 *
 * This function is used to give/release a sempahore.
 *
 * \param semaphore      Pointer to struct pi_sem.
 */
static inline void pi_sem_give(pi_sem_t *semaphore);

/**
 * @} addtogroup Semaphore
 */

/**
 * @ingroup groupRTOS
 *
 * @defgroup Mutex Mutex management
 *
 * \brief Mutex management.
 *
 * This part details OS mutexes management.
 *
 * @addtogroup Mutex
 * @{
 */

/**
 * \brief Initialize a mutex.
 *
 * This function creates and intializes a mutex.
 *
 * \param mutex          Pointer to struct pmsis_mutex.
 *
 * \retval 0             If operation is successful.
 * \retval ERRNO         An error code otherwise.
 */
static inline int pmsis_mutex_init(pmsis_mutex_t *mutex);

/**
 * \brief Delete a mutex.
 *
 * This function deletes a mutex.
 *
 * \param mutex          Pointer to struct pmsis_mutex.
 *
 * \retval 0             If operation is successful.
 * \retval ERRNO         An error code otherwise.
 */
static inline int pmsis_mutex_deinit(pmsis_mutex_t *mutex);

/**
 * \brief Take a mutex.
 *
 * This function is used to lock a mutex.
 *
 * \param mutex          Pointer to struct pmsis_mutex.
 *
 * \note The behaviour when the mutex is not locked depend on OS
 *       implementation.
 */
static inline void pmsis_mutex_take(pmsis_mutex_t *mutex);

/**
 * \brief Give a mutex.
 *
 * This function is used to release a mutex.
 *
 * \param mutex          Pointer to struct pmsis_mutex.
 */
static inline void pmsis_mutex_release(pmsis_mutex_t *mutex);

#endif

/**
 * @} addtogroup Mutex
 */

#endif  /* __PMSIS_RTOS_OS_H__ */
