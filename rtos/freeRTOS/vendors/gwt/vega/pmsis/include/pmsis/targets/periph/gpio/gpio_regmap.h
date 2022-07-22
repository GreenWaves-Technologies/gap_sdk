
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

#ifndef __ARCHI_GPIO_REGMAP__
#define __ARCHI_GPIO_REGMAP__

#if !defined(LANGUAGE_ASSEMBLY) && !defined(__ASSEMBLER__)

#include <stdint.h>

#endif




//
// REGISTERS GLOBAL STRUCT
//

#if !defined(LANGUAGE_ASSEMBLY) && !defined(__ASSEMBLER__)

/** GPIO_Type Register Layout Typedef */
typedef struct {
    volatile uint32_t paddir_00_31;  // GPIO pad direction configuration register.
    volatile uint32_t gpioen_00_31;  // GPIO enable register.
    volatile uint32_t padin_00_31;  // GPIO pad input value register.
    volatile uint32_t padout_00_31;  // GPIO pad output value register.
    volatile uint32_t padoutset_00_31;  // GPIO pad output set register.
    volatile uint32_t padoutclr_00_31;  // GPIO pad output clear register.
    volatile uint32_t inten_00_31;  // GPIO pad interrupt enable configuration register.
    volatile uint32_t inttype_00_15;  // GPIO pad interrupt type gpio 0 to 15 register.
    volatile uint32_t inttype_16_31;  // GPIO pad interrupt type gpio 16 to 31 register.
    volatile uint32_t intstatus_00_31;  // GPIO pad interrupt status register.
    volatile uint32_t padcfg_00_07;  // GPIO pad pin 0 to 7 configuration register.
    volatile uint32_t padcfg_08_15;  // GPIO pad pin 8 to 15 configuration register.
    volatile uint32_t padcfg_16_23;  // GPIO pad pin 16 to 23 configuration register.
    volatile uint32_t padcfg_24_31;  // GPIO pad pin 24 to 31 configuration register.
    volatile uint32_t paddir_32_63;  // GPIO pad direction configuration register.
    volatile uint32_t gpioen_32_63;  // GPIO enable register.
    volatile uint32_t padin_32_63;  // GPIO pad input value register.
    volatile uint32_t padout_32_63;  // GPIO pad output value register.
    volatile uint32_t padoutset_32_63;  // GPIO pad output set register.
    volatile uint32_t padoutclr_32_63;  // GPIO pad output clear register.
    volatile uint32_t inten_32_63;  // GPIO pad interrupt enable configuration register.
    volatile uint32_t inttype_32_47;  // GPIO pad interrupt type gpio 0 to 15 register.
    volatile uint32_t inttype_48_63;  // GPIO pad interrupt type gpio 16 to 31 register.
    volatile uint32_t intstatus_32_63;  // GPIO pad interrupt status register.
    volatile uint32_t padcfg_32_39;  // GPIO pad pin 0 to 7 configuration register.
    volatile uint32_t padcfg_40_47;  // GPIO pad pin 8 to 15 configuration register.
    volatile uint32_t padcfg_48_55;  // GPIO pad pin 16 to 23 configuration register.
    volatile uint32_t padcfg_56_63;  // GPIO pad pin 24 to 31 configuration register.
} __attribute__((packed)) gpio_t;

#endif  /* LANGUAGE_ASSEMBLY || __ASSEMBLER__ */

#endif
