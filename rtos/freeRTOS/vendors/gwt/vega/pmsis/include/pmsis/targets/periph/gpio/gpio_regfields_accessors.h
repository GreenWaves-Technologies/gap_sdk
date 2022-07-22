
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

#ifndef __ARCHI_GPIO_REGFIELDS_ACCESSORS__
#define __ARCHI_GPIO_REGFIELDS_ACCESSORS__

#if !defined(LANGUAGE_ASSEMBLY) && !defined(__ASSEMBLER__)

#include <stdint.h>
#include "archi/gap_utils.h"

#endif




//
// REGISTERS FIELDS ACCESS FUNCTIONS
//

#if !defined(LANGUAGE_ASSEMBLY) && !defined(__ASSEMBLER__)

static inline void gpio_paddir_00_31_paddir_set(uint32_t base, uint32_t value)
{
    GAP_WRITE(base, GPIO_PADDIR_00_31_OFFSET, value);
}

static inline uint32_t gpio_paddir_00_31_paddir_get(uint32_t base)
{
    return GAP_READ(base, GPIO_PADDIR_00_31_OFFSET);
}

static inline int32_t gpio_paddir_00_31_paddir_gets(uint32_t base)
{
    return GAP_READ(base, GPIO_PADDIR_00_31_OFFSET);
}


static inline void gpio_gpioen_00_31_gpioen_set(uint32_t base, uint32_t value)
{
    GAP_WRITE(base, GPIO_GPIOEN_00_31_OFFSET, value);
}

static inline uint32_t gpio_gpioen_00_31_gpioen_get(uint32_t base)
{
    return GAP_READ(base, GPIO_GPIOEN_00_31_OFFSET);
}

static inline int32_t gpio_gpioen_00_31_gpioen_gets(uint32_t base)
{
    return GAP_READ(base, GPIO_GPIOEN_00_31_OFFSET);
}


static inline void gpio_padin_00_31_padin_set(uint32_t base, uint32_t value)
{
    GAP_WRITE(base, GPIO_PADIN_00_31_OFFSET, value);
}

static inline uint32_t gpio_padin_00_31_padin_get(uint32_t base)
{
    return GAP_READ(base, GPIO_PADIN_00_31_OFFSET);
}

static inline int32_t gpio_padin_00_31_padin_gets(uint32_t base)
{
    return GAP_READ(base, GPIO_PADIN_00_31_OFFSET);
}


static inline void gpio_padout_00_31_padout_set(uint32_t base, uint32_t value)
{
    GAP_WRITE(base, GPIO_PADOUT_00_31_OFFSET, value);
}

static inline uint32_t gpio_padout_00_31_padout_get(uint32_t base)
{
    return GAP_READ(base, GPIO_PADOUT_00_31_OFFSET);
}

static inline int32_t gpio_padout_00_31_padout_gets(uint32_t base)
{
    return GAP_READ(base, GPIO_PADOUT_00_31_OFFSET);
}


static inline void gpio_padoutset_00_31_padoutset_set(uint32_t base, uint32_t value)
{
    GAP_WRITE(base, GPIO_PADOUTSET_00_31_OFFSET, value);
}

static inline uint32_t gpio_padoutset_00_31_padoutset_get(uint32_t base)
{
    return GAP_READ(base, GPIO_PADOUTSET_00_31_OFFSET);
}

static inline int32_t gpio_padoutset_00_31_padoutset_gets(uint32_t base)
{
    return GAP_READ(base, GPIO_PADOUTSET_00_31_OFFSET);
}


static inline void gpio_padoutclr_00_31_padoutclr_set(uint32_t base, uint32_t value)
{
    GAP_WRITE(base, GPIO_PADOUTCLR_00_31_OFFSET, value);
}

static inline uint32_t gpio_padoutclr_00_31_padoutclr_get(uint32_t base)
{
    return GAP_READ(base, GPIO_PADOUTCLR_00_31_OFFSET);
}

static inline int32_t gpio_padoutclr_00_31_padoutclr_gets(uint32_t base)
{
    return GAP_READ(base, GPIO_PADOUTCLR_00_31_OFFSET);
}


static inline void gpio_inten_00_31_inten_set(uint32_t base, uint32_t value)
{
    GAP_WRITE(base, GPIO_INTEN_00_31_OFFSET, value);
}

static inline uint32_t gpio_inten_00_31_inten_get(uint32_t base)
{
    return GAP_READ(base, GPIO_INTEN_00_31_OFFSET);
}

