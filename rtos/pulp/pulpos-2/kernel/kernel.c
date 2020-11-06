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

#include "pmsis.h"


int pos_kernel_pmsis_exit_value;


PI_FC_L1 static pos_cbsys_t *pos_cbsys_first[POS_CBSYS_NB];

void pos_cbsys_del(pos_cbsys_t *cbsys_del)
{
    pos_cbsys_e cbsys_id = cbsys_del->id;
    pos_cbsys_t *cbsys = pos_cbsys_first[cbsys_id];
    pos_cbsys_t *prev = NULL;

    while(cbsys)
    {
        if (cbsys == cbsys_del)
        {
            if (prev)
            {
                prev->next = cbsys->next;
            }
            else
            {
                pos_cbsys_first[cbsys_id] = cbsys->next;
            }
        }

        prev = cbsys;
        cbsys = cbsys->next;
    }
}


void pos_cbsys_add(pos_cbsys_t *cbsys, pos_cbsys_e cbsys_id, int (*cb)(void *), void *cb_arg)
{
    cbsys->callback = cb;
    cbsys->arg = cb_arg;
    cbsys->next = pos_cbsys_first[cbsys_id];
    pos_cbsys_first[cbsys_id] = cbsys;
}


int pos_cbsys_exec(pos_cbsys_e cbsys_id)
{
    pos_cbsys_t *cbsys = pos_cbsys_first[cbsys_id];

    while (cbsys)
    {
        if (cbsys->callback(cbsys->arg)) return -1;
        cbsys = cbsys->next;
    }

    return 0;
}


void pos_kernel_init()
{
    for (int i=0; i<POS_CBSYS_NB; i++)
    {
        pos_cbsys_first[i] = NULL;
    }
}
