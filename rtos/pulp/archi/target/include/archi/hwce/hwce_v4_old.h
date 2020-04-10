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

#ifndef __ARCHI_HWCE_HWCE_V4_H__
#define __ARCHI_HWCE_HWCE_V4_H__

#define HWCE_ADDR_BASE ARCHI_HWCE_ADDR

#define HWCE_TRIGGER         0x00
#define HWCE_ACQUIRE         0x04
#define HWCE_FINISHED        0x08
#define HWCE_STATUS          0x0C
#define HWCE_RUNNING_JOB     0x10
#define HWCE_SOFT_CLEAR      0x14
#define HWCE_GEN_CONFIG0     0x20
#define HWCE_GEN_CONFIG1     0x24

#define HWCE_Y_TRANS_SIZE          0x40
#define HWCE_Y_LINE_STRIDE_LENGTH  0x44
#define HWCE_Y_FEAT_STRIDE_LENGTH  0x48
#define HWCE_Y_OUT_3_BASE_ADDR     0x4C
#define HWCE_Y_OUT_2_BASE_ADDR     0x50
#define HWCE_Y_OUT_1_BASE_ADDR     0x54
#define HWCE_Y_OUT_0_BASE_ADDR     0x58
#define HWCE_Y_IN_3_BASE_ADDR      0x5C
#define HWCE_Y_IN_2_BASE_ADDR      0x60
#define HWCE_Y_IN_1_BASE_ADDR      0x64
#define HWCE_Y_IN_0_BASE_ADDR      0x68
#define HWCE_X_TRANS_SIZE          0x6C
#define HWCE_X_LINE_STRIDE_LENGTH  0x70
#define HWCE_X_FEAT_STRIDE_LENGTH  0x74
#define HWCE_X_IN_BASE_ADDR        0x78
#define HWCE_W_BASE_ADDR           0x7C
#define HWCE_JOB_CONFIG0           0x80
#define HWCE_JOB_CONFIG1           0x84

#define HWCE_NB_IO_REGS    18

#define HWCE_ACQUIRE_CONTEXT_COPY -3
#define HWCE_ACQUIRE_LOCKED       -2
#define HWCE_ACQUIRE_QUEUE_FULL   -1
#define HWCE_ACQUIRE_READY        0

#define HWCE_GEN_CONFIG0_CONV_5x5 0
#define HWCE_GEN_CONFIG0_CONV_3x3 1
#define HWCE_GEN_CONFIG0_CONV_4x7 2

#define HWCE_GEN_CONFIG0_VECT_1 0
#define HWCE_GEN_CONFIG0_VECT_2 1
#define HWCE_GEN_CONFIG0_VECT_4 2

/* utility constants - used by hwce_rt and other libs */

// linebuffer width in 16-bit half-words
#define HWCE_LBSIZE 32

// loop order modes
#define HWCE_LOOP_ORDER_OLD   0
#define HWCE_LOOP_ORDER_OF_IF 2
#define HWCE_LOOP_ORDER_IF_OF 3

// 8-bit and 4-bit pixel modes
#define HWCE_PIXMODE_16BIT 0
#define HWCE_PIXMODE_8BIT  1
#define HWCE_PIXMODE_4BIT  2

// y_in modes
#define HWCE_Y_IN_MODE_ON  1
#define HWCE_Y_IN_MODE_OFF 0

// disable vectors
#define HWCE_VECT_DISABLE(y0, y1, y2, y3) ( y0 << 3 | y1 << 2 | y2 << 1 | y3 )

// conv mode
#define HWCE_CONVMODE_5x5 HWCE_GEN_CONFIG0_CONV_5x5
#define HWCE_CONVMODE_3x3 HWCE_GEN_CONFIG0_CONV_3x3
#define HWCE_CONVMODE_4x7 HWCE_GEN_CONFIG0_CONV_4x7

// vect mode
#define HWCE_VECTMODE_1 HWCE_GEN_CONFIG0_VECT_1
#define HWCE_VECTMODE_2 HWCE_GEN_CONFIG0_VECT_2
#define HWCE_VECTMODE_4 HWCE_GEN_CONFIG0_VECT_4

// sign mode
#define HWCE_SIGNMODE_SIGNED   1
#define HWCE_SIGNMODE_UNSIGNED 0

// weight order
#define HWCE_WEIGHTORDER_AUTOCORR 1
#define HWCE_WEIGHTORDER_CONV     0

// job copy mechanism
#define HWCE_JOBCOPY_ON  1
#define HWCE_JOBCOPY_OFF 0

#endif