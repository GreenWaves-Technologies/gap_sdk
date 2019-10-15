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
#include "pmsis/data/i2c.h"


  // x9: channel, x10: event, x8,x11,x12:temp
  .global __rt_i2c_handle_tx_copy
__rt_i2c_handle_tx_copy:

  slli   x8, x10, 2
  lw     x8, %tiny(__rt_udma_channels)(x8)

  lw  	x11, PI_I2C_T_PENDING_STEP(x8)
  jr    x11

  .global __rt_i2c_step1
__rt_i2c_step1:
  // The current copy was enqueued to configure i2c, cs and send command
  // now we need to reenqueue the same copy with the user buffer.
  lw          x12, PI_I2C_T_PENDING_BASE(x8)
  lw          x10, PI_I2C_T_PENDING_DATA(x8)
  lw          x11, PI_I2C_T_PENDING_LENGTH(x8)
  sw          x10, UDMA_CHANNEL_SADDR_OFFSET(x12)
  sw          x11, UDMA_CHANNEL_SIZE_OFFSET(x12)
  lw          x11, PI_I2C_T_PENDING_NEXT_STEP(x8)
  li          x10, UDMA_CHANNEL_CFG_EN
  sw          x11, PI_I2C_T_PENDING_STEP(x8)
  sw          x10, UDMA_CHANNEL_CFG_OFFSET(x12)

  j           udma_event_handler_end

  .global __rt_i2c_step2
__rt_i2c_step2:
  // Now that the user data has been pushed, we must push a STOP command
  la          x10, __rt_i2c_step3
  sw          x10, PI_I2C_T_PENDING_STEP(x8)
  
  lw          x12, PI_I2C_T_PENDING_BASE(x8)
  addi        x10, x8, PI_I2C_T_UDMA_STOP_CMD
  sw          x10, UDMA_CHANNEL_SADDR_OFFSET(x12)
  li          x10, 1
  sw          x10, UDMA_CHANNEL_SIZE_OFFSET(x12)
  li          x10, UDMA_CHANNEL_CFG_EN
  sw          x10, UDMA_CHANNEL_CFG_OFFSET(x12)

  j           udma_event_handler_end

  .global __rt_i2c_step3
__rt_i2c_step3:
  lw        x11, PI_I2C_T_PENDING_COPY(x8)
  sw        x0, PI_I2C_T_PENDING_COPY(x8)
  //beqz      x11, __rt_hyper_handle_emu_task
  jal       x9, __rt_event_enqueue

  j           udma_event_handler_end


  // x9: channel, x10: event, x8,x11,x12:temp
  .global __rt_i2c_handle_rx_copy
__rt_i2c_handle_rx_copy:

  slli   x8, x10, 2
  lw     x8, %tiny(__rt_udma_channels)(x8)

  j      __rt_i2c_step3
