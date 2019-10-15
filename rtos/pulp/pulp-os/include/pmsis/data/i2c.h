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

#ifndef __RT_DATA_I2C_H__
#define __RT_DATA_I2C_H__

#ifndef LANGUAGE_ASSEMBLY

typedef struct {
  pi_task_t *pending_copy;
  pi_task_t *waiting_first;
  pi_task_t *waiting_last;
  uint32_t pending_base;
  uint32_t pending_data;
  uint32_t pending_length;
  uint32_t pending_step;
  uint32_t pending_next_step;
  unsigned int udma_stop_cmd;
  unsigned char udma_cmd[16];
  int channel;
  unsigned char open_count;
  char cs;
  unsigned int  max_baudrate;
  unsigned int  div;
} pi_i2c_t;

#endif

#define PI_I2C_T_PENDING_COPY      0
#define PI_I2C_T_WAITING_FIRST     4
#define PI_I2C_T_WAITING_LAST      8
#define PI_I2C_T_PENDING_BASE      12
#define PI_I2C_T_PENDING_DATA      16
#define PI_I2C_T_PENDING_LENGTH    20
#define PI_I2C_T_PENDING_STEP      24
#define PI_I2C_T_PENDING_NEXT_STEP 28
#define PI_I2C_T_UDMA_STOP_CMD     32

#endif
