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

/* 
 * Authors: Germain Haugou, GreenWaves Technologies (germain.haugou@greenwaves-technologies.com)
 */

#ifndef __POS_IMPLEM_KERNEL_H__
#define __POS_IMPLEM_KERNEL_H__

typedef enum
{
  POS_CBSYS_START,
  POS_CBSYS_STOP,
  POS_CBSYS_POWEROFF,
  POS_CBSYS_POWERON,
  POS_CBSYS_PERIPH_SETFREQ_BEFORE,
  POS_CBSYS_PERIPH_SETFREQ_AFTER,
  POS_CBSYS_NB
} pos_cbsys_e;

void pos_cbsys_del(pos_cbsys_t *cbsys);

void pos_cbsys_add(pos_cbsys_t *cbsys, pos_cbsys_e cbsys_id, int (*cb)(void *), void *cb_arg);

int pos_cbsys_exec(pos_cbsys_e cbsys_id);

void pos_kernel_init();

#endif