static inline int32_t gpio_inten_00_31_inten_gets(uint32_t base)
{
    return GAP_READ(base, GPIO_INTEN_00_31_OFFSET);
}


static inline void gpio_inttype_00_15_inttype_set(uint32_t base, uint32_t value)
{
    GAP_WRITE(base, GPIO_INTTYPE_00_15_OFFSET, value);
}

static inline uint32_t gpio_inttype_00_15_inttype_get(uint32_t base)
{
    return GAP_READ(base, GPIO_INTTYPE_00_15_OFFSET);
}

static inline int32_t gpio_inttype_00_15_inttype_gets(uint32_t base)
{
    return GAP_READ(base, GPIO_INTTYPE_00_15_OFFSET);
}


static inline void gpio_inttype_16_31_inttype_set(uint32_t base, uint32_t value)
{
    GAP_WRITE(base, GPIO_INTTYPE_16_31_OFFSET, value);
}

static inline uint32_t gpio_inttype_16_31_inttype_get(uint32_t base)
{
    return GAP_READ(base, GPIO_INTTYPE_16_31_OFFSET);
}

static inline int32_t gpio_inttype_16_31_inttype_gets(uint32_t base)
{
    return GAP_READ(base, GPIO_INTTYPE_16_31_OFFSET);
}


static inline void gpio_intstatus_00_31_intstatus_set(uint32_t base, uint32_t value)
{
    GAP_WRITE(base, GPIO_INTSTATUS_00_31_OFFSET, value);
}

static inline uint32_t gpio_intstatus_00_31_intstatus_get(uint32_t base)
{
    return GAP_READ(base, GPIO_INTSTATUS_00_31_OFFSET);
}

static inline int32_t gpio_intstatus_00_31_intstatus_gets(uint32_t base)
{
    return GAP_READ(base, GPIO_INTSTATUS_00_31_OFFSET);
}


static inline void gpio_padcfg_00_03_padcfg_set(uint32_t base, uint32_t value)
{
    GAP_WRITE(base, GPIO_PADCFG_00_03_OFFSET, GAP_BINSERT(GAP_READ(base, GPIO_PADCFG_00_03_OFFSET), value, 4, 0));
}

static inline uint32_t gpio_padcfg_00_03_padcfg_get(uint32_t base)
{
    return GAP_BEXTRACTU(GAP_READ(base, GPIO_PADCFG_00_03_OFFSET), 4, 0);
}

static inline int32_t gpio_padcfg_00_03_padcfg_gets(uint32_t base)
{
    return GAP_BEXTRACT(GAP_READ(base, GPIO_PADCFG_00_03_OFFSET), 4, 0);
}


static inline void gpio_padcfg_04_07_padcfg_set(uint32_t base, uint32_t value)
{
    GAP_WRITE(base, GPIO_PADCFG_04_07_OFFSET, GAP_BINSERT(GAP_READ(base, GPIO_PADCFG_04_07_OFFSET), value, 4, 0));
}

static inline uint32_t gpio_padcfg_04_07_padcfg_get(uint32_t base)
{
    return GAP_BEXTRACTU(GAP_READ(base, GPIO_PADCFG_04_07_OFFSET), 4, 0);
}

static inline int32_t gpio_padcfg_04_07_padcfg_gets(uint32_t base)
{
    return GAP_BEXTRACT(GAP_READ(base, GPIO_PADCFG_04_07_OFFSET), 4, 0);
}


static inline void gpio_padcfg_08_11_padcfg_set(uint32_t base, uint32_t value)
{
    GAP_WRITE(base, GPIO_PADCFG_08_11_OFFSET, GAP_BINSERT(GAP_READ(base, GPIO_PADCFG_08_11_OFFSET), value, 4, 0));
}

static inline uint32_t gpio_padcfg_08_11_padcfg_get(uint32_t base)
{
    return GAP_BEXTRACTU(GAP_READ(base, GPIO_PADCFG_08_11_OFFSET), 4, 0);
}

static inline int32_t gpio_padcfg_08_11_padcfg_gets(uint32_t base)
{
    return GAP_BEXTRACT(GAP_READ(base, GPIO_PADCFG_08_11_OFFSET), 4, 0);
}


static inline void gpio_padcfg_12_15_padcfg_set(uint32_t base, uint32_t value)
{
    GAP_WRITE(base, GPIO_PADCFG_12_15_OFFSET, GAP_BINSERT(GAP_READ(base, GPIO_PADCFG_12_15_OFFSET), value, 4, 0));
}

