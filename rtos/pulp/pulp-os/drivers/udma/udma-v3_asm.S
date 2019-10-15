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
 * Authors: Germain Haugou, GreenWaves Technologies (germain.haugou@greenwaves-technologies.com)
 */

#include "rt/rt_data.h"
#include "archi/pulp.h"

#include "pmsis/data/udma.h"

  // x9: channel, x10: event, x8,x11,x12:temp
  .global __rt_udma_handle_copy
__rt_udma_handle_copy:

  slli   x8, x10, 2
  lw     x8, %tiny(__rt_udma_channels)(x8)

  // First update all the queues
  lw     x12, RT_UDMA_CHANNEL_T_PENDINGS_1(x8)
  lw     x11, RT_UDMA_CHANNEL_T_PENDINGS_0(x8)
  lw     x9, RT_UDMA_CHANNEL_T_PENDINGS_FIRST(x8)
  sw     x12, RT_UDMA_CHANNEL_T_PENDINGS_0(x8)


  bnez   x9, __rt_udma_handle_pending

  sw     x0, RT_UDMA_CHANNEL_T_PENDINGS_1(x8)
  la     x9, udma_event_handler_end
  j      __rt_event_enqueue




__rt_udma_handle_pending:
  sw     x9, RT_UDMA_CHANNEL_T_PENDINGS_1(x8)
  lw     x11, PI_TASK_T_NEXT(x9)
  sw     x11, RT_UDMA_CHANNEL_T_PENDINGS_FIRST(x8)

  // Now enqueue the pending copy to the udma
  // First the RX user buffer
  // TODO
