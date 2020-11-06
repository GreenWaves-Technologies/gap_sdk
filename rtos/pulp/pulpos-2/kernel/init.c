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

#include "io.h"


typedef void (*fptr)(void);

static fptr ctor_list[1] __attribute__((section(".ctors.start"))) = { (fptr) -1 };
static fptr dtor_list[1] __attribute__((section(".dtors.start"))) = { (fptr) -1 };


static void pos_init_do_ctors(void)
{
    fptr *fpp;

    for(fpp = ctor_list+1;  *fpp != 0;  ++fpp)
    {
        (**fpp)();
    }
}



static void pos_init_do_dtors(void)
{
    fptr *fpp;
    for(fpp = dtor_list + 1;  *fpp != 0;  ++fpp)
    {
        (**fpp)();
    }
}



void pos_init_start()
{
#if PULP_CHIP_FAMILY == CHIP_GAP
  // Always allow JTAG accesses for now as security is not implemented
  hal_pmu_bypass_set (ARCHI_REG_FIELD_SET (hal_pmu_bypass_get (), 1, 11, 1) );
#endif

  INIT_TRACE(POS_LOG_INFO, "Starting runtime initialization\n");

  pos_irq_init();

  pos_soc_init();

  pos_soc_event_init();

  // Initialize first the memory allocators and the utils so that they are
  // available for constructors, especially to let them declare
  // callbacks
  //__rt_utils_init();
  pos_allocs_init();

  // Scheduler is initialized now to let other modules use it early
  pos_sched_init();

  // Call global and static constructors
  // Each module may do private initializations there
  pos_init_do_ctors();

  // Now that the system is ready, activate IO
  pos_io_start();

  // Now now the minimal init are done, we can activate interruptions
  hal_irq_enable();
}


void pos_init_stop()
{
    // Close IO to flush them
    pos_io_stop();

    /* Call global and static destructors */
    pos_init_do_dtors();
}
