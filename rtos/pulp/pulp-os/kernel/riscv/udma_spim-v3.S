/*
 * Copyright (C) 2018 ETH Zurich and University of Bologna
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
#if UDMA_VERSION == 2
#include "archi/udma/udma_v2.h"
#else
#include "archi/udma/udma_v3.h"
#endif
#if SOC_EU_VERSION == 3
#include "archi/soc_eu/soc_eu_v3.h"
#elif SOC_EU_VERSION == 2
#include "archi/soc_eu/soc_eu_v2.h"
#else
#include "archi/soc_eu/soc_eu_v1.h"
#endif


  // x8: periph, x10:event, x8,x11,x12:temp
  .global __rt_spim_handle_eot
__rt_spim_handle_eot:

  lw     x12, RT_PERIPH_SPIM_T_PENDING1(x8)
  lw     x10, RT_PERIPH_SPIM_T_PENDING0(x8)
  lw     x9, RT_PERIPH_SPIM_T_FIRST_WAITING(x8)
  sw     x12, RT_PERIPH_SPIM_T_PENDING0(x8)

  bnez   x9, __rt_spim_handle_pending

  sw     x0, RT_PERIPH_SPIM_T_PENDING1(x8)
  lw     x11, RT_PERIPH_COPY_T_EVENT(x10)
  la     x9, __rt_fc_socevents_handler_exit
  j      __rt_event_enqueue




__rt_spim_handle_pending:
  sw     x9, RT_PERIPH_SPIM_T_PENDING1(x8)
  lw     x11, RT_PERIPH_COPY_T_NEXT(x9)
  sw     x11, RT_PERIPH_SPIM_T_FIRST_WAITING(x8)

  // Now enqueue the pending copy to the udma
  // First the RX user buffer
  lw     x11, RT_PERIPH_COPY_T_RAW_VAL5(x9)
  lw     x8, RT_PERIPH_COPY_T_RAW_VAL1(x9)
  beqz   x11, __rt_spim_no_pending_rx
  lw     x12, RT_PERIPH_COPY_T_RAW_VAL4(x9)
  sw     x11, UDMA_CHANNEL_SADDR_OFFSET+UDMA_CHANNEL_RX_OFFSET(x8)
  sw     x12, UDMA_CHANNEL_SIZE_OFFSET+UDMA_CHANNEL_RX_OFFSET(x8)
  li     x11 , UDMA_CHANNEL_CFG_SIZE_32 | UDMA_CHANNEL_CFG_EN
  sw     x11, UDMA_CHANNEL_CFG_OFFSET+UDMA_CHANNEL_RX_OFFSET(x8)

__rt_spim_no_pending_rx:
  // Then the command buffer
  lw     x11, RT_PERIPH_COPY_T_RAW_VAL2(x9)
  lw     x12, RT_PERIPH_COPY_T_RAW_VAL3(x9)
  sw     x11, UDMA_CHANNEL_SADDR_OFFSET+ARCHI_SPIM_CMD_OFFSET(x8)
  sw     x12, UDMA_CHANNEL_SIZE_OFFSET+ARCHI_SPIM_CMD_OFFSET(x8)
  li     x11, UDMA_CHANNEL_CFG_SIZE_32 | UDMA_CHANNEL_CFG_EN
  sw     x11, UDMA_CHANNEL_CFG_OFFSET+ARCHI_SPIM_CMD_OFFSET(x8)

  // And finally the TX buffer
  lw     x12, RT_PERIPH_COPY_T_RAW_VAL6(x9)
  lw     x11, RT_PERIPH_COPY_T_RAW_VAL4(x9)
  beqz   x12, __rt_spim_no_pending_tx
  sw     x11, UDMA_CHANNEL_SIZE_OFFSET+UDMA_CHANNEL_TX_OFFSET(x8)
  sw     x12, UDMA_CHANNEL_SADDR_OFFSET+UDMA_CHANNEL_TX_OFFSET(x8)
  li     x11 , UDMA_CHANNEL_CFG_SIZE_32 | UDMA_CHANNEL_CFG_EN
  sw     x11, UDMA_CHANNEL_CFG_OFFSET+UDMA_CHANNEL_TX_OFFSET(x8)

__rt_spim_no_pending_tx:
  lw     x11, RT_PERIPH_COPY_T_EVENT(x10)
  la     x9, __rt_fc_socevents_handler_exit
  j      __rt_event_enqueue

  