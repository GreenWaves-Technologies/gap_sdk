
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

#ifndef __ARCHI_HWCE_REGS__
#define __ARCHI_HWCE_REGS__

#if !defined(LANGUAGE_ASSEMBLY) && !defined(__ASSEMBLER__)


#endif




//
// REGISTERS
//

// Trigger the execution of an offloaded job
#define HWCE_TRIGGER_OFFSET                      0x0

// Acquire the lock to offload job
#define HWCE_ACQUIRE_OFFSET                      0x4

// Number of concluded jobs since last read
#define HWCE_FINISHED_JOBS_OFFSET                0x8

// Status of the HWCE
#define HWCE_STATUS_OFFSET                       0xc

// ID of the currently running job
#define HWCE_RUNNING_JOB_OFFSET                  0x10

// Reset HWCE to known idle state
#define HWCE_SOFT_CLEAR_OFFSET                   0x14

// Generic configuration register 0
#define HWCE_GEN_CONFIG0_OFFSET                  0x20

// Generic configuration register 1
#define HWCE_GEN_CONFIG1_OFFSET                  0x24

// Total number of words to be read for y<sub>in</sub> and y<sub>out</sub>
#define HWCE_Y_TRANS_SIZE_OFFSET                 0x40

// Line stride and length for y<sub>in</sub> and y<sub>out</sub>
#define HWCE_Y_LINE_STRIDE_LENGTH_OFFSET         0x44

// Feature (block) stride and length for y<sub>in</sub> and y<sub>out</sub>
#define HWCE_Y_FEAT_STRIDE_LENGTH_OFFSET         0x48

// Base address of y<sub>out</sub>[3]
#define HWCE_Y_OUT_3_BASE_ADDR_OFFSET            0x4c

// Base address of y<sub>out</sub>[2]
#define HWCE_Y_OUT_2_BASE_ADDR_OFFSET            0x50

// Base address of y<sub>out</sub>[1]
#define HWCE_Y_OUT_1_BASE_ADDR_OFFSET            0x54

// Base address of y<sub>out</sub>[0]
#define HWCE_Y_OUT_0_BASE_ADDR_OFFSET            0x58

// Base address of y<sub>in</sub>[3]
#define HWCE_Y_IN_3_BASE_ADDR_OFFSET             0x5c

// Base address of y<sub>in</sub>[2]
#define HWCE_Y_IN_2_BASE_ADDR_OFFSET             0x60

// Base address of y<sub>in</sub>[1]
#define HWCE_Y_IN_1_BASE_ADDR_OFFSET             0x64

// Base address of y<sub>in</sub>[0]
#define HWCE_Y_IN_0_BASE_ADDR_OFFSET             0x68

// Total number of words to be read for x<sub>in</sub>
#define HWCE_X_TRANS_SIZE_OFFSET                 0x6c

// Line stride and length for x<sub>in</sub>
#define HWCE_X_LINE_STRIDE_LENGTH_OFFSET         0x70

// Feature (block) stride and length for x<sub>in</sub>
#define HWCE_X_FEAT_STRIDE_LENGTH_OFFSET         0x74

// Base address of x<sub>in</sub>
#define HWCE_X_IN_BASE_ADDR_OFFSET               0x78

// Base address of W
#define HWCE_W_BASE_ADDR_OFFSET                  0x7c

// Job configuration register 0
#define HWCE_JOB_CONFIG0_OFFSET                  0x80

// Job configuration register 1
#define HWCE_JOB_CONFIG1_OFFSET                  0x84

// Total number of words to be read for y<sub>in</sub> and y<sub>out</sub>
#define HWCE_Y_TRANS_SIZE_CTX0_OFFSET            0x140

// Line stride and length for y<sub>in</sub> and y<sub>out</sub>
#define HWCE_Y_LINE_STRIDE_LENGTH_CTX0_OFFSET    0x144

// Feature (block) stride and length for y<sub>in</sub> and y<sub>out</sub>
#define HWCE_Y_FEAT_STRIDE_LENGTH_CTX0_OFFSET    0x148

// Base address of y<sub>out</sub>[3]
#define HWCE_Y_OUT_3_BASE_ADDR_CTX0_OFFSET       0x14c

// Base address of y<sub>out</sub>[2]
#define HWCE_Y_OUT_2_BASE_ADDR_CTX0_OFFSET       0x150

