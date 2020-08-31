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

#include "rt/rt_api.h"

#ifndef RT_FC_STACK_SIZE
#define RT_FC_STACK_SIZE 0x800
#endif

__attribute__((weak)) unsigned int __rt_platform = 3;
#ifndef __RT_IODEV__
__attribute__((weak)) unsigned int __rt_iodev = 0;
#else
__attribute__((weak)) unsigned int __rt_iodev = __RT_IODEV__;
#endif
__attribute__((weak)) unsigned int __rt_iodev_uart_baudrate = 115200;
__attribute__((weak)) unsigned int __rt_iodev_uart_channel = 0;

__attribute__((weak)) unsigned int __rt_fc_stack_size = RT_FC_STACK_SIZE;

__attribute__((aligned(8))) PI_FC_L1 unsigned char __rt_fc_stack[RT_FC_STACK_SIZE];