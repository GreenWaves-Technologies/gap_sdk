
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

#ifndef __ARCHI_SOC_EU_REGMAP__
#define __ARCHI_SOC_EU_REGMAP__

#if !defined(LANGUAGE_ASSEMBLY) && !defined(__ASSEMBLER__)

#include <stdint.h>

#endif




//
// REGISTERS GLOBAL STRUCT
//

#if !defined(LANGUAGE_ASSEMBLY) && !defined(__ASSEMBLER__)

/** SOC_EU_Type Register Layout Typedef */
typedef struct {
    volatile uint32_t sw_event;  // SoC software events trigger command register.
    volatile uint32_t fc_mask_0;  // FC event unit event dispatch mask configuration register for events 0 to 31.
    volatile uint32_t fc_mask_1;  // FC event unit event dispatch mask configuration register for events 32 to 63.
    volatile uint32_t fc_mask_2;  // FC event unit event dispatch mask configuration register for events 64 to 95.
    volatile uint32_t fc_mask_3;  // FC event unit event dispatch mask configuration register for events 96 to 127.
    volatile uint32_t fc_mask_4;  // FC event unit event dispatch mask configuration register for events 128 to 159.
    volatile uint32_t fc_mask_5;  // FC event unit event dispatch mask configuration register for events 160 to 191.
    volatile uint32_t fc_mask_6;  // FC event unit event dispatch mask configuration register for events 192 to 223.
    volatile uint32_t fc_mask_7;  // FC event unit event dispatch mask configuration register for events 224 to 255.
    volatile uint32_t cl_mask_0;  // Cluster event dispatch mask configuration register for events 0 to 31.
    volatile uint32_t cl_mask_1;  // Cluster event dispatch mask configuration register for events 32 to 63.
    volatile uint32_t cl_mask_2;  // Cluster event dispatch mask configuration register for events 64 to 95.
    volatile uint32_t cl_mask_3;  // Cluster event dispatch mask configuration register for events 96 to 127.
    volatile uint32_t cl_mask_4;  // Cluster event dispatch mask configuration register for events 128 to 159.
    volatile uint32_t cl_mask_5;  // Cluster event dispatch mask configuration register for events 160 to 191.
    volatile uint32_t cl_mask_6;  // Cluster event dispatch mask configuration register for events 192 to 223.
    volatile uint32_t cl_mask_7;  // Cluster event dispatch mask configuration register for events 224 to 255.
    volatile uint32_t pr_mask_0;  // uDMA event dispatch mask configuration registerfor events 0 to 31.
    volatile uint32_t pr_mask_1;  // uDMA event dispatch mask configuration registerfor events 32 to 63.
    volatile uint32_t pr_mask_2;  // uDMA event dispatch mask configuration registerfor events 64 to 95.
    volatile uint32_t pr_mask_3;  // uDMA event dispatch mask configuration registerfor events 96 to 127.
    volatile uint32_t pr_mask_4;  // uDMA event dispatch mask configuration registerfor events 128 to 159.
    volatile uint32_t pr_mask_5;  // uDMA event dispatch mask configuration registerfor events 160 to 191.
    volatile uint32_t pr_mask_6;  // uDMA event dispatch mask configuration registerfor events 192 to 223.
    volatile uint32_t pr_mask_7;  // uDMA event dispatch mask configuration registerfor events 224 to 255.
    volatile uint32_t err_0;  // Event queue overflow status register for events 0 to 31.
    volatile uint32_t err_1;  // Event queue overflow status register for events 32 to 63.
    volatile uint32_t err_2;  // Event queue overflow status register for events 64 to 95.
    volatile uint32_t err_3;  // Event queue overflow status register for events 96 to 127.
    volatile uint32_t err_4;  // Event queue overflow status register for events 128 to 159.
    volatile uint32_t err_5;  // Event queue overflow status register for events 160 to 191.
    volatile uint32_t err_6;  // Event queue overflow status register for events 192 to 223.
    volatile uint32_t err_7;  // Event queue overflow status register for events 224 to 255.
    volatile uint32_t timer1_sel_hi;  // FC High Timer1 source event configuration register.
    volatile uint32_t timer1_sel_lo;  // FC Low Timer1 source event configuration register.
    volatile uint32_t timer2_sel_hi;  // FC High Timer2 source event configuration register.
    volatile uint32_t timer2_sel_lo;  // FC Low Timer2 source event configuration register.
} __attribute__((packed)) soc_eu_t;
/** SOC_EU_Type Register Layout Typedef */
typedef struct {

    volatile soc_eu_sw_event_t sw_event;  // SoC software events trigger command register.
    volatile soc_eu_fc_mask_0_t fc_mask_0;  // FC event unit event dispatch mask configuration register for events 0 to 31.
    volatile soc_eu_fc_mask_1_t fc_mask_1;  // FC event unit event dispatch mask configuration register for events 32 to 63.
    volatile soc_eu_fc_mask_2_t fc_mask_2;  // FC event unit event dispatch mask configuration register for events 64 to 95.
    volatile soc_eu_fc_mask_3_t fc_mask_3;  // FC event unit event dispatch mask configuration register for events 96 to 127.
    volatile soc_eu_fc_mask_4_t fc_mask_4;  // FC event unit event dispatch mask configuration register for events 128 to 159.
    volatile soc_eu_fc_mask_5_t fc_mask_5;  // FC event unit event dispatch mask configuration register for events 160 to 191.
    volatile soc_eu_fc_mask_6_t fc_mask_6;  // FC event unit event dispatch mask configuration register for events 192 to 223.
    volatile soc_eu_fc_mask_7_t fc_mask_7;  // FC event unit event dispatch mask configuration register for events 224 to 255.
    volatile soc_eu_cl_mask_0_t cl_mask_0;  // Cluster event dispatch mask configuration register for events 0 to 31.
    volatile soc_eu_cl_mask_1_t cl_mask_1;  // Cluster event dispatch mask configuration register for events 32 to 63.
    volatile soc_eu_cl_mask_2_t cl_mask_2;  // Cluster event dispatch mask configuration register for events 64 to 95.
    volatile soc_eu_cl_mask_3_t cl_mask_3;  // Cluster event dispatch mask configuration register for events 96 to 127.
    volatile soc_eu_cl_mask_4_t cl_mask_4;  // Cluster event dispatch mask configuration register for events 128 to 159.
    volatile soc_eu_cl_mask_5_t cl_mask_5;  // Cluster event dispatch mask configuration register for events 160 to 191.
    volatile soc_eu_cl_mask_6_t cl_mask_6;  // Cluster event dispatch mask configuration register for events 192 to 223.
    volatile soc_eu_cl_mask_7_t cl_mask_7;  // Cluster event dispatch mask configuration register for events 224 to 255.
    volatile soc_eu_pr_mask_0_t pr_mask_0;  // uDMA event dispatch mask configuration registerfor events 0 to 31.
    volatile soc_eu_pr_mask_1_t pr_mask_1;  // uDMA event dispatch mask configuration registerfor events 32 to 63.
    volatile soc_eu_pr_mask_2_t pr_mask_2;  // uDMA event dispatch mask configuration registerfor events 64 to 95.
    volatile soc_eu_pr_mask_3_t pr_mask_3;  // uDMA event dispatch mask configuration registerfor events 96 to 127.
    volatile soc_eu_pr_mask_4_t pr_mask_4;  // uDMA event dispatch mask configuration registerfor events 128 to 159.
    volatile soc_eu_pr_mask_5_t pr_mask_5;  // uDMA event dispatch mask configuration registerfor events 160 to 191.
    volatile soc_eu_pr_mask_6_t pr_mask_6;  // uDMA event dispatch mask configuration registerfor events 192 to 223.
    volatile soc_eu_pr_mask_7_t pr_mask_7;  // uDMA event dispatch mask configuration registerfor events 224 to 255.
    volatile soc_eu_err_0_t err_0;  // Event queue overflow status register for events 0 to 31.
    volatile soc_eu_err_1_t err_1;  // Event queue overflow status register for events 32 to 63.
    volatile soc_eu_err_2_t err_2;  // Event queue overflow status register for events 64 to 95.
    volatile soc_eu_err_3_t err_3;  // Event queue overflow status register for events 96 to 127.
    volatile soc_eu_err_4_t err_4;  // Event queue overflow status register for events 128 to 159.
    volatile soc_eu_err_5_t err_5;  // Event queue overflow status register for events 160 to 191.
    volatile soc_eu_err_6_t err_6;  // Event queue overflow status register for events 192 to 223.
    volatile soc_eu_err_7_t err_7;  // Event queue overflow status register for events 224 to 255.
    volatile soc_eu_timer1_sel_hi_t timer1_sel_hi;  // FC High Timer1 source event configuration register.
    volatile soc_eu_timer1_sel_lo_t timer1_sel_lo;  // FC Low Timer1 source event configuration register.
    volatile soc_eu_timer2_sel_hi_t timer2_sel_hi;  // FC High Timer2 source event configuration register.
    volatile soc_eu_timer2_sel_lo_t timer2_sel_lo;  // FC Low Timer2 source event configuration register.
} __attribute__((packed)) soc_eu_struct_t;

#endif  /* LANGUAGE_ASSEMBLY || __ASSEMBLER__ */

#endif