static inline uint32_t gpio_padcfg_12_15_padcfg_get(uint32_t base)
{
    return GAP_BEXTRACTU(GAP_READ(base, GPIO_PADCFG_12_15_OFFSET), 4, 0);
}

static inline int32_t gpio_padcfg_12_15_padcfg_gets(uint32_t base)
{
    return GAP_BEXTRACT(GAP_READ(base, GPIO_PADCFG_12_15_OFFSET), 4, 0);
}


static inline void gpio_padcfg_16_19_padcfg_set(uint32_t base, uint32_t value)
{
    GAP_WRITE(base, GPIO_PADCFG_16_19_OFFSET, GAP_BINSERT(GAP_READ(base, GPIO_PADCFG_16_19_OFFSET), value, 4, 0));
}

static inline uint32_t gpio_padcfg_16_19_padcfg_get(uint32_t base)
{
    return GAP_BEXTRACTU(GAP_READ(base, GPIO_PADCFG_16_19_OFFSET), 4, 0);
}

static inline int32_t gpio_padcfg_16_19_padcfg_gets(uint32_t base)
{
    return GAP_BEXTRACT(GAP_READ(base, GPIO_PADCFG_16_19_OFFSET), 4, 0);
}


static inline void gpio_padcfg_20_23_padcfg_set(uint32_t base, uint32_t value)
{
    GAP_WRITE(base, GPIO_PADCFG_20_23_OFFSET, GAP_BINSERT(GAP_READ(base, GPIO_PADCFG_20_23_OFFSET), value, 4, 0));
}

static inline uint32_t gpio_padcfg_20_23_padcfg_get(uint32_t base)
{
    return GAP_BEXTRACTU(GAP_READ(base, GPIO_PADCFG_20_23_OFFSET), 4, 0);
}

static inline int32_t gpio_padcfg_20_23_padcfg_gets(uint32_t base)
{
    return GAP_BEXTRACT(GAP_READ(base, GPIO_PADCFG_20_23_OFFSET), 4, 0);
}


static inline void gpio_padcfg_24_27_padcfg_set(uint32_t base, uint32_t value)
{
    GAP_WRITE(base, GPIO_PADCFG_24_27_OFFSET, GAP_BINSERT(GAP_READ(base, GPIO_PADCFG_24_27_OFFSET), value, 4, 0));
}

static inline uint32_t gpio_padcfg_24_27_padcfg_get(uint32_t base)
{
    return GAP_BEXTRACTU(GAP_READ(base, GPIO_PADCFG_24_27_OFFSET), 4, 0);
}

static inline int32_t gpio_padcfg_24_27_padcfg_gets(uint32_t base)
{
    return GAP_BEXTRACT(GAP_READ(base, GPIO_PADCFG_24_27_OFFSET), 4, 0);
}


static inline void gpio_padcfg_28_31_padcfg_set(uint32_t base, uint32_t value)
{
    GAP_WRITE(base, GPIO_PADCFG_28_31_OFFSET, GAP_BINSERT(GAP_READ(base, GPIO_PADCFG_28_31_OFFSET), value, 4, 0));
}

static inline uint32_t gpio_padcfg_28_31_padcfg_get(uint32_t base)
{
    return GAP_BEXTRACTU(GAP_READ(base, GPIO_PADCFG_28_31_OFFSET), 4, 0);
}

static inline int32_t gpio_padcfg_28_31_padcfg_gets(uint32_t base)
{
    return GAP_BEXTRACT(GAP_READ(base, GPIO_PADCFG_28_31_OFFSET), 4, 0);
}


static inline void gpio_paddir_32_63_paddir_set(uint32_t base, uint32_t value)
{
    GAP_WRITE(base, GPIO_PADDIR_32_63_OFFSET, value);
}

static inline uint32_t gpio_paddir_32_63_paddir_get(uint32_t base)
{
    return GAP_READ(base, GPIO_PADDIR_32_63_OFFSET);
}

static inline int32_t gpio_paddir_32_63_paddir_gets(uint32_t base)
{
    return GAP_READ(base, GPIO_PADDIR_32_63_OFFSET);
}


static inline void gpio_gpioen_32_63_gpioen_set(uint32_t base, uint32_t value)
{
    GAP_WRITE(base, GPIO_GPIOEN_32_63_OFFSET, value);
}

static inline uint32_t gpio_gpioen_32_63_gpioen_get(uint32_t base)
{
    return GAP_READ(base, GPIO_GPIOEN_32_63_OFFSET);
}

