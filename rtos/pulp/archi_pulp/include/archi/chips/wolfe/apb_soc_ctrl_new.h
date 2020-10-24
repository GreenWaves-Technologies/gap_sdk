
/* THIS FILE HAS BEEN GENERATED, DO NOT MODIFY IT.
 */

/*
 * Copyright (C) 2018 ETH Zurich, University of Bologna
 * and GreenWaves Technologies
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

#ifndef __INCLUDE_ARCHI_CHIPS_WOLFE_APB_SOC_CTRL_NEW_H__
#define __INCLUDE_ARCHI_CHIPS_WOLFE_APB_SOC_CTRL_NEW_H__

#ifndef LANGUAGE_ASSEMBLY

#include <stdint.h>
#include "archi/utils.h"

#endif




//
// REGISTERS
//

// Deep sleep control register
#define APB_SOC_SLEEP_CONTROL_OFFSET             0x104

// First RTC register
#define APB_SOC_RTC_FIRST_REG_OFFSET             0x1d0

// Last RTC register
#define APB_SOC_RTC_LAST_REG_OFFSET              0x1dc



//
// REGISTERS FIELDS
//

// FLL retention configuration. (access: R/W)
#define APB_SOC_SLEEP_CONTROL_FLL_RET_BIT                            0
#define APB_SOC_SLEEP_CONTROL_FLL_RET_WIDTH                          2
#define APB_SOC_SLEEP_CONTROL_FLL_RET_MASK                           0x3

// Memory retention configuration part 0. (access: R/W)
#define APB_SOC_SLEEP_CONTROL_MEM_RET_0_BIT                          2
#define APB_SOC_SLEEP_CONTROL_MEM_RET_0_WIDTH                        1
#define APB_SOC_SLEEP_CONTROL_MEM_RET_0_MASK                         0x4

// External wakeup selection. This gives the GPIO numer which can wakeup the chip when it is in deep sleep mode. (access: R/W)
#define APB_SOC_SLEEP_CONTROL_EXTWAKEUP_SEL_BIT                      6
#define APB_SOC_SLEEP_CONTROL_EXTWAKEUP_SEL_WIDTH                    5
#define APB_SOC_SLEEP_CONTROL_EXTWAKEUP_SEL_MASK                     0x7c0

// External wakeup type. This tells the way the external GPIO can wakeup the chip while it is in deep sleep (raising edge, falling edge, etc). Possible values: - 0: Rising edge. - 1: Falling edge. - 2: Level high. - 3: Level low. (access: R/W)
#define APB_SOC_SLEEP_CONTROL_EXTWAKEUP_TYPE_BIT                     11
#define APB_SOC_SLEEP_CONTROL_EXTWAKEUP_TYPE_WIDTH                   2
#define APB_SOC_SLEEP_CONTROL_EXTWAKEUP_TYPE_MASK                    0x1800

// External wakeup enable. (access: R/W)
#define APB_SOC_SLEEP_CONTROL_EXTWAKEUP_EN_BIT                       13
#define APB_SOC_SLEEP_CONTROL_EXTWAKEUP_EN_WIDTH                     1
#define APB_SOC_SLEEP_CONTROL_EXTWAKEUP_EN_MASK                      0x2000

// Wake-up state. This specifies in which state the PMU should wakeup the chip. (access: R/W)
#define APB_SOC_SLEEP_CONTROL_WAKEUP_BIT                             14
#define APB_SOC_SLEEP_CONTROL_WAKEUP_WIDTH                           2
#define APB_SOC_SLEEP_CONTROL_WAKEUP_MASK                            0xc000

// Boot type. This is a user field written by the SW which is interpreted by the oot code and runtime after the chip has rebooted. (access: R/W)
#define APB_SOC_SLEEP_CONTROL_BOOT_TYPE_BIT                          18
#define APB_SOC_SLEEP_CONTROL_BOOT_TYPE_WIDTH                        2
#define APB_SOC_SLEEP_CONTROL_BOOT_TYPE_MASK                         0xc0000

// Cluster wakeup state. (access: R/W)
#define APB_SOC_SLEEP_CONTROL_CLUSTER_WAKEUP_BIT                     20
#define APB_SOC_SLEEP_CONTROL_CLUSTER_WAKEUP_WIDTH                   1
#define APB_SOC_SLEEP_CONTROL_CLUSTER_WAKEUP_MASK                    0x100000

// Memory retention configuration part 1. (access: R/W)
#define APB_SOC_SLEEP_CONTROL_MEM_RET_1_BIT                          21
#define APB_SOC_SLEEP_CONTROL_MEM_RET_1_WIDTH                        11
#define APB_SOC_SLEEP_CONTROL_MEM_RET_1_MASK                         0xffe00000



//
// REGISTERS STRUCTS
//

#ifndef LANGUAGE_ASSEMBLY

typedef union {
  struct {
    unsigned int fll_ret         :2 ; // FLL retention configuration.
    unsigned int mem_ret_0       :1 ; // Memory retention configuration part 0.
    unsigned int padding0:3 ;
    unsigned int extwakeup_sel   :5 ; // External wakeup selection. This gives the GPIO numer which can wakeup the chip when it is in deep sleep mode.
    unsigned int extwakeup_type  :2 ; // External wakeup type. This tells the way the external GPIO can wakeup the chip while it is in deep sleep (raising edge, falling edge, etc). Possible values: - 0: Rising edge. - 1: Falling edge. - 2: Level high. - 3: Level low.
    unsigned int extwakeup_en    :1 ; // External wakeup enable.
    unsigned int wakeup          :2 ; // Wake-up state. This specifies in which state the PMU should wakeup the chip.
    unsigned int padding1:2 ;
    unsigned int boot_type       :2 ; // Boot type. This is a user field written by the SW which is interpreted by the oot code and runtime after the chip has rebooted.
    unsigned int cluster_wakeup  :1 ; // Cluster wakeup state.
    unsigned int mem_ret_1       :11; // Memory retention configuration part 1.
  };
  unsigned int raw;
} __attribute__((packed)) apb_soc_sleep_control_t;

typedef union {
  struct {
  };
  unsigned int raw;
} __attribute__((packed)) apb_soc_rtc_first_reg_t;

typedef union {
  struct {
  };
  unsigned int raw;
} __attribute__((packed)) apb_soc_rtc_last_reg_t;

#endif



//
// REGISTERS STRUCTS
//

#ifdef __GVSOC__

class vp_apb_soc_sleep_control : public vp::reg_32
{
public:
  inline void fll_ret_set(uint32_t value) { this->set_field(value, APB_SOC_SLEEP_CONTROL_FLL_RET_BIT, APB_SOC_SLEEP_CONTROL_FLL_RET_WIDTH); }
  inline uint32_t fll_ret_get() { return this->get_field(APB_SOC_SLEEP_CONTROL_FLL_RET_BIT, APB_SOC_SLEEP_CONTROL_FLL_RET_WIDTH); }
  inline void mem_ret_0_set(uint32_t value) { this->set_field(value, APB_SOC_SLEEP_CONTROL_MEM_RET_0_BIT, APB_SOC_SLEEP_CONTROL_MEM_RET_0_WIDTH); }
  inline uint32_t mem_ret_0_get() { return this->get_field(APB_SOC_SLEEP_CONTROL_MEM_RET_0_BIT, APB_SOC_SLEEP_CONTROL_MEM_RET_0_WIDTH); }
  inline void extwakeup_sel_set(uint32_t value) { this->set_field(value, APB_SOC_SLEEP_CONTROL_EXTWAKEUP_SEL_BIT, APB_SOC_SLEEP_CONTROL_EXTWAKEUP_SEL_WIDTH); }
  inline uint32_t extwakeup_sel_get() { return this->get_field(APB_SOC_SLEEP_CONTROL_EXTWAKEUP_SEL_BIT, APB_SOC_SLEEP_CONTROL_EXTWAKEUP_SEL_WIDTH); }
  inline void extwakeup_type_set(uint32_t value) { this->set_field(value, APB_SOC_SLEEP_CONTROL_EXTWAKEUP_TYPE_BIT, APB_SOC_SLEEP_CONTROL_EXTWAKEUP_TYPE_WIDTH); }
  inline uint32_t extwakeup_type_get() { return this->get_field(APB_SOC_SLEEP_CONTROL_EXTWAKEUP_TYPE_BIT, APB_SOC_SLEEP_CONTROL_EXTWAKEUP_TYPE_WIDTH); }
  inline void extwakeup_en_set(uint32_t value) { this->set_field(value, APB_SOC_SLEEP_CONTROL_EXTWAKEUP_EN_BIT, APB_SOC_SLEEP_CONTROL_EXTWAKEUP_EN_WIDTH); }
  inline uint32_t extwakeup_en_get() { return this->get_field(APB_SOC_SLEEP_CONTROL_EXTWAKEUP_EN_BIT, APB_SOC_SLEEP_CONTROL_EXTWAKEUP_EN_WIDTH); }
  inline void wakeup_set(uint32_t value) { this->set_field(value, APB_SOC_SLEEP_CONTROL_WAKEUP_BIT, APB_SOC_SLEEP_CONTROL_WAKEUP_WIDTH); }
  inline uint32_t wakeup_get() { return this->get_field(APB_SOC_SLEEP_CONTROL_WAKEUP_BIT, APB_SOC_SLEEP_CONTROL_WAKEUP_WIDTH); }
  inline void boot_type_set(uint32_t value) { this->set_field(value, APB_SOC_SLEEP_CONTROL_BOOT_TYPE_BIT, APB_SOC_SLEEP_CONTROL_BOOT_TYPE_WIDTH); }
  inline uint32_t boot_type_get() { return this->get_field(APB_SOC_SLEEP_CONTROL_BOOT_TYPE_BIT, APB_SOC_SLEEP_CONTROL_BOOT_TYPE_WIDTH); }
  inline void cluster_wakeup_set(uint32_t value) { this->set_field(value, APB_SOC_SLEEP_CONTROL_CLUSTER_WAKEUP_BIT, APB_SOC_SLEEP_CONTROL_CLUSTER_WAKEUP_WIDTH); }
  inline uint32_t cluster_wakeup_get() { return this->get_field(APB_SOC_SLEEP_CONTROL_CLUSTER_WAKEUP_BIT, APB_SOC_SLEEP_CONTROL_CLUSTER_WAKEUP_WIDTH); }
  inline void mem_ret_1_set(uint32_t value) { this->set_field(value, APB_SOC_SLEEP_CONTROL_MEM_RET_1_BIT, APB_SOC_SLEEP_CONTROL_MEM_RET_1_WIDTH); }
  inline uint32_t mem_ret_1_get() { return this->get_field(APB_SOC_SLEEP_CONTROL_MEM_RET_1_BIT, APB_SOC_SLEEP_CONTROL_MEM_RET_1_WIDTH); }
};

class vp_apb_soc_rtc_first_reg : public vp::reg_32
{
public:
};

class vp_apb_soc_rtc_last_reg : public vp::reg_32
{
public:
};

#endif



//
// REGISTERS GLOBAL STRUCT
//

#ifndef LANGUAGE_ASSEMBLY

typedef struct {
  unsigned int sleep_control   ; // Deep sleep control register
  unsigned int rtc_first_reg   ; // First RTC register
  unsigned int rtc_last_reg    ; // Last RTC register
} __attribute__((packed)) apb_soc_apb_soc_t;

#endif



//
// REGISTERS ACCESS FUNCTIONS
//

#ifndef LANGUAGE_ASSEMBLY

static inline uint32_t apb_soc_sleep_control_get(uint32_t base) { return ARCHI_READ(base, APB_SOC_SLEEP_CONTROL_OFFSET); }
static inline void apb_soc_sleep_control_set(uint32_t base, uint32_t value) { ARCHI_WRITE(base, APB_SOC_SLEEP_CONTROL_OFFSET, value); }

static inline uint32_t apb_soc_rtc_first_reg_get(uint32_t base) { return ARCHI_READ(base, APB_SOC_RTC_FIRST_REG_OFFSET); }
static inline void apb_soc_rtc_first_reg_set(uint32_t base, uint32_t value) { ARCHI_WRITE(base, APB_SOC_RTC_FIRST_REG_OFFSET, value); }

static inline uint32_t apb_soc_rtc_last_reg_get(uint32_t base) { return ARCHI_READ(base, APB_SOC_RTC_LAST_REG_OFFSET); }
static inline void apb_soc_rtc_last_reg_set(uint32_t base, uint32_t value) { ARCHI_WRITE(base, APB_SOC_RTC_LAST_REG_OFFSET, value); }

#endif



//
// REGISTERS FIELDS MACROS
//

#ifndef LANGUAGE_ASSEMBLY

#define APB_SOC_SLEEP_CONTROL_FLL_RET_GET(value)           (ARCHI_BEXTRACTU((value),2,0))
#define APB_SOC_SLEEP_CONTROL_FLL_RET_GETS(value)          (ARCHI_BEXTRACT((value),2,0))
#define APB_SOC_SLEEP_CONTROL_FLL_RET_SET(value,field)     (ARCHI_BINSERT((value),(field),2,0))
#define APB_SOC_SLEEP_CONTROL_FLL_RET(val)                 ((val) << 0)

#define APB_SOC_SLEEP_CONTROL_MEM_RET_0_GET(value)         (ARCHI_BEXTRACTU((value),1,2))
#define APB_SOC_SLEEP_CONTROL_MEM_RET_0_GETS(value)        (ARCHI_BEXTRACT((value),1,2))
#define APB_SOC_SLEEP_CONTROL_MEM_RET_0_SET(value,field)   (ARCHI_BINSERT((value),(field),1,2))
#define APB_SOC_SLEEP_CONTROL_MEM_RET_0(val)               ((val) << 2)

#define APB_SOC_SLEEP_CONTROL_EXTWAKEUP_SEL_GET(value)     (ARCHI_BEXTRACTU((value),5,6))
#define APB_SOC_SLEEP_CONTROL_EXTWAKEUP_SEL_GETS(value)    (ARCHI_BEXTRACT((value),5,6))
#define APB_SOC_SLEEP_CONTROL_EXTWAKEUP_SEL_SET(value,field) (ARCHI_BINSERT((value),(field),5,6))
#define APB_SOC_SLEEP_CONTROL_EXTWAKEUP_SEL(val)           ((val) << 6)

#define APB_SOC_SLEEP_CONTROL_EXTWAKEUP_TYPE_GET(value)    (ARCHI_BEXTRACTU((value),2,11))
#define APB_SOC_SLEEP_CONTROL_EXTWAKEUP_TYPE_GETS(value)   (ARCHI_BEXTRACT((value),2,11))
#define APB_SOC_SLEEP_CONTROL_EXTWAKEUP_TYPE_SET(value,field) (ARCHI_BINSERT((value),(field),2,11))
#define APB_SOC_SLEEP_CONTROL_EXTWAKEUP_TYPE(val)          ((val) << 11)

#define APB_SOC_SLEEP_CONTROL_EXTWAKEUP_EN_GET(value)      (ARCHI_BEXTRACTU((value),1,13))
#define APB_SOC_SLEEP_CONTROL_EXTWAKEUP_EN_GETS(value)     (ARCHI_BEXTRACT((value),1,13))
#define APB_SOC_SLEEP_CONTROL_EXTWAKEUP_EN_SET(value,field) (ARCHI_BINSERT((value),(field),1,13))
#define APB_SOC_SLEEP_CONTROL_EXTWAKEUP_EN(val)            ((val) << 13)

#define APB_SOC_SLEEP_CONTROL_WAKEUP_GET(value)            (ARCHI_BEXTRACTU((value),2,14))
#define APB_SOC_SLEEP_CONTROL_WAKEUP_GETS(value)           (ARCHI_BEXTRACT((value),2,14))
#define APB_SOC_SLEEP_CONTROL_WAKEUP_SET(value,field)      (ARCHI_BINSERT((value),(field),2,14))
#define APB_SOC_SLEEP_CONTROL_WAKEUP(val)                  ((val) << 14)

#define APB_SOC_SLEEP_CONTROL_BOOT_TYPE_GET(value)         (ARCHI_BEXTRACTU((value),2,18))
#define APB_SOC_SLEEP_CONTROL_BOOT_TYPE_GETS(value)        (ARCHI_BEXTRACT((value),2,18))
#define APB_SOC_SLEEP_CONTROL_BOOT_TYPE_SET(value,field)   (ARCHI_BINSERT((value),(field),2,18))
#define APB_SOC_SLEEP_CONTROL_BOOT_TYPE(val)               ((val) << 18)

#define APB_SOC_SLEEP_CONTROL_CLUSTER_WAKEUP_GET(value)    (ARCHI_BEXTRACTU((value),1,20))
#define APB_SOC_SLEEP_CONTROL_CLUSTER_WAKEUP_GETS(value)   (ARCHI_BEXTRACT((value),1,20))
#define APB_SOC_SLEEP_CONTROL_CLUSTER_WAKEUP_SET(value,field) (ARCHI_BINSERT((value),(field),1,20))
#define APB_SOC_SLEEP_CONTROL_CLUSTER_WAKEUP(val)          ((val) << 20)

#define APB_SOC_SLEEP_CONTROL_MEM_RET_1_GET(value)         (ARCHI_BEXTRACTU((value),11,21))
#define APB_SOC_SLEEP_CONTROL_MEM_RET_1_GETS(value)        (ARCHI_BEXTRACT((value),11,21))
#define APB_SOC_SLEEP_CONTROL_MEM_RET_1_SET(value,field)   (ARCHI_BINSERT((value),(field),11,21))
#define APB_SOC_SLEEP_CONTROL_MEM_RET_1(val)               ((val) << 21)

#endif

#endif
