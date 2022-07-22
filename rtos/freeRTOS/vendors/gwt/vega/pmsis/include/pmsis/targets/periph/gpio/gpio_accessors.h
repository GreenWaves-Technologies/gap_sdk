
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

#ifndef __ARCHI_GPIO_ACCESSORS__
#define __ARCHI_GPIO_ACCESSORS__

#if !defined(LANGUAGE_ASSEMBLY) && !defined(__ASSEMBLER__)

#include <stdint.h>

#endif




//
// REGISTERS ACCESS FUNCTIONS
//

#if !defined(LANGUAGE_ASSEMBLY) && !defined(__ASSEMBLER__)

static inline uint32_t gpio_paddir_00_31_get(uint32_t base)
{
    return GAP_READ(base, GPIO_PADDIR_00_31_OFFSET);
}

static inline void gpio_paddir_00_31_set(uint32_t base, uint32_t value)
{
    GAP_WRITE(base, GPIO_PADDIR_00_31_OFFSET, value);
}


static inline uint32_t gpio_gpioen_00_31_get(uint32_t base)
{
    return GAP_READ(base, GPIO_GPIOEN_00_31_OFFSET);
}

static inline void gpio_gpioen_00_31_set(uint32_t base, uint32_t value)
{
    GAP_WRITE(base, GPIO_GPIOEN_00_31_OFFSET, value);
}


static inline uint32_t gpio_padin_00_31_get(uint32_t base)
{
    return GAP_READ(base, GPIO_PADIN_00_31_OFFSET);
}

static inline void gpio_padin_00_31_set(uint32_t base, uint32_t value)
{
    GAP_WRITE(base, GPIO_PADIN_00_31_OFFSET, value);
}


static inline uint32_t gpio_padout_00_31_get(uint32_t base)
{
    return GAP_READ(base, GPIO_PADOUT_00_31_OFFSET);
}

static inline void gpio_padout_00_31_set(uint32_t base, uint32_t value)
{
    GAP_WRITE(base, GPIO_PADOUT_00_31_OFFSET, value);
}


static inline uint32_t gpio_padoutset_00_31_get(uint32_t base)
{
    return GAP_READ(base, GPIO_PADOUTSET_00_31_OFFSET);
}

static inline void gpio_padoutset_00_31_set(uint32_t base, uint32_t value)
{
    GAP_WRITE(base, GPIO_PADOUTSET_00_31_OFFSET, value);
}


static inline uint32_t gpio_padoutclr_00_31_get(uint32_t base)
{
    return GAP_READ(base, GPIO_PADOUTCLR_00_31_OFFSET);
}

static inline void gpio_padoutclr_00_31_set(uint32_t base, uint32_t value)
{
    GAP_WRITE(base, GPIO_PADOUTCLR_00_31_OFFSET, value);
}


static inline uint32_t gpio_inten_00_31_get(uint32_t base)
{
    return GAP_READ(base, GPIO_INTEN_00_31_OFFSET);
}

static inline void gpio_inten_00_31_set(uint32_t base, uint32_t value)
{
    GAP_WRITE(base, GPIO_INTEN_00_31_OFFSET, value);
}


static inline uint32_t gpio_inttype_00_15_get(uint32_t base)
{
    return GAP_READ(base, GPIO_INTTYPE_00_15_OFFSET);
}

static inline void gpio_inttype_00_15_set(uint32_t base, uint32_t value)
{
    GAP_WRITE(base, GPIO_INTTYPE_00_15_OFFSET, value);
}


static inline uint32_t gpio_inttype_16_31_get(uint32_t base)
{
    return GAP_READ(base, GPIO_INTTYPE_16_31_OFFSET);
}

static inline void gpio_inttype_16_31_set(uint32_t base, uint32_t value)
{
    GAP_WRITE(base, GPIO_INTTYPE_16_31_OFFSET, value);
}


static inline uint32_t gpio_intstatus_00_31_get(uint32_t base)
{
    return GAP_READ(base, GPIO_INTSTATUS_00_31_OFFSET);
}

static inline void gpio_intstatus_00_31_set(uint32_t base, uint32_t value)
{
    GAP_WRITE(base, GPIO_INTSTATUS_00_31_OFFSET, value);
}


static inline uint32_t gpio_padcfg_00_07_get(uint32_t base)
{
    return GAP_READ(base, GPIO_PADCFG_00_07_OFFSET);
}

static inline void gpio_padcfg_00_07_set(uint32_t base, uint32_t value)
{
    GAP_WRITE(base, GPIO_PADCFG_00_07_OFFSET, value);
}


static inline uint32_t gpio_padcfg_08_15_get(uint32_t base)
{
    return GAP_READ(base, GPIO_PADCFG_08_15_OFFSET);
}

static inline void gpio_padcfg_08_15_set(uint32_t base, uint32_t value)
{
    GAP_WRITE(base, GPIO_PADCFG_08_15_OFFSET, value);
}


static inline uint32_t gpio_padcfg_16_23_get(uint32_t base)
{
    return GAP_READ(base, GPIO_PADCFG_16_23_OFFSET);
}

static inline void gpio_padcfg_16_23_set(uint32_t base, uint32_t value)
{
    GAP_WRITE(base, GPIO_PADCFG_16_23_OFFSET, value);
}


