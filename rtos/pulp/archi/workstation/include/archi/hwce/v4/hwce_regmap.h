
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

#ifndef __ARCHI_HWCE_REGMAP__
#define __ARCHI_HWCE_REGMAP__

#if !defined(LANGUAGE_ASSEMBLY) && !defined(__ASSEMBLER__)

#include <stdint.h>

#endif




//
// REGISTERS GLOBAL STRUCT
//

#if !defined(LANGUAGE_ASSEMBLY) && !defined(__ASSEMBLER__)

/** HWCE_Type Register Layout Typedef */
typedef struct {
    volatile uint32_t trigger;  // Trigger the execution of an offloaded job
    volatile uint32_t acquire;  // Acquire the lock to offload job
    volatile uint32_t finished_jobs;  // Number of concluded jobs since last read
    volatile uint32_t status;  // Status of the HWCE
    volatile uint32_t running_job;  // ID of the currently running job
    volatile uint32_t soft_clear;  // Reset HWCE to known idle state
    volatile uint32_t reserved_0[2];  // Reserved/Not used.
    volatile uint32_t gen_config0;  // Generic configuration register 0
    volatile uint32_t gen_config1;  // Generic configuration register 1
    volatile uint32_t reserved_1[6];  // Reserved/Not used.
    volatile uint32_t y_trans_size;  // Total number of words to be read for y<sub>in</sub> and y<sub>out</sub>
    volatile uint32_t y_line_stride_length;  // Line stride and length for y<sub>in</sub> and y<sub>out</sub>
    volatile uint32_t y_feat_stride_length;  // Feature (block) stride and length for y<sub>in</sub> and y<sub>out</sub>
    volatile uint32_t y_out_3_base_addr;  // Base address of y<sub>out</sub>[3]
    volatile uint32_t y_out_2_base_addr;  // Base address of y<sub>out</sub>[2]
    volatile uint32_t y_out_1_base_addr;  // Base address of y<sub>out</sub>[1]
    volatile uint32_t y_out_0_base_addr;  // Base address of y<sub>out</sub>[0]
    volatile uint32_t y_in_3_base_addr;  // Base address of y<sub>in</sub>[3]
    volatile uint32_t y_in_2_base_addr;  // Base address of y<sub>in</sub>[2]
    volatile uint32_t y_in_1_base_addr;  // Base address of y<sub>in</sub>[1]
    volatile uint32_t y_in_0_base_addr;  // Base address of y<sub>in</sub>[0]
    volatile uint32_t x_trans_size;  // Total number of words to be read for x<sub>in</sub>
    volatile uint32_t x_line_stride_length;  // Line stride and length for x<sub>in</sub>
    volatile uint32_t x_feat_stride_length;  // Feature (block) stride and length for x<sub>in</sub>
    volatile uint32_t x_in_base_addr;  // Base address of x<sub>in</sub>
    volatile uint32_t w_base_addr;  // Base address of W
    volatile uint32_t job_config0;  // Job configuration register 0
    volatile uint32_t job_config1;  // Job configuration register 1
    volatile uint32_t reserved_2[46];  // Reserved/Not used.
    volatile uint32_t y_trans_size_ctx0;  // Total number of words to be read for y<sub>in</sub> and y<sub>out</sub>
    volatile uint32_t y_line_stride_length_ctx0;  // Line stride and length for y<sub>in</sub> and y<sub>out</sub>
    volatile uint32_t y_feat_stride_length_ctx0;  // Feature (block) stride and length for y<sub>in</sub> and y<sub>out</sub>
    volatile uint32_t y_out_3_base_addr_ctx0;  // Base address of y<sub>out</sub>[3]
    volatile uint32_t y_out_2_base_addr_ctx0;  // Base address of y<sub>out</sub>[2]
    volatile uint32_t y_out_1_base_addr_ctx0;  // Base address of y<sub>out</sub>[1]
    volatile uint32_t y_out_0_base_addr_ctx0;  // Base address of y<sub>out</sub>[0]
    volatile uint32_t y_in_3_base_addr_ctx0;  // Base address of y<sub>in</sub>[3]
    volatile uint32_t y_in_2_base_addr_ctx0;  // Base address of y<sub>in</sub>[2]
    volatile uint32_t y_in_1_base_addr_ctx0;  // Base address of y<sub>in</sub>[1]
    volatile uint32_t y_in_0_base_addr_ctx0;  // Base address of y<sub>in</sub>[0]
    volatile uint32_t x_trans_size_ctx0;  // Total number of words to be read for x<sub>in</sub>
    volatile uint32_t x_line_stride_length_ctx0;  // Line stride and length for x<sub>in</sub>
    volatile uint32_t x_feat_stride_length_ctx0;  // Feature (block) stride and length for x<sub>in</sub>
    volatile uint32_t x_in_base_addr_ctx0;  // Base address of x<sub>in</sub>
    volatile uint32_t w_base_addr_ctx0;  // Base address of W
    volatile uint32_t job_config0_ctx0;  // Job configuration register 0
    volatile uint32_t job_config1_ctx0;  // Job configuration register 1
    volatile uint32_t reserved_3[46];  // Reserved/Not used.
    volatile uint32_t y_trans_size_ctx1;  // Total number of words to be read for y<sub>in</sub> and y<sub>out</sub>
    volatile uint32_t y_line_stride_length_ctx1;  // Line stride and length for y<sub>in</sub> and y<sub>out</sub>
    volatile uint32_t y_feat_stride_length_ctx1;  // Feature (block) stride and length for y<sub>in</sub> and y<sub>out</sub>
    volatile uint32_t y_out_3_base_addr_ctx1;  // Base address of y<sub>out</sub>[3]
    volatile uint32_t y_out_2_base_addr_ctx1;  // Base address of y<sub>out</sub>[2]
    volatile uint32_t y_out_1_base_addr_ctx1;  // Base address of y<sub>out</sub>[1]
    volatile uint32_t y_out_0_base_addr_ctx1;  // Base address of y<sub>out</sub>[0]
    volatile uint32_t y_in_3_base_addr_ctx1;  // Base address of y<sub>in</sub>[3]
    volatile uint32_t y_in_2_base_addr_ctx1;  // Base address of y<sub>in</sub>[2]
    volatile uint32_t y_in_1_base_addr_ctx1;  // Base address of y<sub>in</sub>[1]
    volatile uint32_t y_in_0_base_addr_ctx1;  // Base address of y<sub>in</sub>[0]
    volatile uint32_t x_trans_size_ctx1;  // Total number of words to be read for x<sub>in</sub>
    volatile uint32_t x_line_stride_length_ctx1;  // Line stride and length for x<sub>in</sub>
    volatile uint32_t x_feat_stride_length_ctx1;  // Feature (block) stride and length for x<sub>in</sub>
    volatile uint32_t x_in_base_addr_ctx1;  // Base address of x<sub>in</sub>
    volatile uint32_t w_base_addr_ctx1;  // Base address of W
    volatile uint32_t job_config0_ctx1;  // Job configuration register 0
    volatile uint32_t job_config1_ctx1;  // Job configuration register 1
} __attribute__((packed)) hwce_t;

#endif  /* LANGUAGE_ASSEMBLY || __ASSEMBLER__ */

#endif
