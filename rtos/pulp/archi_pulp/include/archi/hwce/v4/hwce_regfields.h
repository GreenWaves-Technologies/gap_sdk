
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

#ifndef __ARCHI_HWCE_REGFIELD__
#define __ARCHI_HWCE_REGFIELD__

#if !defined(LANGUAGE_ASSEMBLY) && !defined(__ASSEMBLER__)


#endif




//
// REGISTERS FIELDS
//

// Write of any value will close the current offload phase by releasing the job offload lock and inserting the currently offloaded job in the control queue. (access: W)
#define HWCE_TRIGGER_ANY_BIT                                         0
#define HWCE_TRIGGER_ANY_WIDTH                                       32
#define HWCE_TRIGGER_ANY_MASK                                        0xffffffff
#define HWCE_TRIGGER_ANY_RESET                                       0x0

// If ERR is 0 then the ID of the offloaded job. Otherwise, part of the error code (access: R)
#define HWCE_ACQUIRE_ID_ERR_BIT                                      0
#define HWCE_ACQUIRE_ID_ERR_WIDTH                                    8
#define HWCE_ACQUIRE_ID_ERR_MASK                                     0xff
#define HWCE_ACQUIRE_ID_ERR_RESET                                    0x0

// An error code if one of the following conditions apply: 1. if the context copy is going on, it will answer 0xfffffffd (-3) 2. else, if the job offload lock has been established, it will answer 0xfffffffe (-2) 3. else, if the job queue is full, it will answer 0xffffffff (-1) (access: R)
#define HWCE_ACQUIRE_ERR_BIT                                         8
#define HWCE_ACQUIRE_ERR_WIDTH                                       24
#define HWCE_ACQUIRE_ERR_MASK                                        0xffffff00
#define HWCE_ACQUIRE_ERR_RESET                                       0x0

// The number of jobs that the HWCE executed and finished since the last time the same FINISHED_JOBS register was accessed. A read to FINISHED_JOBS returns: - 0x0 if no job was completed since the last access - 0x1 if a single job was completed since the last access - 0x2 if two or more jobs were completed since the last access (access: R)
#define HWCE_FINISHED_JOBS_JOBS_BIT                                  0
#define HWCE_FINISHED_JOBS_JOBS_WIDTH                                32
#define HWCE_FINISHED_JOBS_JOBS_MASK                                 0xffffffff
#define HWCE_FINISHED_JOBS_JOBS_RESET                                0x0

// Status of the HWCE - 1'b0 Not running a job - 1'b1 Running a job (access: R)
#define HWCE_STATUS_ST_BIT                                           0
#define HWCE_STATUS_ST_WIDTH                                         1
#define HWCE_STATUS_ST_MASK                                          0x1
#define HWCE_STATUS_ST_RESET                                         0x0

// ID of the currently running job (access: R)
#define HWCE_RUNNING_JOB_ID_BIT                                      0
#define HWCE_RUNNING_JOB_ID_WIDTH                                    8
#define HWCE_RUNNING_JOB_ID_MASK                                     0xff
#define HWCE_RUNNING_JOB_ID_RESET                                    0x0

// A write of any value to this register will reset the HWCE to its idle state. (access: W)
#define HWCE_SOFT_CLEAR_ANY_BIT                                      0
#define HWCE_SOFT_CLEAR_ANY_WIDTH                                    32
#define HWCE_SOFT_CLEAR_ANY_MASK                                     0xffffffff
#define HWCE_SOFT_CLEAR_ANY_RESET                                    0x0

// Fixed-point format. Pixels will be shifted to the right by QF bits in the normalization step after the sum-of-products stage. (access: R/W)
#define HWCE_GEN_CONFIG0_QF_BIT                                      0
#define HWCE_GEN_CONFIG0_QF_WIDTH                                    6
#define HWCE_GEN_CONFIG0_QF_MASK                                     0x3f
#define HWCE_GEN_CONFIG0_QF_RESET                                    0xc

// Operation type: - 1'b0 - Normal convolution - 1'b1 - Does not flip weights (i.e. implements a stencil instead of a mathematical convolution). (access: R/W)
#define HWCE_GEN_CONFIG0_NF_BIT                                      6
#define HWCE_GEN_CONFIG0_NF_WIDTH                                    1
#define HWCE_GEN_CONFIG0_NF_MASK                                     0x40
#define HWCE_GEN_CONFIG0_NF_RESET                                    0x0