static inline int32_t gpio_gpioen_32_63_gpioen_gets(uint32_t base)
{
    return GAP_READ(base, GPIO_GPIOEN_32_63_OFFSET);
}


static inline void gpio_padin_32_63_padin_set(uint32_t base, uint32_t value)
{
    GAP_WRITE(base, GPIO_PADIN_32_63_OFFSET, value);
}

static inline uint32_t gpio_padin_32_63_padin_get(uint32_t base)
{
    return GAP_READ(base, GPIO_PADIN_32_63_OFFSET);
}

static inline int32_t gpio_padin_32_63_padin_gets(uint32_t base)
{
    return GAP_READ(base, GPIO_PADIN_32_63_OFFSET);
}


static inline void gpio_padout_32_63_padout_set(uint32_t base, uint32_t value)
{
    GAP_WRITE(base, GPIO_PADOUT_32_63_OFFSET, value);
}

static inline uint32_t gpio_padout_32_63_padout_get(uint32_t base)
{
    return GAP_READ(base, GPIO_PADOUT_32_63_OFFSET);
}

static inline int32_t gpio_padout_32_63_padout_gets(uint32_t base)
{
    return GAP_READ(base, GPIO_PADOUT_32_63_OFFSET);
}


static inline void gpio_padoutset_32_63_padoutset_set(uint32_t base, uint32_t value)
{
    GAP_WRITE(base, GPIO_PADOUTSET_32_63_OFFSET, value);
}

static inline uint32_t gpio_padoutset_32_63_padoutset_get(uint32_t base)
{
    return GAP_READ(base, GPIO_PADOUTSET_32_63_OFFSET);
}

static inline int32_t gpio_padoutset_32_63_padoutset_gets(uint32_t base)
{
    return GAP_READ(base, GPIO_PADOUTSET_32_63_OFFSET);
}


static inline void gpio_padoutclr_32_63_padoutclr_set(uint32_t base, uint32_t value)
{
    GAP_WRITE(base, GPIO_PADOUTCLR_32_63_OFFSET, value);
}

static inline uint32_t gpio_padoutclr_32_63_padoutclr_get(uint32_t base)
{
    return GAP_READ(base, GPIO_PADOUTCLR_32_63_OFFSET);
}

static inline int32_t gpio_padoutclr_32_63_padoutclr_gets(uint32_t base)
{
    return GAP_READ(base, GPIO_PADOUTCLR_32_63_OFFSET);
}


static inline void gpio_inten_32_63_inten_set(uint32_t base, uint32_t value)
{
    GAP_WRITE(base, GPIO_INTEN_32_63_OFFSET, value);
}

static inline uint32_t gpio_inten_32_63_inten_get(uint32_t base)
{
    return GAP_READ(base, GPIO_INTEN_32_63_OFFSET);
}

static inline int32_t gpio_inten_32_63_inten_gets(uint32_t base)
{
    return GAP_READ(base, GPIO_INTEN_32_63_OFFSET);
}


static inline void gpio_inttype_32_47_inttype_set(uint32_t base, uint32_t value)
{
    GAP_WRITE(base, GPIO_INTTYPE_32_47_OFFSET, value);
}

static inline uint32_t gpio_inttype_32_47_inttype_get(uint32_t base)
{
    return GAP_READ(base, GPIO_INTTYPE_32_47_OFFSET);
}

static inline int32_t gpio_inttype_32_47_inttype_gets(uint32_t base)
{
    return GAP_READ(base, GPIO_INTTYPE_32_47_OFFSET);
}


static inline void gpio_inttype_48_63_inttype_set(uint32_t base, uint32_t value)
{
    GAP_WRITE(base, GPIO_INTTYPE_48_63_OFFSET, value);
}

static inline uint32_t gpio_inttype_48_63_inttype_get(uint32_t base)
{
    return GAP_READ(base, GPIO_INTTYPE_48_63_OFFSET);
}

static inline int32_t gpio_inttype_48_63_inttype_gets(uint32_t base)
{
    return GAP_READ(base, GPIO_INTTYPE_48_63_OFFSET);
}


static inline void gpio_intstatus_32_63_intstatus_set(uint32_t base, uint32_t value)
{
    GAP_WRITE(base, GPIO_INTSTATUS_32_63_OFFSET, value);
}

static inline uint32_t gpio_intstatus_32_63_intstatus_get(uint32_t base)
{
    return GAP_READ(base, GPIO_INTSTATUS_32_63_OFFSET);
}

static inline int32_t gpio_intstatus_32_63_intstatus_gets(uint32_t base)
{
    return GAP_READ(base, GPIO_INTSTATUS_32_63_OFFSET);
}


