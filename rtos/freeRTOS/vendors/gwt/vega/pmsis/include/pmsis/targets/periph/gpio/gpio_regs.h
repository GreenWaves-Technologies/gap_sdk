
/* THIS FILE HAS BEEN GENERATED, DO NOT MODIFY IT.
 */

/*
 * Copyright (C) 2020 GreenWaves Technologies
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

#ifndef __ARCHI_GPIO_REGS__
#define __ARCHI_GPIO_REGS__

#if !defined(LANGUAGE_ASSEMBLY) && !defined(__ASSEMBLER__)


#endif




//
// REGISTERS
//

// GPIO pad direction configuration register.
#define GPIO_PADDIR_00_31_OFFSET                 0x0

// GPIO enable register.
#define GPIO_GPIOEN_00_31_OFFSET                 0x4

// GPIO pad input value register.
#define GPIO_PADIN_00_31_OFFSET                  0x8

// GPIO pad output value register.
#define GPIO_PADOUT_00_31_OFFSET                 0xc

// GPIO pad output set register.
#define GPIO_PADOUTSET_00_31_OFFSET              0x10

// GPIO pad output clear register.
#define GPIO_PADOUTCLR_00_31_OFFSET              0x14

// GPIO pad interrupt enable configuration register.
#define GPIO_INTEN_00_31_OFFSET                  0x18

// GPIO pad interrupt type gpio 0 to 15 register.
#define GPIO_INTTYPE_00_15_OFFSET                0x1c

// GPIO pad interrupt type gpio 16 to 31 register.
#define GPIO_INTTYPE_16_31_OFFSET                0x20

// GPIO pad interrupt status register.
#define GPIO_INTSTATUS_00_31_OFFSET              0x24

// GPIO pad pin 0 to 7 configuration register.
#define GPIO_PADCFG_00_07_OFFSET                 0x28

// GPIO pad pin 8 to 15 configuration register.
#define GPIO_PADCFG_08_15_OFFSET                 0x2c

// GPIO pad pin 16 to 23 configuration register.
#define GPIO_PADCFG_16_23_OFFSET                 0x30

// GPIO pad pin 24 to 31 configuration register.
#define GPIO_PADCFG_24_31_OFFSET                 0x34

// GPIO pad direction configuration register.
#define GPIO_PADDIR_32_63_OFFSET                 0x38

// GPIO enable register.
#define GPIO_GPIOEN_32_63_OFFSET                 0x3c

// GPIO pad input value register.
#define GPIO_PADIN_32_63_OFFSET                  0x40

// GPIO pad output value register.
#define GPIO_PADOUT_32_63_OFFSET                 0x44

// GPIO pad output set register.
#define GPIO_PADOUTSET_32_63_OFFSET              0x48

// GPIO pad output clear register.
#define GPIO_PADOUTCLR_32_63_OFFSET              0x4c

// GPIO pad interrupt enable configuration register.
#define GPIO_INTEN_32_63_OFFSET                  0x50

// GPIO pad interrupt type gpio 0 to 15 register.
#define GPIO_INTTYPE_32_47_OFFSET                0x54

// GPIO pad interrupt type gpio 16 to 31 register.
#define GPIO_INTTYPE_48_63_OFFSET                0x58

// GPIO pad interrupt status register.
#define GPIO_INTSTATUS_32_63_OFFSET              0x5c

// GPIO pad pin 0 to 7 configuration register.
#define GPIO_PADCFG_32_39_OFFSET                 0x60

// GPIO pad pin 8 to 15 configuration register.
#define GPIO_PADCFG_40_47_OFFSET                 0x64

// GPIO pad pin 16 to 23 configuration register.
#define GPIO_PADCFG_48_55_OFFSET                 0x68

// GPIO pad pin 24 to 31 configuration register.
#define GPIO_PADCFG_56_63_OFFSET                 0x6c

#endif