// No y_in mode: - 1'b0 - Normal operation - 1'b1 - Disable loading of y_in and add a constant set in the CONFIG2 register (access: R/W)
#define HWCE_GEN_CONFIG0_NY_BIT                                      7
#define HWCE_GEN_CONFIG0_NY_WIDTH                                    1
#define HWCE_GEN_CONFIG0_NY_MASK                                     0x80
#define HWCE_GEN_CONFIG0_NY_RESET                                    0x0

// Set unsigned multiplication - 1'b0 - Consider multiplication results as signed fixed-point numbers. - 1'b1 - Consider multiplication results as unsigned fixed-point numbers. (access: R/W)
#define HWCE_GEN_CONFIG0_UNS_BIT                                     8
#define HWCE_GEN_CONFIG0_UNS_WIDTH                                   1
#define HWCE_GEN_CONFIG0_UNS_MASK                                    0x100
#define HWCE_GEN_CONFIG0_UNS_RESET                                   0x0

// Vector mode: - 2'b00 - mode is scalar (1 feat/cycle, 1x16-bit weights). - 2'b01 - mode is vectorial 2 (2 feat/cycle, 2x8-bit weights). - 2'b10 - mode is vectorial 4 (4 feat/cycle, 4x4-bit weights). (access: R/W)
#define HWCE_GEN_CONFIG0_VECT_BIT                                    9
#define HWCE_GEN_CONFIG0_VECT_WIDTH                                  2
#define HWCE_GEN_CONFIG0_VECT_MASK                                   0x600
#define HWCE_GEN_CONFIG0_VECT_RESET                                  0x0

// Convolution mode: - 2'b00 - mode is 5x5. - 2'b01 - mode is 3x3. - 2'b10 - mode is 4x7. (access: R/W)
#define HWCE_GEN_CONFIG0_CONV_BIT                                    11
#define HWCE_GEN_CONFIG0_CONV_WIDTH                                  2
#define HWCE_GEN_CONFIG0_CONV_MASK                                   0x1800
#define HWCE_GEN_CONFIG0_CONV_RESET                                  0x0

// No job copy: - 1'b0 - do job copy - 1'b1 - don't do job copy (access: R/W)
#define HWCE_GEN_CONFIG0_NCP_BIT                                     13
#define HWCE_GEN_CONFIG0_NCP_WIDTH                                   1
#define HWCE_GEN_CONFIG0_NCP_MASK                                    0x2000
#define HWCE_GEN_CONFIG0_NCP_RESET                                   0x0

// If 1, do normalization-and-rounding instead of only normalization. (access: R/W)
#define HWCE_GEN_CONFIG0_RND_BIT                                     14
#define HWCE_GEN_CONFIG0_RND_WIDTH                                   1
#define HWCE_GEN_CONFIG0_RND_MASK                                    0x4000
#define HWCE_GEN_CONFIG0_RND_RESET                                   0x0

// Stride between one FILTER_SIZExFILTER_SIZE filter and the next. (access: R/W)
#define HWCE_GEN_CONFIG0_WSTRIDE_BIT                                 16
#define HWCE_GEN_CONFIG0_WSTRIDE_WIDTH                               16
#define HWCE_GEN_CONFIG0_WSTRIDE_MASK                                0xffff0000
#define HWCE_GEN_CONFIG0_WSTRIDE_RESET                               0x0

// Shift input pixels to the left by this number of positions when PIXMODE is not 16bit. (access: R/W)
#define HWCE_GEN_CONFIG1_PIXSHIFTL_BIT                               0
#define HWCE_GEN_CONFIG1_PIXSHIFTL_WIDTH                             5
#define HWCE_GEN_CONFIG1_PIXSHIFTL_MASK                              0x1f
#define HWCE_GEN_CONFIG1_PIXSHIFTL_RESET                             0x0

// Input pixel size - 2'b00  16bit - 2'b01 - 8bit - 2'b10 - 4bit - 2'b11  16bit bis (access: R/W)
#define HWCE_GEN_CONFIG1_PIXMODE_BIT                                 8
#define HWCE_GEN_CONFIG1_PIXMODE_WIDTH                               2
#define HWCE_GEN_CONFIG1_PIXMODE_MASK                                0x300
#define HWCE_GEN_CONFIG1_PIXMODE_RESET                               0x0

// Shift output pixels to the right by this number of positions when PIXMODE is not 16bit. (access: R/W)
#define HWCE_GEN_CONFIG1_PIXSHIFTR_BIT                               16
#define HWCE_GEN_CONFIG1_PIXSHIFTR_WIDTH                             5
#define HWCE_GEN_CONFIG1_PIXSHIFTR_MASK                              0x1f0000
#define HWCE_GEN_CONFIG1_PIXSHIFTR_RESET                             0x0

