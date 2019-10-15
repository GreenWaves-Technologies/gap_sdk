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


#ifndef HYPER_EXT_ADDR_CHANNEL_CUSTOM_OFFSET
#define HYPER_EXT_ADDR_CHANNEL_CUSTOM_OFFSET HYPER_EXT_ADDR_OFFSET
#endif


  .global udma_event_handler
udma_event_handler:

  // TODO this should disappearas soon as all peripherals are
  // using the new periph structure
  
  // We have the channel ID in x10, get pointer to the corresponding channel

#if defined(ARCHI_SOC_EVENT_UDMA_NB_CHANNEL_EVT) && ARCHI_SOC_EVENT_UDMA_NB_CHANNEL_EVT > 2
  andi   x8, x10, 1
  srli   x12, x10, 1
  or     x12, x12, x8
#endif

  la     x8, periph_channels
  slli   x9, x12, RT_PERIPH_CHANNEL_T_SIZEOF_LOG2
  add    x9, x9, x8


  // Dequeue the transfer which have just finished and mark it as done
  lw   x8, RT_PERIPH_CHANNEL_T_FIRST(x9)
  lw   x11, RT_PERIPH_CHANNEL_T_FIRST_TO_ENQUEUE(x9)   // This is used later on, just put here to fill the slot
  beq  x8, x0, __rt_udma_no_copy                       // Special case where there is no copy, just register the event in the bitfield
  lw   x12, RT_PERIPH_COPY_T_REPEAT(x8)
  lw   x10, RT_PERIPH_COPY_T_NEXT(x8)
  bne  x12, x0, repeat_transfer
  sw   x10, RT_PERIPH_CHANNEL_T_FIRST(x9)
  
  // Handle any special end-of-transfer control
  lw       x10, RT_PERIPH_COPY_T_CTRL(x8)
  bnez     x10, handle_special_end
resume_after_special_end:


  // Now check if there are some transfers enqueued in the SW FIFO to be enqueued to the UDMA
  beq x11, x0, checkTask

  // x9 contains the pointer to the channel and x11 the first copy

  // Update the FIFO pointers and just copy from node to UDMA
  lw  x12, RT_PERIPH_COPY_T_ENQUEUE_CALLBACK(x11)
  lw  x10, RT_PERIPH_COPY_T_NEXT(x11)
  beqz x12, __rt_udma_call_enqueue_callback_resume

  la  x9, __rt_udma_call_enqueue_callback_resume
  jr  x12

__rt_udma_call_enqueue_callback_resume:
  lw  x12, RT_PERIPH_CHANNEL_T_BASE(x9)
  sw  x10, RT_PERIPH_CHANNEL_T_FIRST_TO_ENQUEUE(x9)
  lw  x10, RT_PERIPH_COPY_T_ADDR(x11)
  lw  x9, RT_PERIPH_COPY_T_SIZE(x11)
  sw  x10, UDMA_CHANNEL_SADDR_OFFSET(x12)
  sw  x9, UDMA_CHANNEL_SIZE_OFFSET(x12)

  lw  x9, RT_PERIPH_COPY_T_CTRL(x11)
  andi x9, x9, (1<<RT_PERIPH_COPY_CTRL_TYPE_WIDTH)-1
  li  x10, RT_PERIPH_COPY_SPECIAL_ENQUEUE_THRESHOLD
  blt x9, x10, transfer_resume


#ifdef RV_ISA_RV32
  li          x10, RT_PERIPH_COPY_HYPER
  beq         x9, x10, hyper
  li          x10, RT_PERIPH_COPY_FC_TCDM
  beq         x9, x10, fc_tcdm
#else
  p.beqimm      x9, RT_PERIPH_COPY_HYPER, hyper
  p.beqimm      x9, RT_PERIPH_COPY_FC_TCDM, fc_tcdm
#endif


dual:
#if 0
  lw  x10, RT_PERIPH_COPY_T_RX_L2ADDR(x11)
  lw  x9, RT_PERIPH_COPY_T_RX_SIZE(x11)
  sw  x10, UDMA_CHANNEL_SADDR_OFFSET-UDMA_CHANNEL_TX_OFFSET(x12)
  lw  x10, RT_PERIPH_COPY_T_CFG(x11)
  sw  x9, UDMA_CHANNEL_SIZE_OFFSET-UDMA_CHANNEL_TX_OFFSET(x12)
  sw  x10, UDMA_CHANNEL_CFG_OFFSET-UDMA_CHANNEL_TX_OFFSET(x12)
  j   transfer_resume
#endif

fc_tcdm:
#ifdef RT_PERIPH_HAS_TCDM
#ifdef RV_ISA_RV32
  la    x10, ~(1<<UDMA_CHANNEL_SIZE_LOG2)
  and   x9, x12, x10
  lw    x10, RT_PERIPH_COPY_T_EXT_ADDR(x11)
#else
  lw    x10, RT_PERIPH_COPY_T_EXT_ADDR(x11)
  p.bclr  x9, x12, 0, UDMA_CHANNEL_SIZE_LOG2