// Base address of y<sub>out</sub>[1]
#define HWCE_Y_OUT_1_BASE_ADDR_CTX0_OFFSET       0x154

// Base address of y<sub>out</sub>[0]
#define HWCE_Y_OUT_0_BASE_ADDR_CTX0_OFFSET       0x158

// Base address of y<sub>in</sub>[3]
#define HWCE_Y_IN_3_BASE_ADDR_CTX0_OFFSET        0x15c

// Base address of y<sub>in</sub>[2]
#define HWCE_Y_IN_2_BASE_ADDR_CTX0_OFFSET        0x160

// Base address of y<sub>in</sub>[1]
#define HWCE_Y_IN_1_BASE_ADDR_CTX0_OFFSET        0x164

// Base address of y<sub>in</sub>[0]
#define HWCE_Y_IN_0_BASE_ADDR_CTX0_OFFSET        0x168

// Total number of words to be read for x<sub>in</sub>
#define HWCE_X_TRANS_SIZE_CTX0_OFFSET            0x16c

// Line stride and length for x<sub>in</sub>
#define HWCE_X_LINE_STRIDE_LENGTH_CTX0_OFFSET    0x170

// Feature (block) stride and length for x<sub>in</sub>
#define HWCE_X_FEAT_STRIDE_LENGTH_CTX0_OFFSET    0x174

// Base address of x<sub>in</sub>
#define HWCE_X_IN_BASE_ADDR_CTX0_OFFSET          0x178

// Base address of W
#define HWCE_W_BASE_ADDR_CTX0_OFFSET             0x17c

// Job configuration register 0
#define HWCE_JOB_CONFIG0_CTX0_OFFSET             0x180

// Job configuration register 1
#define HWCE_JOB_CONFIG1_CTX0_OFFSET             0x184

// Total number of words to be read for y<sub>in</sub> and y<sub>out</sub>
#define HWCE_Y_TRANS_SIZE_CTX1_OFFSET            0x240

// Line stride and length for y<sub>in</sub> and y<sub>out</sub>
#define HWCE_Y_LINE_STRIDE_LENGTH_CTX1_OFFSET    0x244

// Feature (block) stride and length for y<sub>in</sub> and y<sub>out</sub>
#define HWCE_Y_FEAT_STRIDE_LENGTH_CTX1_OFFSET    0x248

// Base address of y<sub>out</sub>[3]
#define HWCE_Y_OUT_3_BASE_ADDR_CTX1_OFFSET       0x24c

// Base address of y<sub>out</sub>[2]
#define HWCE_Y_OUT_2_BASE_ADDR_CTX1_OFFSET       0x250

// Base address of y<sub>out</sub>[1]
#define HWCE_Y_OUT_1_BASE_ADDR_CTX1_OFFSET       0x254

// Base address of y<sub>out</sub>[0]
#define HWCE_Y_OUT_0_BASE_ADDR_CTX1_OFFSET       0x258

// Base address of y<sub>in</sub>[3]
#define HWCE_Y_IN_3_BASE_ADDR_CTX1_OFFSET        0x25c

// Base address of y<sub>in</sub>[2]
#define HWCE_Y_IN_2_BASE_ADDR_CTX1_OFFSET        0x260

// Base address of y<sub>in</sub>[1]
#define HWCE_Y_IN_1_BASE_ADDR_CTX1_OFFSET        0x264

// Base address of y<sub>in</sub>[0]
#define HWCE_Y_IN_0_BASE_ADDR_CTX1_OFFSET        0x268

// Total number of words to be read for x<sub>in</sub>
#define HWCE_X_TRANS_SIZE_CTX1_OFFSET            0x26c

// Line stride and length for x<sub>in</sub>
#define HWCE_X_LINE_STRIDE_LENGTH_CTX1_OFFSET    0x270

// Feature (block) stride and length for x<sub>in</sub>
#define HWCE_X_FEAT_STRIDE_LENGTH_CTX1_OFFSET    0x274

// Base address of x<sub>in</sub>
#define HWCE_X_IN_BASE_ADDR_CTX1_OFFSET          0x278

// Base address of W
#define HWCE_W_BASE_ADDR_CTX1_OFFSET             0x27c

// Job configuration register 0
#define HWCE_JOB_CONFIG0_CTX1_OFFSET             0x280

// Job configuration register 1
#define HWCE_JOB_CONFIG1_CTX1_OFFSET             0x284

#endif
