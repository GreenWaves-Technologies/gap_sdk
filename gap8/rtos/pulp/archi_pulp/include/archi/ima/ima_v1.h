#ifndef __ARCHI_IMA_V1_H__
#define __ARCHI_IMA_V1_H__

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

/* 
 * Authors:  Geethan Karunaratne, IBM <kar@zurich.ibm.com>
 */

/************************* IMA defines**********************/
/*
 * Control and generic configuration register layout 
 *********************FOR IMA********************* 
 * ================================================================================
 *  # reg |  offset  |  bits   |   bitmask    ||  content
 * -------+----------+---------+--------------++-----------------------------------
 *     0  |  0x0000  |  31: 0  |  0xffffffff  ||  TRIGGER
 *     1  |  0x0004  |  31: 0  |  0xffffffff  ||  ACQUIRE
 *     2  |  0x0008  |  31: 0  |  0xffffffff  ||  FINISHED_JOBS
 *     3  |  0x000c  |  31: 0  |  0xffffffff  ||  STATUS
 *     4  |  0x0010  |  31: 0  |  0xffffffff  ||  RUNNING_TASK
 *     5  |  0x0014  |  31: 0  |  0xffffffff  ||  SOFT_CLEAR
 *     6  |  0x0018  |  31: 0  |  0xffffffff  ||  CHECK_STATE
 *     7  |          |         |              ||  Reserved
 * ================================================================================
 *
 * Job-dependent registers layout
 * ================================================================================
 *  # reg |  offset  |  bits   |   bitmask    ||  content
 * -------+----------+---------+--------------++-----------------------------------
 *     0  |  0x0020  |  31: 0  |  0xffffffff  ||  P_START_X
 *     1  |  0x0024  |  31: 0  |  0xffffffff  ||  P_START_Y
 *     2  |  0x0028  |  31: 0  |  0xffffffff  ||  P_WIDTH
 *     3  |  0x002c  |  31: 0  |  0xffffffff  ||  P_HIGHT
 *     4  |  0x0030  |  31: 0  |  0xffffffff  ||  SUBMIT_PLOT
 *     5  |  0x0034  |  31: 0  |  0xffffffff  ||  J_START_X
 *     6  |  0x0038  |  31:16  |  0xffffffff  ||  J_START_Y
 *     7  |  0x003c  |  31: 0  |  0xffffffff  ||  J_WIDTH
 *     8  |  0x0040  |  31: 0  |  0xffffffff  ||  J_HIGHT
 *     9  |  0x0044  |  31: 0  |  0xffffffff  ||  J_SRC_ADDR
 *    10  |  0x0048  |  31: 0  |  0xffffffff  ||  J_DST_ADDR
 *    11  |  0x004c  |  31: 0  |  0xffffffff  ||  J_SRC_STRID
 *    12  |  0x0050  |  31: 0  |  0xffffffff  ||  J_DST_STRID
 * 13-23  |          |         |              ||  Reserved
 *    24  |  0x0080  |  31: 0  |  0xffffffff  ||  PR_ADDR_X
 *    25  |  0x0084  |  31: 0  |  0xffffffff  ||  PR_ADDR_Y
 *    26  |  0x0088  |  31: 0  |  0xffffffff  ||  PR_VAL
 * ================================================================================
 *
 */

#define IMA_NB_REGS 		43

#define IMA_TRIGGER             0x00
#define IMA_ACQUIRE             0x04
#define IMA_FINISHED_JOBS       0x08
#define IMA_STATUS              0x0c
#define IMA_RUNNING_TASK        0x10
#define IMA_SOFT_CLEAR          0x14
#define IMA_CHECK_STATE         0x18

#define IMA_P_START_X           0x20
#define IMA_P_START_Y           0x24
#define IMA_P_WIDTH             0x28
#define IMA_P_HIGHT             0x2c
#define IMA_SUBMIT_PLOT         0x30
#define IMA_J_START_X           0x34
#define IMA_J_START_Y           0x38
#define IMA_J_WIDTH             0x3c
#define IMA_J_HIGHT             0x40
#define IMA_J_SRC_ADDR          0x44
#define IMA_J_DST_ADDR          0x48
#define IMA_J_SRC_STRID         0x4c
#define IMA_J_DST_STRID         0x50

#define IMA_ADC_LOW             0x54
#define IMA_ADC_HIGH            0x58

#define IMA_FETCH_LENGTH        0x5c
#define IMA_STORE_LENGTH        0x60
#define IMA_JOB_LINE_LENGTH     0x64
#define IMA_JOB_FEAT_STRIDE     0x68
#define IMA_JOB_FEAT_LENGTH     0x6c

#define IMA_NUM_JOBS            0x70

#define IMA_ALPHA_IN_LENGTH     0x74
#define IMA_ALPHA_IN_STRIDE     0x78
#define IMA_BETA_IN_LENGTH      0x7c
#define IMA_BETA_IN_STRIDE      0x80

#define IMA_ALPHA_OUT_LENGTH    0x84
#define IMA_ALPHA_OUT_STRIDE    0x88
#define IMA_BETA_OUT_LENGTH     0x8c
#define IMA_BETA_OUT_STRIDE     0x90

#define IMA_JOB_LL_MEMT         0x94
#define IMA_JOB_FEAT_ROLL       0x98
#define IMA_DW_MODE             0x9c

#define IMA_PR_ADDR_X           0xa0
#define IMA_PR_ADDR_Y           0xa4
#define IMA_PR_VAL              0xa8

#define STOR_DWIDTH             4
#define DAC_PRECISION           8
#define ADC_PRECISION           8

#endif