#endif
  beq   x9, x12, fcTcdm_rx
  sw    x10, UDMA_CHANNEL_CUSTOM_OFFSET+TCDM_T_DST_SADDR(x9)
  j   transfer_resume
fcTcdm_rx:
  sw    x10, UDMA_CHANNEL_CUSTOM_OFFSET+TCDM_T_SRC_SADDR(x9)
  j   transfer_resume
#endif


hyper:
#ifdef ARCHI_UDMA_HAS_HYPER
#ifdef RV_ISA_RV32
  li    x10, ~(1<<UDMA_CHANNEL_SIZE_LOG2)
  and   x9, x12, x10
  lw    x10, RT_PERIPH_COPY_T_HYPER_ADDR(x11)
#else
  lw    x10, RT_PERIPH_COPY_T_HYPER_ADDR(x11)
  p.bclr  x9, x12, 0, UDMA_CHANNEL_SIZE_LOG2
#endif
  sw    x10, HYPER_EXT_ADDR_CHANNEL_CUSTOM_OFFSET(x9)
#endif

transfer_resume:
  lw  x10, RT_PERIPH_COPY_T_CFG(x11)
  sw  x10, UDMA_CHANNEL_CFG_OFFSET(x12)

checkTask:

  // Check if we have a DMA transfer from L2 to L1   
  //lw          x10, RT_PERIPH_COPY_T_DMACMD(x8)           // Not null if we must transfer
  lw          x11, RT_PERIPH_COPY_T_EVENT(x8)             // Read this in advance to fill the slot, it is used later on in case there is no DMA command

  //bne         x10, zero, dmaCmd
  la          x9, __rt_fc_socevents_handler_exit
  bne         x11, zero, __rt_event_enqueue

  // Loop again in case there are still events in the FIFO
  j __rt_fc_socevents_handler_exit

dmaCmd:

#if 0
  // Detected L2 to L1 copy, enqueue to DMA
  la          x11, DEMUX_PERIPHERALS_BASE_ADDR                            // x11 = DMA base
  lw          x9, MCHAN_BASE_OFFSET+PLP_DMA_QUEUE_OFFSET(x11)             // x9 = allocated DMA counter
  sw          x10, MCHAN_BASE_OFFSET+PLP_DMA_QUEUE_OFFSET(x11)             // store DMA command to DMA from x10
  sw          x9, RT_PERIPH_COPY_T_DMACOPY+PLP_DMA_COPY_T_COUNTER(x8)     // store DMA counter to DMA from x9
  lw          x10, RT_PERIPH_COPY_T_L1ADDR(x8)                             // x10 = L1 transfer addr
  lw          x9, RT_PERIPH_COPY_T_ADDR(x8)                             // x9 = L2 transfer addr
  sw          x10, MCHAN_BASE_OFFSET+PLP_DMA_QUEUE_OFFSET(x11)             // store L1 addr to DMA from x10
  sw          x9, MCHAN_BASE_OFFSET+PLP_DMA_QUEUE_OFFSET(x11)             // store L2 addr to DMA from x9

  // Updated DMA pending copies linked list
  lw          x11, %tiny(firstDmaCopy)(x0)          // x11 = firstDmaCopy
  addi        x10, x8, RT_PERIPH_COPY_T_DMACOPY      // x10 = DMA copy address computed from x8
  beq         x11, x0, isFirst                      // check if we don't have any pending copy
  lw          x11, %tiny(lastDmaCopy)(x0)           // x11 = lastDmaCopy
  sw          x10, PLP_DMA_COPY_T_NEXT(x11)          // store copy in x10 to lastDmaCopy->next
  j           resume
isFirst:   
  sw          x10, %tiny(firstDmaCopy)(x0)          // It is the first copy, just store it to firstDmaCopy
resume:
  sw          x10, %tiny(lastDmaCopy)(x0)           // store copy in x10 in lastDmaCopy
  sw          x0, PLP_DMA_COPY_T_NEXT(x10)
  j           __rt_fc_socevents_handler_exit
#endif



// Registers content
//   x8  : current copy
//   x9  : pointer to channel
//   x12 : number of bytes to repeat
repeat_transfer:

#ifdef ARCHI_UDMA_HAS_HYPER

  lw      x11, RT_PERIPH_CHANNEL_T_BASE(x9)
  
#ifdef RV_ISA_RV32
  li      x10, ~(1<<UDMA_CHANNEL_SIZE_LOG2)
  and     x9, x11, x10
  lw      x10, RT_PERIPH_COPY_T_HYPER_ADDR(x8)
#else
  lw      x10, RT_PERIPH_COPY_T_HYPER_ADDR(x8)
  p.bclr  x9, x11, 0, UDMA_CHANNEL_SIZE_LOG2
#endif
  add     x10, x10, x12
  sw      x10, HYPER_EXT_ADDR_CHANNEL_CUSTOM_OFFSET(x9)
  sw      x10, RT_PERIPH_COPY_T_HYPER_ADDR(x8)

  lw      x10, RT_PERIPH_COPY_T_ADDR(x8)
  lw      x9, RT_PERIPH_COPY_T_REPEAT_SIZE(x8)
  add     x10, x10, x12
  sub     x9, x9, x12
  blt     x12, x9, not_last
  mv      x12, x9
  sw      x0, RT_PERIPH_COPY_T_REPEAT(x8)
  beq     x12, x0, __rt_fc_socevents_handler_exit

