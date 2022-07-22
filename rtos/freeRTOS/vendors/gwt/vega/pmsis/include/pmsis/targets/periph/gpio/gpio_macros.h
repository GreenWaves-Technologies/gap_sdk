
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

#ifndef __ARCHI_GPIO_MACROS__
#define __ARCHI_GPIO_MACROS__

#if !defined(LANGUAGE_ASSEMBLY) && !defined(__ASSEMBLER__)

#endif




//
// REGISTERS FIELDS MACROS
//

#if !defined(LANGUAGE_ASSEMBLY) && !defined(__ASSEMBLER__)

#define GPIO_PADDIR_00_31_PADDIR_GET(value)                (GAP_BEXTRACTU((value),32,0))
#define GPIO_PADDIR_00_31_PADDIR_GETS(value)               (GAP_BEXTRACT((value),32,0))
#define GPIO_PADDIR_00_31_PADDIR_SET(value,field)          (GAP_BINSERT((value),(field),32,0))
#define GPIO_PADDIR_00_31_PADDIR(val)                      ((val) << 0)

#define GPIO_GPIOEN_00_31_GPIOEN_GET(value)                (GAP_BEXTRACTU((value),32,0))
#define GPIO_GPIOEN_00_31_GPIOEN_GETS(value)               (GAP_BEXTRACT((value),32,0))
#define GPIO_GPIOEN_00_31_GPIOEN_SET(value,field)          (GAP_BINSERT((value),(field),32,0))
#define GPIO_GPIOEN_00_31_GPIOEN(val)                      ((val) << 0)

#define GPIO_PADIN_00_31_PADIN_GET(value)                  (GAP_BEXTRACTU((value),32,0))
#define GPIO_PADIN_00_31_PADIN_GETS(value)                 (GAP_BEXTRACT((value),32,0))
#define GPIO_PADIN_00_31_PADIN_SET(value,field)            (GAP_BINSERT((value),(field),32,0))
#define GPIO_PADIN_00_31_PADIN(val)                        ((val) << 0)

#define GPIO_PADOUT_00_31_PADOUT_GET(value)                (GAP_BEXTRACTU((value),32,0))
#define GPIO_PADOUT_00_31_PADOUT_GETS(value)               (GAP_BEXTRACT((value),32,0))
#define GPIO_PADOUT_00_31_PADOUT_SET(value,field)          (GAP_BINSERT((value),(field),32,0))
#define GPIO_PADOUT_00_31_PADOUT(val)                      ((val) << 0)

#define GPIO_PADOUTSET_00_31_PADOUTSET_GET(value)          (GAP_BEXTRACTU((value),32,0))
#define GPIO_PADOUTSET_00_31_PADOUTSET_GETS(value)         (GAP_BEXTRACT((value),32,0))
#define GPIO_PADOUTSET_00_31_PADOUTSET_SET(value,field)    (GAP_BINSERT((value),(field),32,0))
#define GPIO_PADOUTSET_00_31_PADOUTSET(val)                ((val) << 0)

#define GPIO_PADOUTCLR_00_31_PADOUTCLR_GET(value)          (GAP_BEXTRACTU((value),32,0))
#define GPIO_PADOUTCLR_00_31_PADOUTCLR_GETS(value)         (GAP_BEXTRACT((value),32,0))
#define GPIO_PADOUTCLR_00_31_PADOUTCLR_SET(value,field)    (GAP_BINSERT((value),(field),32,0))
#define GPIO_PADOUTCLR_00_31_PADOUTCLR(val)                ((val) << 0)

#define GPIO_INTEN_00_31_INTEN_GET(value)                  (GAP_BEXTRACTU((value),32,0))
#define GPIO_INTEN_00_31_INTEN_GETS(value)                 (GAP_BEXTRACT((value),32,0))
#define GPIO_INTEN_00_31_INTEN_SET(value,field)            (GAP_BINSERT((value),(field),32,0))
#define GPIO_INTEN_00_31_INTEN(val)                        ((val) << 0)

#define GPIO_INTTYPE_00_15_INTTYPE_GET(value)              (GAP_BEXTRACTU((value),32,0))
#define GPIO_INTTYPE_00_15_INTTYPE_GETS(value)             (GAP_BEXTRACT((value),32,0))
#define GPIO_INTTYPE_00_15_INTTYPE_SET(value,field)        (GAP_BINSERT((value),(field),32,0))
#define GPIO_INTTYPE_00_15_INTTYPE(val)                    ((val) << 0)

