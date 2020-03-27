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

#ifndef __RT_DATA_UART_H__
#define __RT_DATA_UART_H__

typedef struct pi_uart_s {
  int open_count;
  int channel;
  int active;
  struct pi_uart_conf conf;
  rt_udma_channel_t rx_channel;
  rt_udma_channel_t tx_channel;
} pi_uart_t;

#endif
