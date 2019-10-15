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

#ifndef __EVENT_KERNEL_H__
#define __EVENT_KERNEL_H__

#include "pmsis/pmsis_types.h"

/** Typical event kernel main function **/
void pmsis_event_kernel_main(void *arg);

/** Allocate the structure and create a task if need be **/
int pmsis_event_kernel_init(struct pmsis_event_kernel_wrap **event_kernel,
        void (*event_kernel_entry)(void*));

/** Free the structure and kill the task if need be **/
void pmsis_event_kernel_destroy(struct pmsis_event_kernel_wrap **event_kernel);

/**
 * Affect task to an allocated event
 * May be called from either cluster or fc
 **/
int pmsis_event_push(struct pmsis_event_kernel_wrap *event_kernel, pi_task_t *task);

void pmsis_event_kernel_mutex_release(struct pmsis_event_kernel_wrap *wrap);

void pmsis_event_lock_cl_to_fc_init(struct pmsis_event_kernel_wrap *wrap);

struct pmsis_event_kernel_wrap *pmsis_event_get_default_scheduler(void);

void pmsis_event_set_default_scheduler(struct pmsis_event_kernel_wrap *wrap);

void pmsis_event_destroy_default_scheduler(struct pmsis_event_kernel_wrap *wrap);
#endif