#define GPIO_INTTYPE_16_31_INTTYPE_GET(value)              (GAP_BEXTRACTU((value),32,0))
#define GPIO_INTTYPE_16_31_INTTYPE_GETS(value)             (GAP_BEXTRACT((value),32,0))
#define GPIO_INTTYPE_16_31_INTTYPE_SET(value,field)        (GAP_BINSERT((value),(field),32,0))
#define GPIO_INTTYPE_16_31_INTTYPE(val)                    ((val) << 0)

#define GPIO_INTSTATUS_00_31_INTSTATUS_GET(value)          (GAP_BEXTRACTU((value),32,0))
#define GPIO_INTSTATUS_00_31_INTSTATUS_GETS(value)         (GAP_BEXTRACT((value),32,0))
#define GPIO_INTSTATUS_00_31_INTSTATUS_SET(value,field)    (GAP_BINSERT((value),(field),32,0))
#define GPIO_INTSTATUS_00_31_INTSTATUS(val)                ((val) << 0)

#define GPIO_PADCFG_00_07_PADCFG_GET(value)                (GAP_BEXTRACTU((value),4,0))
#define GPIO_PADCFG_00_07_PADCFG_GETS(value)               (GAP_BEXTRACT((value),4,0))
#define GPIO_PADCFG_00_07_PADCFG_SET(value,field)          (GAP_BINSERT((value),(field),4,0))
#define GPIO_PADCFG_00_07_PADCFG(val)                      ((val) << 0)

#define GPIO_PADCFG_08_15_PADCFG_GET(value)                (GAP_BEXTRACTU((value),4,0))
#define GPIO_PADCFG_08_15_PADCFG_GETS(value)               (GAP_BEXTRACT((value),4,0))
#define GPIO_PADCFG_08_15_PADCFG_SET(value,field)          (GAP_BINSERT((value),(field),4,0))
#define GPIO_PADCFG_08_15_PADCFG(val)                      ((val) << 0)

#define GPIO_PADCFG_16_23_PADCFG_GET(value)                (GAP_BEXTRACTU((value),4,0))
#define GPIO_PADCFG_16_23_PADCFG_GETS(value)               (GAP_BEXTRACT((value),4,0))
#define GPIO_PADCFG_16_23_PADCFG_SET(value,field)          (GAP_BINSERT((value),(field),4,0))
#define GPIO_PADCFG_16_23_PADCFG(val)                      ((val) << 0)

#define GPIO_PADCFG_24_31_PADCFG_GET(value)                (GAP_BEXTRACTU((value),4,0))
#define GPIO_PADCFG_24_31_PADCFG_GETS(value)               (GAP_BEXTRACT((value),4,0))
#define GPIO_PADCFG_24_31_PADCFG_SET(value,field)          (GAP_BINSERT((value),(field),4,0))
#define GPIO_PADCFG_24_31_PADCFG(val)                      ((val) << 0)

#define GPIO_PADDIR_32_63_PADDIR_GET(value)                (GAP_BEXTRACTU((value),32,0))
#define GPIO_PADDIR_32_63_PADDIR_GETS(value)               (GAP_BEXTRACT((value),32,0))
#define GPIO_PADDIR_32_63_PADDIR_SET(value,field)          (GAP_BINSERT((value),(field),32,0))
#define GPIO_PADDIR_32_63_PADDIR(val)                      ((val) << 0)

#define GPIO_GPIOEN_32_63_GPIOEN_GET(value)                (GAP_BEXTRACTU((value),32,0))
#define GPIO_GPIOEN_32_63_GPIOEN_GETS(value)               (GAP_BEXTRACT((value),32,0))
#define GPIO_GPIOEN_32_63_GPIOEN_SET(value,field)          (GAP_BINSERT((value),(field),32,0))
#define GPIO_GPIOEN_32_63_GPIOEN(val)                      ((val) << 0)

#define GPIO_PADIN_32_63_PADIN_GET(value)                  (GAP_BEXTRACTU((value),32,0))
#define GPIO_PADIN_32_63_PADIN_GETS(value)                 (GAP_BEXTRACT((value),32,0))
#define GPIO_PADIN_32_63_PADIN_SET(value,field)            (GAP_BINSERT((value),(field),32,0))
#define GPIO_PADIN_32_63_PADIN(val)                        ((val) << 0)

#define GPIO_PADOUT_32_63_PADOUT_GET(value)                (GAP_BEXTRACTU((value),32,0))
#define GPIO_PADOUT_32_63_PADOUT_GETS(value)               (GAP_BEXTRACT((value),32,0))
#define GPIO_PADOUT_32_63_PADOUT_SET(value,field)          (GAP_BINSERT((value),(field),32,0))
#define GPIO_PADOUT_32_63_PADOUT(val)                      ((val) << 0)