// Contains the total number of words (i.e. double 16bit pixels, quad 8bit pixels, etc.) to be read / written by the streaming source and sink interfaces for y<sub>in</sub> and y<sub>out</sub> streams. (access: R/W)
#define HWCE_Y_TRANS_SIZE_SIZE_BIT                                   0
#define HWCE_Y_TRANS_SIZE_SIZE_WIDTH                                 32
#define HWCE_Y_TRANS_SIZE_SIZE_MASK                                  0xffffffff
#define HWCE_Y_TRANS_SIZE_SIZE_RESET                                 0x0

// Length of a line in number of words (access: R/W)
#define HWCE_Y_LINE_STRIDE_LENGTH_LENGTH_BIT                         0
#define HWCE_Y_LINE_STRIDE_LENGTH_LENGTH_WIDTH                       16
#define HWCE_Y_LINE_STRIDE_LENGTH_LENGTH_MASK                        0xffff
#define HWCE_Y_LINE_STRIDE_LENGTH_LENGTH_RESET                       0x0

// Distance in bytes between two consecutive lines. (access: R/W)
#define HWCE_Y_LINE_STRIDE_LENGTH_STRIDE_BIT                         16
#define HWCE_Y_LINE_STRIDE_LENGTH_STRIDE_WIDTH                       16
#define HWCE_Y_LINE_STRIDE_LENGTH_STRIDE_MASK                        0xffff0000
#define HWCE_Y_LINE_STRIDE_LENGTH_STRIDE_RESET                       0x0

// Length of a line in number of words (access: R/W)
#define HWCE_Y_FEAT_STRIDE_LENGTH_LENGTH_BIT                         0
#define HWCE_Y_FEAT_STRIDE_LENGTH_LENGTH_WIDTH                       16
#define HWCE_Y_FEAT_STRIDE_LENGTH_LENGTH_MASK                        0xffff
#define HWCE_Y_FEAT_STRIDE_LENGTH_LENGTH_RESET                       0x0

// Distance in bytes between two consecutive lines. (access: R/W)
#define HWCE_Y_FEAT_STRIDE_LENGTH_STRIDE_BIT                         16
#define HWCE_Y_FEAT_STRIDE_LENGTH_STRIDE_WIDTH                       16
#define HWCE_Y_FEAT_STRIDE_LENGTH_STRIDE_MASK                        0xffff0000
#define HWCE_Y_FEAT_STRIDE_LENGTH_STRIDE_RESET                       0x0

// Pointer into cluster L1 memory (4x4 bit mode) (access: R/W)
#define HWCE_Y_OUT_3_BASE_ADDR_ADDR_BIT                              0
#define HWCE_Y_OUT_3_BASE_ADDR_ADDR_WIDTH                            32
#define HWCE_Y_OUT_3_BASE_ADDR_ADDR_MASK                             0xffffffff
#define HWCE_Y_OUT_3_BASE_ADDR_ADDR_RESET                            0x0

// Pointer into cluster L1 memory (4x4 bit mode) (access: R/W)
#define HWCE_Y_OUT_2_BASE_ADDR_ADDR_BIT                              0
#define HWCE_Y_OUT_2_BASE_ADDR_ADDR_WIDTH                            32
#define HWCE_Y_OUT_2_BASE_ADDR_ADDR_MASK                             0xffffffff
#define HWCE_Y_OUT_2_BASE_ADDR_ADDR_RESET                            0x0

// Pointer into cluster L1 memory (4x4 bit and 2x8 bit modes) (access: R/W)
#define HWCE_Y_OUT_1_BASE_ADDR_ADDR_BIT                              0
#define HWCE_Y_OUT_1_BASE_ADDR_ADDR_WIDTH                            32
#define HWCE_Y_OUT_1_BASE_ADDR_ADDR_MASK                             0xffffffff
#define HWCE_Y_OUT_1_BASE_ADDR_ADDR_RESET                            0x0

// Pointer into cluster L1 memory (4x4 bit, 2x8 bit and 1x16 bit modes) (access: R/W)
#define HWCE_Y_OUT_0_BASE_ADDR_ADDR_BIT                              0
#define HWCE_Y_OUT_0_BASE_ADDR_ADDR_WIDTH                            32
#define HWCE_Y_OUT_0_BASE_ADDR_ADDR_MASK                             0xffffffff
#define HWCE_Y_OUT_0_BASE_ADDR_ADDR_RESET                            0x0

