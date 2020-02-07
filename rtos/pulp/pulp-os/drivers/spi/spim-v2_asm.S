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

#include "pmsis/data/spi.h"
#include "rt/rt_data.h"
#include "archi/pulp.h"

  // x9: channel, x10:event, x8,x11,x12:temp
  .global __pi_spim_handle_eot
__pi_spim_handle_eot:
	
  // End of transfer notified through EOT, just compute the corresponding
  // event so that we can get the SPI context as for events

  addi   x8, x10, (- ARCHI_SOC_EVENT_SPIM0_EOT + ARCHI_UDMA_SPIM_ID(0))
  slli   x10, x8, UDMA_NB_PERIPH_EVENTS_LOG2

  // x9: channel, x10: event, x8,x11,x12:temp
  .global __pi_spim_handle_copy
__pi_spim_handle_copy:

  // Get the SPI context
  slli   x8, x10, 2  
  lw     x8, %tiny(__rt_udma_channels)(x8)

  // Check if we have to repeat the current transfer
  lw     x11, PI_SPIM_T_REPEAT_LEN(x8)
  beqz   x11, __rt_spim_no_repeat

  la     x9, udma_event_handler_end
  la     x12, __rt_spi_handle_repeat
  mv     x10, x8

  j      __rt_call_external_c_function

__rt_spim_no_repeat:
  // Check if we have to handle a waiting copy
  lw     x11, PI_SPIM_T_WAITING_FIRST(x8)
  bnez   x11, __rt_spim_handle_waiting

  // And handle the end-of-transfer task
  lw     x11, PI_SPIM_T_PENDING_COPY(x8)
  sw     x0, PI_SPIM_T_PENDING_COPY(x8)

  la     x9, udma_event_handler_end
  j      __rt_event_enqueue


__rt_spim_handle_waiting:
  lw     x11, PI_SPIM_T_PENDING_COPY(x8)
  sw     x0, PI_SPIM_T_PENDING_COPY(x8)
  jal    x9, __rt_event_enqueue

  // Get the current and move the first waiting one to current one
  lw     x10, PI_SPIM_T_WAITING_FIRST(x8)
  lw     x12, PI_TASK_T_NEXT(x10)
  sw     x12, PI_SPIM_T_WAITING_FIRST(x8)

  la     x9, udma_event_handler_end
  la     x12, __pi_handle_waiting_copy

  j      __rt_call_external_c_function







  .global __rt_spim_handle_tx_copy

  // x9: channel, x10:event, x8,x11,x12:temp
__rt_spim_handle_tx_copy:

  // This handler is only used for CS keep mode where
  // EOT can not be used for the termination
  // TX soc event was temporarly activated due to that, now we
  // have to deactivate it

  li     x12, ARCHI_SOC_EU_ADDR
  lw     x11, SOC_FC_MASK_LSB(x12)
  li     x8, 1
  sll    x8, x8, x10
  or     x8, x8, x11
  sw     x8, SOC_FC_MASK_LSB(x12)

  j      __pi_spim_handle_copy





  .global __rt_spim_handle_rx_copy

  // x9: channel, x10:event, x8,x11,x12:temp
__rt_spim_handle_rx_copy:

  // This handler is only used for CS keep mode where
  // EOT can not be used for the termination
  // TX soc event was temporarly activated due to that, now we
  // have to deactivate it

  li     x12, ARCHI_SOC_EU_ADDR
  lw     x11, SOC_FC_MASK_LSB(x12)
  li     x8, 1
  sll    x8, x8, x10
  or     x8, x8, x11
  sw     x8, SOC_FC_MASK_LSB(x12)

  j      __pi_spim_handle_copy



  .global __pos_spim_receive_handle_temp_buffer
__pos_spim_receive_handle_temp_buffer:
  la     x9, udma_event_handler_end
  la     x12, pos_spim_receive_handle_temp_buffer

  j      __rt_call_external_c_function


  .global __pos_spim_receive_enqueue_transfer
__pos_spim_receive_enqueue_transfer:
  la     x9, udma_event_handler_end
  la     x12, pos_spim_receive_enqueue_transfer

  j      __rt_call_external_c_function


  .global __pos_spim_send_enqueue_transfer
__pos_spim_send_enqueue_transfer:
  la     x9, udma_event_handler_end
  la     x12, pos_spim_send_enqueue_transfer

  j      __rt_call_external_c_function


  .global __pos_spim_handle_copy_eot
__pos_spim_handle_copy_eot:
  la     x9, udma_event_handler_end
  la     x12, pos_spim_handle_copy_eot

  j      __rt_call_external_c_function