#define GPIO_PADOUTSET_32_63_PADOUTSET_GET(value)          (GAP_BEXTRACTU((value),32,0))
#define GPIO_PADOUTSET_32_63_PADOUTSET_GETS(value)         (GAP_BEXTRACT((value),32,0))
#define GPIO_PADOUTSET_32_63_PADOUTSET_SET(value,field)    (GAP_BINSERT((value),(field),32,0))
#define GPIO_PADOUTSET_32_63_PADOUTSET(val)                ((val) << 0)

#define GPIO_PADOUTCLR_32_63_PADOUTCLR_GET(value)          (GAP_BEXTRACTU((value),32,0))
#define GPIO_PADOUTCLR_32_63_PADOUTCLR_GETS(value)         (GAP_BEXTRACT((value),32,0))
#define GPIO_PADOUTCLR_32_63_PADOUTCLR_SET(value,field)    (GAP_BINSERT((value),(field),32,0))
#define GPIO_PADOUTCLR_32_63_PADOUTCLR(val)                ((val) << 0)

#define GPIO_INTEN_32_63_INTEN_GET(value)                  (GAP_BEXTRACTU((value),32,0))
#define GPIO_INTEN_32_63_INTEN_GETS(value)                 (GAP_BEXTRACT((value),32,0))
#define GPIO_INTEN_32_63_INTEN_SET(value,field)            (GAP_BINSERT((value),(field),32,0))
#define GPIO_INTEN_32_63_INTEN(val)                        ((val) << 0)

#define GPIO_INTTYPE_32_47_INTTYPE_GET(value)              (GAP_BEXTRACTU((value),32,0))
#define GPIO_INTTYPE_32_47_INTTYPE_GETS(value)             (GAP_BEXTRACT((value),32,0))
#define GPIO_INTTYPE_32_47_INTTYPE_SET(value,field)        (GAP_BINSERT((value),(field),32,0))
#define GPIO_INTTYPE_32_47_INTTYPE(val)                    ((val) << 0)

#define GPIO_INTTYPE_48_63_INTTYPE_GET(value)              (GAP_BEXTRACTU((value),32,0))
#define GPIO_INTTYPE_48_63_INTTYPE_GETS(value)             (GAP_BEXTRACT((value),32,0))
#define GPIO_INTTYPE_48_63_INTTYPE_SET(value,field)        (GAP_BINSERT((value),(field),32,0))
#define GPIO_INTTYPE_48_63_INTTYPE(val)                    ((val) << 0)

#define GPIO_INTSTATUS_32_63_INTSTATUS_GET(value)          (GAP_BEXTRACTU((value),32,0))
#define GPIO_INTSTATUS_32_63_INTSTATUS_GETS(value)         (GAP_BEXTRACT((value),32,0))
#define GPIO_INTSTATUS_32_63_INTSTATUS_SET(value,field)    (GAP_BINSERT((value),(field),32,0))
#define GPIO_INTSTATUS_32_63_INTSTATUS(val)                ((val) << 0)

#define GPIO_PADCFG_32_39_PADCFG_GET(value)                (GAP_BEXTRACTU((value),4,0))
#define GPIO_PADCFG_32_39_PADCFG_GETS(value)               (GAP_BEXTRACT((value),4,0))
#define GPIO_PADCFG_32_39_PADCFG_SET(value,field)          (GAP_BINSERT((value),(field),4,0))
#define GPIO_PADCFG_32_39_PADCFG(val)                      ((val) << 0)

#define GPIO_PADCFG_40_47_PADCFG_GET(value)                (GAP_BEXTRACTU((value),4,0))
#define GPIO_PADCFG_40_47_PADCFG_GETS(value)               (GAP_BEXTRACT((value),4,0))
#define GPIO_PADCFG_40_47_PADCFG_SET(value,field)          (GAP_BINSERT((value),(field),4,0))
#define GPIO_PADCFG_40_47_PADCFG(val)                      ((val) << 0)

#define GPIO_PADCFG_48_55_PADCFG_GET(value)                (GAP_BEXTRACTU((value),4,0))
#define GPIO_PADCFG_48_55_PADCFG_GETS(value)               (GAP_BEXTRACT((value),4,0))
#define GPIO_PADCFG_48_55_PADCFG_SET(value,field)          (GAP_BINSERT((value),(field),4,0))
#define GPIO_PADCFG_48_55_PADCFG(val)                      ((val) << 0)

#define GPIO_PADCFG_56_63_PADCFG_GET(value)                (GAP_BEXTRACTU((value),4,0))
#define GPIO_PADCFG_56_63_PADCFG_GETS(value)               (GAP_BEXTRACT((value),4,0))
#define GPIO_PADCFG_56_63_PADCFG_SET(value,field)          (GAP_BINSERT((value),(field),4,0))
#define GPIO_PADCFG_56_63_PADCFG(val)                      ((val) << 0)

#endif

#endif