// Pointer into cluster L1 memory (4x4 bit mode) (access: R/W)
#define HWCE_Y_IN_3_BASE_ADDR_ADDR_BIT                               0
#define HWCE_Y_IN_3_BASE_ADDR_ADDR_WIDTH                             32
#define HWCE_Y_IN_3_BASE_ADDR_ADDR_MASK                              0xffffffff
#define HWCE_Y_IN_3_BASE_ADDR_ADDR_RESET                             0x0

// Pointer into cluster L1 memory (4x4 bit mode) (access: R/W)
#define HWCE_Y_IN_2_BASE_ADDR_ADDR_BIT                               0
#define HWCE_Y_IN_2_BASE_ADDR_ADDR_WIDTH                             32
#define HWCE_Y_IN_2_BASE_ADDR_ADDR_MASK                              0xffffffff
#define HWCE_Y_IN_2_BASE_ADDR_ADDR_RESET                             0x0

// Pointer into cluster L1 memory (4x4 bit and 2x8 bit modes) (access: R/W)
#define HWCE_Y_IN_1_BASE_ADDR_ADDR_BIT                               0
#define HWCE_Y_IN_1_BASE_ADDR_ADDR_WIDTH                             32
#define HWCE_Y_IN_1_BASE_ADDR_ADDR_MASK                              0xffffffff
#define HWCE_Y_IN_1_BASE_ADDR_ADDR_RESET                             0x0

// Pointer into cluster L1 memory (4x4 bit, 2x8 bit and 1x16 bit modes) (access: R/W)
#define HWCE_Y_IN_0_BASE_ADDR_ADDR_BIT                               0
#define HWCE_Y_IN_0_BASE_ADDR_ADDR_WIDTH                             32
#define HWCE_Y_IN_0_BASE_ADDR_ADDR_MASK                              0xffffffff
#define HWCE_Y_IN_0_BASE_ADDR_ADDR_RESET                             0x0

// Contains the total number of words (i.e. double 16bit pixels, quad 8bit pixels, etc.) to be read / written by the streaming source interface for the x<sub>in</sub>  stream. (access: R/W)
#define HWCE_X_TRANS_SIZE_SIZE_BIT                                   0
#define HWCE_X_TRANS_SIZE_SIZE_WIDTH                                 32
#define HWCE_X_TRANS_SIZE_SIZE_MASK                                  0xffffffff
#define HWCE_X_TRANS_SIZE_SIZE_RESET                                 0x0

// Length of a line in number of words (access: R/W)
#define HWCE_X_LINE_STRIDE_LENGTH_LENGTH_BIT                         0
#define HWCE_X_LINE_STRIDE_LENGTH_LENGTH_WIDTH                       16
#define HWCE_X_LINE_STRIDE_LENGTH_LENGTH_MASK                        0xffff
#define HWCE_X_LINE_STRIDE_LENGTH_LENGTH_RESET                       0x0

// Distance in bytes between two consecutive lines. (access: R/W)
#define HWCE_X_LINE_STRIDE_LENGTH_STRIDE_BIT                         16
#define HWCE_X_LINE_STRIDE_LENGTH_STRIDE_WIDTH                       16
#define HWCE_X_LINE_STRIDE_LENGTH_STRIDE_MASK                        0xffff0000
#define HWCE_X_LINE_STRIDE_LENGTH_STRIDE_RESET                       0x0

// Length of a line in number of words (access: R/W)
#define HWCE_X_FEAT_STRIDE_LENGTH_LENGTH_BIT                         0
#define HWCE_X_FEAT_STRIDE_LENGTH_LENGTH_WIDTH                       16
#define HWCE_X_FEAT_STRIDE_LENGTH_LENGTH_MASK                        0xffff
#define HWCE_X_FEAT_STRIDE_LENGTH_LENGTH_RESET                       0x0

// Distance in bytes between two consecutive lines. (access: R/W)
#define HWCE_X_FEAT_STRIDE_LENGTH_STRIDE_BIT                         16
#define HWCE_X_FEAT_STRIDE_LENGTH_STRIDE_WIDTH                       16
#define HWCE_X_FEAT_STRIDE_LENGTH_STRIDE_MASK                        0xffff0000
#define HWCE_X_FEAT_STRIDE_LENGTH_STRIDE_RESET                       0x0