static inline uint32_t gpio_padcfg_24_31_get(uint32_t base)
{
    return GAP_READ(base, GPIO_PADCFG_24_31_OFFSET);
}

static inline void gpio_padcfg_24_31_set(uint32_t base, uint32_t value)
{
    GAP_WRITE(base, GPIO_PADCFG_24_31_OFFSET, value);
}


static inline uint32_t gpio_paddir_32_63_get(uint32_t base)
{
    return GAP_READ(base, GPIO_PADDIR_32_63_OFFSET);
}

static inline void gpio_paddir_32_63_set(uint32_t base, uint32_t value)
{
    GAP_WRITE(base, GPIO_PADDIR_32_63_OFFSET, value);
}


static inline uint32_t gpio_gpioen_32_63_get(uint32_t base)
{
    return GAP_READ(base, GPIO_GPIOEN_32_63_OFFSET);
}

static inline void gpio_gpioen_32_63_set(uint32_t base, uint32_t value)
{
    GAP_WRITE(base, GPIO_GPIOEN_32_63_OFFSET, value);
}


static inline uint32_t gpio_padin_32_63_get(uint32_t base)
{
    return GAP_READ(base, GPIO_PADIN_32_63_OFFSET);
}

static inline void gpio_padin_32_63_set(uint32_t base, uint32_t value)
{
    GAP_WRITE(base, GPIO_PADIN_32_63_OFFSET, value);
}


static inline uint32_t gpio_padout_32_63_get(uint32_t base)
{
    return GAP_READ(base, GPIO_PADOUT_32_63_OFFSET);
}

static inline void gpio_padout_32_63_set(uint32_t base, uint32_t value)
{
    GAP_WRITE(base, GPIO_PADOUT_32_63_OFFSET, value);
}


static inline uint32_t gpio_padoutset_32_63_get(uint32_t base)
{
    return GAP_READ(base, GPIO_PADOUTSET_32_63_OFFSET);
}

static inline void gpio_padoutset_32_63_set(uint32_t base, uint32_t value)
{
    GAP_WRITE(base, GPIO_PADOUTSET_32_63_OFFSET, value);
}


static inline uint32_t gpio_padoutclr_32_63_get(uint32_t base)
{
    return GAP_READ(base, GPIO_PADOUTCLR_32_63_OFFSET);
}

static inline void gpio_padoutclr_32_63_set(uint32_t base, uint32_t value)
{
    GAP_WRITE(base, GPIO_PADOUTCLR_32_63_OFFSET, value);
}


static inline uint32_t gpio_inten_32_63_get(uint32_t base)
{
    return GAP_READ(base, GPIO_INTEN_32_63_OFFSET);
}

static inline void gpio_inten_32_63_set(uint32_t base, uint32_t value)
{
    GAP_WRITE(base, GPIO_INTEN_32_63_OFFSET, value);
}


static inline uint32_t gpio_inttype_32_47_get(uint32_t base)
{
    return GAP_READ(base, GPIO_INTTYPE_32_47_OFFSET);
}

static inline void gpio_inttype_32_47_set(uint32_t base, uint32_t value)
{
    GAP_WRITE(base, GPIO_INTTYPE_32_47_OFFSET, value);
}


static inline uint32_t gpio_inttype_48_63_get(uint32_t base)
{
    return GAP_READ(base, GPIO_INTTYPE_48_63_OFFSET);
}

static inline void gpio_inttype_48_63_set(uint32_t base, uint32_t value)
{
    GAP_WRITE(base, GPIO_INTTYPE_48_63_OFFSET, value);
}


static inline uint32_t gpio_intstatus_32_63_get(uint32_t base)
{
    return GAP_READ(base, GPIO_INTSTATUS_32_63_OFFSET);
}

static inline void gpio_intstatus_32_63_set(uint32_t base, uint32_t value)
{
    GAP_WRITE(base, GPIO_INTSTATUS_32_63_OFFSET, value);
}


static inline uint32_t gpio_padcfg_32_39_get(uint32_t base)
{
    return GAP_READ(base, GPIO_PADCFG_32_39_OFFSET);
}

static inline void gpio_padcfg_32_39_set(uint32_t base, uint32_t value)
{
    GAP_WRITE(base, GPIO_PADCFG_32_39_OFFSET, value);
}


static inline uint32_t gpio_padcfg_40_47_get(uint32_t base)
{
    return GAP_READ(base, GPIO_PADCFG_40_47_OFFSET);
}

static inline void gpio_padcfg_40_47_set(uint32_t base, uint32_t value)
{
    GAP_WRITE(base, GPIO_PADCFG_40_47_OFFSET, value);
}


static inline uint32_t gpio_padcfg_48_55_get(uint32_t base)
{
    return GAP_READ(base, GPIO_PADCFG_48_55_OFFSET);
}

static inline void gpio_padcfg_48_55_set(uint32_t base, uint32_t value)
{
    GAP_WRITE(base, GPIO_PADCFG_48_55_OFFSET, value);
}


static inline uint32_t gpio_padcfg_56_63_get(uint32_t base)
{
    return GAP_READ(base, GPIO_PADCFG_56_63_OFFSET);
}

static inline void gpio_padcfg_56_63_set(uint32_t base, uint32_t value)
{
    GAP_WRITE(base, GPIO_PADCFG_56_63_OFFSET, value);
}


#endif

#endif