static inline void gpio_padcfg_32_35_padcfg_set(uint32_t base, uint32_t value)
{
    GAP_WRITE(base, GPIO_PADCFG_32_35_OFFSET, GAP_BINSERT(GAP_READ(base, GPIO_PADCFG_32_35_OFFSET), value, 4, 0));
}

static inline uint32_t gpio_padcfg_32_35_padcfg_get(uint32_t base)
{
    return GAP_BEXTRACTU(GAP_READ(base, GPIO_PADCFG_32_35_OFFSET), 4, 0);
}

static inline int32_t gpio_padcfg_32_35_padcfg_gets(uint32_t base)
{
    return GAP_BEXTRACT(GAP_READ(base, GPIO_PADCFG_32_35_OFFSET), 4, 0);
}


static inline void gpio_padcfg_36_39_padcfg_set(uint32_t base, uint32_t value)
{
    GAP_WRITE(base, GPIO_PADCFG_36_39_OFFSET, GAP_BINSERT(GAP_READ(base, GPIO_PADCFG_36_39_OFFSET), value, 4, 0));
}

static inline uint32_t gpio_padcfg_36_39_padcfg_get(uint32_t base)
{
    return GAP_BEXTRACTU(GAP_READ(base, GPIO_PADCFG_36_39_OFFSET), 4, 0);
}

static inline int32_t gpio_padcfg_36_39_padcfg_gets(uint32_t base)
{
    return GAP_BEXTRACT(GAP_READ(base, GPIO_PADCFG_36_39_OFFSET), 4, 0);
}


static inline void gpio_padcfg_40_43_padcfg_set(uint32_t base, uint32_t value)
{
    GAP_WRITE(base, GPIO_PADCFG_40_43_OFFSET, GAP_BINSERT(GAP_READ(base, GPIO_PADCFG_40_43_OFFSET), value, 4, 0));
}

static inline uint32_t gpio_padcfg_40_43_padcfg_get(uint32_t base)
{
    return GAP_BEXTRACTU(GAP_READ(base, GPIO_PADCFG_40_43_OFFSET), 4, 0);
}

static inline int32_t gpio_padcfg_40_43_padcfg_gets(uint32_t base)
{
    return GAP_BEXTRACT(GAP_READ(base, GPIO_PADCFG_40_43_OFFSET), 4, 0);
}


static inline void gpio_padcfg_44_47_padcfg_set(uint32_t base, uint32_t value)
{
    GAP_WRITE(base, GPIO_PADCFG_44_47_OFFSET, GAP_BINSERT(GAP_READ(base, GPIO_PADCFG_44_47_OFFSET), value, 4, 0));
}

static inline uint32_t gpio_padcfg_44_47_padcfg_get(uint32_t base)
{
    return GAP_BEXTRACTU(GAP_READ(base, GPIO_PADCFG_44_47_OFFSET), 4, 0);
}

static inline int32_t gpio_padcfg_44_47_padcfg_gets(uint32_t base)
{
    return GAP_BEXTRACT(GAP_READ(base, GPIO_PADCFG_44_47_OFFSET), 4, 0);
}


static inline void gpio_padcfg_48_51_padcfg_set(uint32_t base, uint32_t value)
{
    GAP_WRITE(base, GPIO_PADCFG_48_51_OFFSET, GAP_BINSERT(GAP_READ(base, GPIO_PADCFG_48_51_OFFSET), value, 4, 0));
}

static inline uint32_t gpio_padcfg_48_51_padcfg_get(uint32_t base)
{
    return GAP_BEXTRACTU(GAP_READ(base, GPIO_PADCFG_48_51_OFFSET), 4, 0);
}

static inline int32_t gpio_padcfg_48_51_padcfg_gets(uint32_t base)
{
    return GAP_BEXTRACT(GAP_READ(base, GPIO_PADCFG_48_51_OFFSET), 4, 0);
}


static inline void gpio_padcfg_52_55_padcfg_set(uint32_t base, uint32_t value)
{
    GAP_WRITE(base, GPIO_PADCFG_52_55_OFFSET, GAP_BINSERT(GAP_READ(base, GPIO_PADCFG_52_55_OFFSET), value, 4, 0));
}

static inline uint32_t gpio_padcfg_52_55_padcfg_get(uint32_t base)
{
    return GAP_BEXTRACTU(GAP_READ(base, GPIO_PADCFG_52_55_OFFSET), 4, 0);
}

