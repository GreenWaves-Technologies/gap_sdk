/*
 * Copyright (C) 2018 ETH Zurich, University of Bologna and GreenWaves Technologies
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
 * Authors: Germain Haugou, ETH (germain.haugou@iis.ee.ethz.ch)
 */

#include "rt/rt_data.h"
#include "archi/pulp.h"


  // x8: periph data, x10:event, x9,x11,x12:temp
  .global __rt_mram_handle_event
__rt_mram_handle_event:

  // Get pending copy corresponding to the soc event we just received
  lw     x12, RT_MRAM_T_FIRST_PENDING_COPY(x8)
  lw     x9, RT_PERIPH_COPY_T_NEXT(x12)
  lw     x11, RT_PERIPH_COPY_T_EVENT(x12)

  // Update pending copy list to the next one
  sw     x9, RT_MRAM_T_FIRST_PENDING_COPY(x8)

  // Enqueue pending copy event to scheduler
  la     x9, __rt_fc_socevents_handler_exit
  j      __rt_event_enqueue
