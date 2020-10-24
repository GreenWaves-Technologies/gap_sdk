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

#ifndef __ARCHI_CHIPS_USOC_V1_GPIO_H__
#define __ARCHI_CHIPS_USOC_V1_GPIO_H__
 
#define ARCHI_GPIO_PADIN     0x0    // i:0..31, Bit[i] Received bit from GPIOi
#define ARCHI_GPIO_PADOUT    0x4    // i:0..31, Bit[i] Bit to transmit through GPIOi
#define ARCHI_GPIO_INTEN     0x8    // i:0..31, Bit[i]=1 Enable Event for GPIOi
#define ARCHI_GPIO_INTTYPE0  0xC   // Interrupt Condition	i:0..15, Bit[2*i:2*i+1] = 00 Falling edge		on GPIOi
                                  // = 01 Raising edge		on GPIOi
                                  // = 11 Raising then falling edge	on GPIOi
#define ARCHI_GPIO_INTTYPE1  0x10   // Interrupt Condition	i:0..15, Bit[2*i:2*i+1] = 00 Falling edge		on GPIOi+16
                                  // = 01 Raising edge		on GPIOi+16
                                  // = 11 Raising the falling edge	on GPIOi+16
#define ARCHI_GPIO_INTSTATUS 0x14   // Interrupt Status	i:0..31, Bit[i]=1 Event received for GPIOi, bit is cleared when readen
#define ARCHI_GPIO_EN        0x18

#define ARCHI_GPIO_PADOUTSET 0x1C
#define ARCHI_GPIO_PADOUTCLR 0x20


#define ARCHI_GPIO_PADDIR_IN  0
#define ARCHI_GPIO_PADDIR_OUT 1

#define ARCHI_GPIO_INTTYPE_RISE          1
#define ARCHI_GPIO_INTTYPE_FALL          0
#define ARCHI_GPIO_INTTYPE_RISE_AND_FALL 2

#define ARCHI_GPIO_INTTYPE(id)           (ARCHI_GPIO_INTTYPE0 + (id)*4)
#define ARCHI_GPIO_INTTYPE_NO(gpio)      ((gpio) >> 4)
#define ARCHI_GPIO_INTTYPE_GPIO(inttype) ((inttype)*16)
#define ARCHI_GPIO_INTTYPE_SIZE          2
#define ARCHI_GPIO_INTTYPE_BIT(pad)      (((pad) & 0xF) << 1)
#define ARCHI_GPIO_INTTYPE_GET(gpio,value) (((value) >> ARCHI_GPIO_INTTYPE_BIT(gpio)) & ((1<<ARCHI_GPIO_INTTYPE_SIZE) - 1))

#endif