static inline int32_t gpio_padcfg_52_55_padcfg_gets(uint32_t base)
{
    return GAP_BEXTRACT(GAP_READ(base, GPIO_PADCFG_52_55_OFFSET), 4, 0);
}


static inline void gpio_padcfg_56_59_padcfg_set(uint32_t base, uint32_t value)
{
    GAP_WRITE(base, GPIO_PADCFG_56_59_OFFSET, GAP_BINSERT(GAP_READ(base, GPIO_PADCFG_56_59_OFFSET), value, 4, 0));
}

static inline uint32_t gpio_padcfg_56_59_padcfg_get(uint32_t base)
{
    return GAP_BEXTRACTU(GAP_READ(base, GPIO_PADCFG_56_59_OFFSET), 4, 0);
}

static inline int32_t gpio_padcfg_56_59_padcfg_gets(uint32_t base)
{
    return GAP_BEXTRACT(GAP_READ(base, GPIO_PADCFG_56_59_OFFSET), 4, 0);
}


static inline void gpio_padcfg_60_63_padcfg_set(uint32_t base, uint32_t value)
{
    GAP_WRITE(base, GPIO_PADCFG_60_63_OFFSET, GAP_BINSERT(GAP_READ(base, GPIO_PADCFG_60_63_OFFSET), value, 4, 0));
}

static inline uint32_t gpio_padcfg_60_63_padcfg_get(uint32_t base)
{
    return GAP_BEXTRACTU(GAP_READ(base, GPIO_PADCFG_60_63_OFFSET), 4, 0);
}

static inline int32_t gpio_padcfg_60_63_padcfg_gets(uint32_t base)
{
    return GAP_BEXTRACT(GAP_READ(base, GPIO_PADCFG_60_63_OFFSET), 4, 0);
}


static inline void gpio_paddir_64_95_paddir_set(uint32_t base, uint32_t value)
{
    GAP_WRITE(base, GPIO_PADDIR_64_95_OFFSET, value);
}

static inline uint32_t gpio_paddir_64_95_paddir_get(uint32_t base)
{
    return GAP_READ(base, GPIO_PADDIR_64_95_OFFSET);
}

static inline int32_t gpio_paddir_64_95_paddir_gets(uint32_t base)
{
    return GAP_READ(base, GPIO_PADDIR_64_95_OFFSET);
}


static inline void gpio_gpioen_64_95_gpioen_set(uint32_t base, uint32_t value)
{
    GAP_WRITE(base, GPIO_GPIOEN_64_95_OFFSET, value);
}

static inline uint32_t gpio_gpioen_64_95_gpioen_get(uint32_t base)
{
    return GAP_READ(base, GPIO_GPIOEN_64_95_OFFSET);
}

static inline int32_t gpio_gpioen_64_95_gpioen_gets(uint32_t base)
{
    return GAP_READ(base, GPIO_GPIOEN_64_95_OFFSET);
}


static inline void gpio_padin_64_95_padin_set(uint32_t base, uint32_t value)
{
    GAP_WRITE(base, GPIO_PADIN_64_95_OFFSET, value);
}

static inline uint32_t gpio_padin_64_95_padin_get(uint32_t base)
{
    return GAP_READ(base, GPIO_PADIN_64_95_OFFSET);
}

static inline int32_t gpio_padin_64_95_padin_gets(uint32_t base)
{
    return GAP_READ(base, GPIO_PADIN_64_95_OFFSET);
}


static inline void gpio_padout_64_95_padout_set(uint32_t base, uint32_t value)
{
    GAP_WRITE(base, GPIO_PADOUT_64_95_OFFSET, value);
}

static inline uint32_t gpio_padout_64_95_padout_get(uint32_t base)
{
    return GAP_READ(base, GPIO_PADOUT_64_95_OFFSET);
}

static inline int32_t gpio_padout_64_95_padout_gets(uint32_t base)
{
    return GAP_READ(base, GPIO_PADOUT_64_95_OFFSET);
}


static inline void gpio_padoutset_64_95_padoutset_set(uint32_t base, uint32_t value)
{
    GAP_WRITE(base, GPIO_PADOUTSET_64_95_OFFSET, value);
}

static inline uint32_t gpio_padoutset_64_95_padoutset_get(uint32_t base)
{
    return GAP_READ(base, GPIO_PADOUTSET_64_95_OFFSET);
}

static inline int32_t gpio_padoutset_64_95_padoutset_gets(uint32_t base)
{
    return GAP_READ(base, GPIO_PADOUTSET_64_95_OFFSET);
}