not_last:
  sw      x10, RT_PERIPH_COPY_T_ADDR(x8)
  sw      x9, RT_PERIPH_COPY_T_REPEAT_SIZE(x8)
  sw      x10, UDMA_CHANNEL_SADDR_OFFSET(x11)
  sw      x12, UDMA_CHANNEL_SIZE_OFFSET(x11)

  li      x10, UDMA_CHANNEL_CFG_EN
  sw      x10, UDMA_CHANNEL_CFG_OFFSET(x11)

#endif

  j           __rt_fc_socevents_handler_exit





__rt_udma_no_copy:
  srli    x11, x10, 5
  slli    x11, x11, 2
  lw      x12, %tiny(__rt_socevents_status)(x11)
  andi    x10, x10, 0x1f

#ifdef RV_ISA_RV32
  li      x8, 1
  sll     x10, x8, x10
  or      x12, x12, x10
#else
  p.bsetr x12, x12, x10
#endif

  sw      x12, %tiny(__rt_socevents_status)(x11)

  j __rt_fc_socevents_handler_exit
  


// This is called when the terminated transfer has a special control to do
// x10 contains the ctrl id.
// x9 contains the channel pointer
// x12 is free
handle_special_end:

// TODO there are now too many cases, switch it to a handler as on udma v1
#ifdef RV_ISA_RV32
  li          x12, RT_PERIPH_COPY_I2C_STEP1
  beq         x10, x12, i2c_step1
  li          x12, RT_PERIPH_COPY_I2C_STEP2
  beq         x10, x12, i2c_step2
#else
  p.beqimm    x10, RT_PERIPH_COPY_I2C_STEP1, i2c_step1  
  p.beqimm    x10, RT_PERIPH_COPY_I2C_STEP2, i2c_step2
#endif

  j           resume_after_special_end




/*
 * I2C termination handling
 */

i2c_step1:
  // The current copy was enqueued to configure i2c, cs and send command
  // now we need to reenqueue the same copy with the user buffer.
  lw          x10, RT_PERIPH_COPY_T_RAW_VAL1(x8)
  sw          x10, RT_PERIPH_COPY_T_CTRL(x8)

  // Reenqueue the same copy to the list of pending copies as it has been removed
  lw          x10, RT_PERIPH_CHANNEL_T_FIRST(x9)
  sw          x10, RT_PERIPH_COPY_T_NEXT(x8)
  sw          x8, RT_PERIPH_CHANNEL_T_FIRST(x9)

  // And rearm the transfer in the udma
  lw          x12, RT_PERIPH_CHANNEL_T_BASE(x9)
  lw          x10, RT_PERIPH_COPY_T_ADDR(x8)
  sw          x10, UDMA_CHANNEL_SADDR_OFFSET(x12)
  lw          x10, RT_PERIPH_COPY_T_RAW_VAL0(x8)
  sw          x10, UDMA_CHANNEL_SIZE_OFFSET(x12)
  lw          x10, RT_PERIPH_COPY_T_CFG(x8)
  sw          x10, UDMA_CHANNEL_CFG_OFFSET(x12)

  j           __rt_fc_socevents_handler_exit

i2c_step2:
  // Now that the user data has been pushed, we must push a STOP command
  sw          x0, RT_PERIPH_COPY_T_CTRL(x8)
  
  // Reenqueue the same copy to the list of pending copies as it has been removed
  lw          x10, RT_PERIPH_CHANNEL_T_FIRST(x9)
  sw          x10, RT_PERIPH_COPY_T_NEXT(x8)
  sw          x8, RT_PERIPH_CHANNEL_T_FIRST(x9)

  // And rearm the transfer in the udma
  #if PULP_CHIP == CHIP_GAP

  lw          x8, RT_PERIPH_COPY_T_PERIPH_DATA(x8)
  li          x12, I2C_CMD_STOP
  sw          x12, 0(x8)
  lw          x12, RT_PERIPH_CHANNEL_T_BASE(x9)
  sw          x8, UDMA_CHANNEL_SADDR_OFFSET(x12)

  #else

  li          x12, I2C_CMD_STOP
  sw          x12, RT_PERIPH_COPY_T_PERIPH_DATA(x8)
  lw          x12, RT_PERIPH_CHANNEL_T_BASE(x9)
  addi        x10, x8, RT_PERIPH_COPY_T_PERIPH_DATA
  sw          x10, UDMA_CHANNEL_SADDR_OFFSET(x12)

  #endif

  li          x10, 1
  sw          x10, UDMA_CHANNEL_SIZE_OFFSET(x12)
  li          x10, UDMA_CHANNEL_CFG_EN
  sw          x10, UDMA_CHANNEL_CFG_OFFSET(x12)

  j           __rt_fc_socevents_handler_exit