// Pointer into cluster L1 memory (access: R/W)
#define HWCE_X_IN_BASE_ADDR_ADDR_BIT                                 0
#define HWCE_X_IN_BASE_ADDR_ADDR_WIDTH                               32
#define HWCE_X_IN_BASE_ADDR_ADDR_MASK                                0xffffffff
#define HWCE_X_IN_BASE_ADDR_ADDR_RESET                               0x0

// Pointer into cluster L1 memory (access: R/W)
#define HWCE_W_BASE_ADDR_ADDR_BIT                                    0
#define HWCE_W_BASE_ADDR_ADDR_WIDTH                                  32
#define HWCE_W_BASE_ADDR_ADDR_MASK                                   0xffffffff
#define HWCE_W_BASE_ADDR_ADDR_RESET                                  0x0

// Linebuffer virtual length. Set to the same number as X_LINE_LENGTH. Acceptable LBUFLEN values range between 2 and LINEBUF_LENGTH. (access: R/W)
#define HWCE_JOB_CONFIG0_LBUFLEN_BIT                                 0
#define HWCE_JOB_CONFIG0_LBUFLEN_WIDTH                               10
#define HWCE_JOB_CONFIG0_LBUFLEN_MASK                                0x3ff
#define HWCE_JOB_CONFIG0_LBUFLEN_RESET                               0x0

// Constant to sum instead of y<sub>in</sub> if the NY flag is active in the CONFIG1 register. (access: R/W)
#define HWCE_JOB_CONFIG0_NOYCONST_BIT                                16
#define HWCE_JOB_CONFIG0_NOYCONST_WIDTH                              16
#define HWCE_JOB_CONFIG0_NOYCONST_MASK                               0xffff0000
#define HWCE_JOB_CONFIG0_NOYCONST_RESET                              0x0

// Vector mode mask. Defaults to 0x0, which means that all vectors are enabled. Can be used to disable unused vector routes when using approximate vector or 3x3 mode. The bits are reversed in order, so bit 3 indicates vector 0, bit 2 vector 1, etc. (access: R/W)
#define HWCE_JOB_CONFIG1_VECT_DISABLE_MASK_BIT                       0
#define HWCE_JOB_CONFIG1_VECT_DISABLE_MASK_WIDTH                     4
#define HWCE_JOB_CONFIG1_VECT_DISABLE_MASK_MASK                      0xf
#define HWCE_JOB_CONFIG1_VECT_DISABLE_MASK_RESET                     0x0

// Output feature (OF) counter wrap parameter. If both WIF and WOF are 1'b0, the looping mechanism is disabled (access: R/W)
#define HWCE_JOB_CONFIG1_WOF_PARAM_BIT                               8
#define HWCE_JOB_CONFIG1_WOF_PARAM_WIDTH                             6
#define HWCE_JOB_CONFIG1_WOF_PARAM_MASK                              0x3f00
#define HWCE_JOB_CONFIG1_WOF_PARAM_RESET                             0x0

// Input feature (IF) counter wrap parameter. If both WIF and WOF are 1'b0, the looping mechanism is disabled (access: R/W)
#define HWCE_JOB_CONFIG1_WIF_PARAM_BIT                               16
#define HWCE_JOB_CONFIG1_WIF_PARAM_WIDTH                             6
#define HWCE_JOB_CONFIG1_WIF_PARAM_MASK                              0x3f0000
#define HWCE_JOB_CONFIG1_WIF_PARAM_RESET                             0x0

// Loop order: - 1'b0 - output features (OF) are the outer loop - 1'b1 - input features (IF) are the outer loop (access: R/W)
#define HWCE_JOB_CONFIG1_LO_BIT                                      24
#define HWCE_JOB_CONFIG1_LO_WIDTH                                    1
#define HWCE_JOB_CONFIG1_LO_MASK                                     0x1000000
#define HWCE_JOB_CONFIG1_LO_RESET                                    0x0

// Looping mechanism: - 1'b0 - both counters work as inner loops - 1'b1 - outer loop feature address is updated only when the feature counter reaches the wrap parameter, inner loop feature address is updated when the counter is less than the wrap parameter and reset when it is reached (access: R/W)
#define HWCE_JOB_CONFIG1_LN_BIT                                      25
#define HWCE_JOB_CONFIG1_LN_WIDTH                                    1
#define HWCE_JOB_CONFIG1_LN_MASK                                     0x2000000
#define HWCE_JOB_CONFIG1_LN_RESET                                    0x0

#endif