static inline void gpio_padoutclr_64_95_padoutclr_set(uint32_t base, uint32_t value)
{
    GAP_WRITE(base, GPIO_PADOUTCLR_64_95_OFFSET, value);
}

static inline uint32_t gpio_padoutclr_64_95_padoutclr_get(uint32_t base)
{
    return GAP_READ(base, GPIO_PADOUTCLR_64_95_OFFSET);
}

static inline int32_t gpio_padoutclr_64_95_padoutclr_gets(uint32_t base)
{
    return GAP_READ(base, GPIO_PADOUTCLR_64_95_OFFSET);
}


static inline void gpio_inten_64_95_inten_set(uint32_t base, uint32_t value)
{
    GAP_WRITE(base, GPIO_INTEN_64_95_OFFSET, value);
}

static inline uint32_t gpio_inten_64_95_inten_get(uint32_t base)
{
    return GAP_READ(base, GPIO_INTEN_64_95_OFFSET);
}

static inline int32_t gpio_inten_64_95_inten_gets(uint32_t base)
{
    return GAP_READ(base, GPIO_INTEN_64_95_OFFSET);
}


static inline void gpio_inttype_64_79_inttype_set(uint32_t base, uint32_t value)
{
    GAP_WRITE(base, GPIO_INTTYPE_64_79_OFFSET, value);
}

static inline uint32_t gpio_inttype_64_79_inttype_get(uint32_t base)
{
    return GAP_READ(base, GPIO_INTTYPE_64_79_OFFSET);
}

static inline int32_t gpio_inttype_64_79_inttype_gets(uint32_t base)
{
    return GAP_READ(base, GPIO_INTTYPE_64_79_OFFSET);
}


static inline void gpio_inttype_80_95_inttype_set(uint32_t base, uint32_t value)
{
    GAP_WRITE(base, GPIO_INTTYPE_80_95_OFFSET, value);
}

static inline uint32_t gpio_inttype_80_95_inttype_get(uint32_t base)
{
    return GAP_READ(base, GPIO_INTTYPE_80_95_OFFSET);
}

static inline int32_t gpio_inttype_80_95_inttype_gets(uint32_t base)
{
    return GAP_READ(base, GPIO_INTTYPE_80_95_OFFSET);
}


static inline void gpio_intstatus_64_95_intstatus_set(uint32_t base, uint32_t value)
{
    GAP_WRITE(base, GPIO_INTSTATUS_64_95_OFFSET, value);
}

static inline uint32_t gpio_intstatus_64_95_intstatus_get(uint32_t base)
{
    return GAP_READ(base, GPIO_INTSTATUS_64_95_OFFSET);
}

static inline int32_t gpio_intstatus_64_95_intstatus_gets(uint32_t base)
{
    return GAP_READ(base, GPIO_INTSTATUS_64_95_OFFSET);
}


static inline void gpio_padcfg_64_67_padcfg_set(uint32_t base, uint32_t value)
{
    GAP_WRITE(base, GPIO_PADCFG_64_67_OFFSET, GAP_BINSERT(GAP_READ(base, GPIO_PADCFG_64_67_OFFSET), value, 4, 0));
}

static inline uint32_t gpio_padcfg_64_67_padcfg_get(uint32_t base)
{
    return GAP_BEXTRACTU(GAP_READ(base, GPIO_PADCFG_64_67_OFFSET), 4, 0);
}

static inline int32_t gpio_padcfg_64_67_padcfg_gets(uint32_t base)
{
    return GAP_BEXTRACT(GAP_READ(base, GPIO_PADCFG_64_67_OFFSET), 4, 0);
}


static inline void gpio_padcfg_68_71_padcfg_set(uint32_t base, uint32_t value)
{
    GAP_WRITE(base, GPIO_PADCFG_68_71_OFFSET, GAP_BINSERT(GAP_READ(base, GPIO_PADCFG_68_71_OFFSET), value, 4, 0));
}

static inline uint32_t gpio_padcfg_68_71_padcfg_get(uint32_t base)
{
    return GAP_BEXTRACTU(GAP_READ(base, GPIO_PADCFG_68_71_OFFSET), 4, 0);
}

static inline int32_t gpio_padcfg_68_71_padcfg_gets(uint32_t base)
{
    return GAP_BEXTRACT(GAP_READ(base, GPIO_PADCFG_68_71_OFFSET), 4, 0);
}


static inline void gpio_padcfg_72_75_padcfg_set(uint32_t base, uint32_t value)
{
    GAP_WRITE(base, GPIO_PADCFG_72_75_OFFSET, GAP_BINSERT(GAP_READ(base, GPIO_PADCFG_72_75_OFFSET), value, 4, 0));
}

static inline uint32_t gpio_padcfg_72_75_padcfg_get(uint32_t base)
{
    return GAP_BEXTRACTU(GAP_READ(base, GPIO_PADCFG_72_75_OFFSET), 4, 0);
}

static inline int32_t gpio_padcfg_72_75_padcfg_gets(uint32_t base)
{
    return GAP_BEXTRACT(GAP_READ(base, GPIO_PADCFG_72_75_OFFSET), 4, 0);
}


static inline void gpio_padcfg_76_79_padcfg_set(uint32_t base, uint32_t value)
{
    GAP_WRITE(base, GPIO_PADCFG_76_79_OFFSET, GAP_BINSERT(GAP_READ(base, GPIO_PADCFG_76_79_OFFSET), value, 4, 0));
}

static inline uint32_t gpio_padcfg_76_79_padcfg_get(uint32_t base)
{
    return GAP_BEXTRACTU(GAP_READ(base, GPIO_PADCFG_76_79_OFFSET), 4, 0);
}

static inline int32_t gpio_padcfg_76_79_padcfg_gets(uint32_t base)
{
    return GAP_BEXTRACT(GAP_READ(base, GPIO_PADCFG_76_79_OFFSET), 4, 0);
}


static inline void gpio_padcfg_80_83_padcfg_set(uint32_t base, uint32_t value)
{
    GAP_WRITE(base, GPIO_PADCFG_80_83_OFFSET, GAP_BINSERT(GAP_READ(base, GPIO_PADCFG_80_83_OFFSET), value, 4, 0));
}

static inline uint32_t gpio_padcfg_80_83_padcfg_get(uint32_t base)
{
    return GAP_BEXTRACTU(GAP_READ(base, GPIO_PADCFG_80_83_OFFSET), 4, 0);
}

static inline int32_t gpio_padcfg_80_83_padcfg_gets(uint32_t base)
{
    return GAP_BEXTRACT(GAP_READ(base, GPIO_PADCFG_80_83_OFFSET), 4, 0);
}


static inline void gpio_padcfg_84_87_padcfg_set(uint32_t base, uint32_t value)
{
    GAP_WRITE(base, GPIO_PADCFG_84_87_OFFSET, GAP_BINSERT(GAP_READ(base, GPIO_PADCFG_84_87_OFFSET), value, 4, 0));
}

static inline uint32_t gpio_padcfg_84_87_padcfg_get(uint32_t base)
{
    return GAP_BEXTRACTU(GAP_READ(base, GPIO_PADCFG_84_87_OFFSET), 4, 0);
}

static inline int32_t gpio_padcfg_84_87_padcfg_gets(uint32_t base)
{
    return GAP_BEXTRACT(GAP_READ(base, GPIO_PADCFG_84_87_OFFSET), 4, 0);
}


static inline void gpio_padcfg_88_91_padcfg_set(uint32_t base, uint32_t value)
{
    GAP_WRITE(base, GPIO_PADCFG_88_91_OFFSET, GAP_BINSERT(GAP_READ(base, GPIO_PADCFG_88_91_OFFSET), value, 4, 0));
}

static inline uint32_t gpio_padcfg_88_91_padcfg_get(uint32_t base)
{
    return GAP_BEXTRACTU(GAP_READ(base, GPIO_PADCFG_88_91_OFFSET), 4, 0);
}

static inline int32_t gpio_padcfg_88_91_padcfg_gets(uint32_t base)
{
    return GAP_BEXTRACT(GAP_READ(base, GPIO_PADCFG_88_91_OFFSET), 4, 0);
}


static inline void gpio_padcfg_92_95_padcfg_set(uint32_t base, uint32_t value)
{
    GAP_WRITE(base, GPIO_PADCFG_92_95_OFFSET, GAP_BINSERT(GAP_READ(base, GPIO_PADCFG_92_95_OFFSET), value, 4, 0));
}

static inline uint32_t gpio_padcfg_92_95_padcfg_get(uint32_t base)
{
    return GAP_BEXTRACTU(GAP_READ(base, GPIO_PADCFG_92_95_OFFSET), 4, 0);
}

static inline int32_t gpio_padcfg_92_95_padcfg_gets(uint32_t base)
{
    return GAP_BEXTRACT(GAP_READ(base, GPIO_PADCFG_92_95_OFFSET), 4, 0);
}


#endif

#endif
