/*
 * Copyright (c) 2017, GreenWaves Technologies, Inc.
 * All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without modification,
 * are permitted provided that the following conditions are met:
 *
 * o Redistributions of source code must retain the above copyright notice, this list
 *   of conditions and the following disclaimer.
 *
 * o Redistributions in binary form must reproduce the above copyright notice, this
 *   list of conditions and the following disclaimer in the documentation and/or
 *   other materials provided with the distribution.
 *
 * o Neither the name of GreenWaves Technologies, Inc. nor the names of its
 *   contributors may be used to endorse or promote products derived from this
 *   software without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND
 * ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED
 * WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
 * DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE FOR
 * ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES
 * (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES;
 * LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON
 * ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
 * (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS
 * SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 */


#ifndef _GAP_HWCE_H_
#define _GAP_HWCE_H_

#include <cmsis.h>
#include "gap_util.h"

/*!
 * @addtogroup dmamux
 * @{
 */

#ifdef FEATURE_CLUSTER

#define NB_HWCE 1

/*
 * Control and generic configuration register layout
 * =====================================================================
 *  # reg |  bits   |   bitmask    ||  content
 * -------+---------+--------------++-----------------------------------
 *     0  |  31: 0  |  0xffffffff  ||  TRIGGER
 *     1  |  31: 0  |  0xffffffff  ||  ACQUIRE
 *     2  |  31: 0  |  0xffffffff  ||  EVT_ENABLE
 *     3  |  31: 0  |  0xffffffff  ||  STATUS
 *     4  |  31: 0  |  0xffffffff  ||  RUNNING_JOB
 *     5  |  31: 0  |  0xffffffff  ||  SOFT_CLEAR
 *   6-7  |         |              ||  Reserved
 *     8  |  31:16  |  0xffff0000  ||  WSTRIDE
 *        |  15:13  |  0x0000e000  ||  Reserved
 *        |  12:11  |  0x00001800  ||  CONV
 *        |  10: 9  |  0x00000600  ||  VECT
 *        |      8  |  0x00000100  ||  UNS
 *        |      7  |  0x00000080  ||  NY
 *        |      6  |  0x00000040  ||  LC
 *        |   5: 0  |  0x0000003f  ||  QF
 *     9  |  31: 0  |  0xffffffff  ||  GEN_CONFIG1 (reserved)
 * =====================================================================
 *
 * Register layout in case of NB_Y_STREAMS = 4
 * =====================================================================
 *  # reg |  bits   |   bitmask    ||  content
 * -------+---------+--------------++-----------------------------------
 *     0  |  31: 0  |  0xffffffff  ||  y trans_size
 *     1  |  31:16  |  0xffff0000  ||  y line_stride
 *        |  15: 0  |  0x0000ffff  ||  y line_length
 *     2  |  31:16  |  0xffff0000  ||  y feat_stride
 *        |  15: 0  |  0x0000ffff  ||  y feat_length
 *     3  |  31: 0  |  0xffffffff  ||  y_out[0] base_addr (logic y_out[3])
 *     4  |  31: 0  |  0xffffffff  ||  y_out[1] base_addr (logic y_out[2])
 *     5  |  31: 0  |  0xffffffff  ||  y_out[2] base_addr (logic y_out[1])
 *     6  |  31: 0  |  0xffffffff  ||  y_out[3] base_addr (logic y_out[0])
 *     7  |  31: 0  |  0xffffffff  ||  y_in[0] base_addr (logic y_in[3])
 *     8  |  31: 0  |  0xffffffff  ||  y_in[1] base_addr (logic y_in[2])
 *     9  |  31: 0  |  0xffffffff  ||  y_in[2] base_addr (logic y_in[1])
 *    10  |  31: 0  |  0xffffffff  ||  y_in[3] base_addr (logic y_in[0])
 *    11  |  31: 0  |  0xffffffff  ||  x trans_size
 *    12  |  31:16  |  0xffff0000  ||  x line_stride
 *        |  15: 0  |  0x0000ffff  ||  x line_length
 *    13  |  31:16  |  0xffff0000  ||  x feat_stride
 *        |  15: 0  |  0x0000ffff  ||  x feat_length
 *    14  |  31: 0  |  0xffffffff  ||  x_in base_addr
 *    15  |  31: 0  |  0xffffffff  ||  weight base_addr
 *    16  |  31:16  |  0xffff0000  ||  constant to sum in no y_in mode
 *        |  15:10  |  0x0000fa00  ||  Reserved
 *        |   9: 0  |  0x0000ffff  ||  linebuffer virtual length
 *    17  |  31:25  |  0xfe000000  ||  Reserved
 *        |     24  |  0x01000000  ||  loop order
 *        |  23:22  |  0x00c00000  ||  Reserved
 *        |  21:16  |  0x003f0000  ||  input feat map rolling parameter
 *        |  15:14  |  0x0000c000  ||  Reserved
 *        |  13: 8  |  0x00003f00  ||  input feat map rolling parameter
 *        |   7: 4  |  0x000000f0  ||  Reserved
 *        |   3: 0  |  0x0000000f  ||  vector disable mask
 * =====================================================================
 *
 */


// This data structure represents context-dependent registers
typedef struct {
    unsigned       int y_trans_size;
    unsigned short int y_line_length;
    unsigned short int y_line_stride;
    unsigned short int y_feat_length;
    unsigned short int y_feat_stride;
             short int *y_out3_ptr;
             short int *y_out2_ptr;
             short int *y_out1_ptr;
             short int *y_out0_ptr;
             short int *y_in3_ptr;
             short int *y_in2_ptr;
             short int *y_in1_ptr;
             short int *y_in0_ptr;
    unsigned       int x_trans_size;
    unsigned short int x_line_length;
    unsigned short int x_line_stride;
    unsigned short int x_feat_length;
    unsigned short int x_feat_stride;
             short int *x_in_ptr;
    unsigned       int weight_ptr;
    unsigned       int constsum_linebuflen;
    unsigned       int lo_wif_wof_vdis;
} __attribute__((__packed__)) hwce_config_t;

// This data structure represent other registers + unused mmap space
typedef struct {
    unsigned int trigger;
    unsigned int acquire;
    unsigned int sync_mode;
    unsigned int status;
    unsigned int running_job;
    unsigned int soft_clear;
    unsigned int reserved_mandatory_0;
    unsigned int reserved_mandatory_1;
    unsigned int wstride_conv_vect_uint_noyin_lin_qf;
    unsigned int pix_shiftr_mode_shiftl;
    unsigned int reserved_generic_2;
    unsigned int reserved_generic_3;
    unsigned int reserved_generic_4;
    unsigned int reserved_generic_5;
    unsigned int reserved_generic_6;
    unsigned int reserved_generic_7;
    hwce_config_t hwce_cfg;
} __attribute__((__packed__)) hwce_mmap_t;

// linebuffer width in 16-bit half-words
#define HWCE_LBSIZE 32

/* LOW-LEVEL HAL */
#define HWCE_ADDR_BASE  HWCE_BASE
#define HWCE_ADDR_SPACE 0x00000100

static volatile hwce_mmap_t *hwce_memory_map = (hwce_mmap_t *) (HWCE_ADDR_BASE);

#define HWCE_WRITE(value, offset) ( HWCE->offset##_REG = value )
#define HWCE_READ(offset)         ( HWCE->offset##_REG )

/*******************************************************************************
 * Definitions
 ******************************************************************************/

static inline void hwce_y_trans_size_set(uint32_t y_trans)
{
    HWCE->HWCE_Y_TRANS_SIZE_REG = y_trans;
}

static inline void hwce_y_line_stride_length_set(uint32_t y_line)
{
    HWCE->HWCE_Y_LINE_STRIDE_LENGTH_REG = y_line;
}

static inline void hwce_y_feat_stride_length_set(uint32_t y_feat)
{
    HWCE->HWCE_Y_FEAT_STRIDE_LENGTH_REG = y_feat;
}

static inline void hwce_y_out_0_addr_set(uint32_t y_out_0)
{
    HWCE->HWCE_Y_OUT_0_REG = y_out_0;
}

static inline void hwce_y_out_1_addr_set(uint32_t y_out_1)
{
    HWCE->HWCE_Y_OUT_1_REG = y_out_1;
}

static inline void hwce_y_out_2_addr_set(uint32_t y_out_2)
{
    HWCE->HWCE_Y_OUT_2_REG = y_out_2;
}

static inline void hwce_y_out_3_addr_set(uint32_t y_out_3)
{
    HWCE->HWCE_Y_OUT_3_REG = y_out_3;
}

static inline void hwce_y_in_0_addr_set(uint32_t y_in_0)
{
    HWCE->HWCE_Y_IN_0_REG = y_in_0;
}

static inline void hwce_y_in_1_addr_set(uint32_t y_in_1)
{
    HWCE->HWCE_Y_IN_1_REG = y_in_1;
}

static inline void hwce_y_in_2_addr_set(uint32_t y_in_2)
{
    HWCE->HWCE_Y_IN_2_REG = y_in_2;
}

static inline void hwce_y_in_3_addr_set(uint32_t y_in_3)
{
    HWCE->HWCE_Y_IN_3_REG = y_in_3;
}

static inline void hwce_x_trans_size_set(uint32_t x_trans)
{
    HWCE->HWCE_X_TRANS_SIZE_REG = x_trans;
}

static inline void hwce_x_line_stride_length_set(uint32_t x_line)
{
    HWCE->HWCE_X_LINE_STRIDE_LENGTH_REG = x_line;
}

static inline void hwce_x_feat_stride_length_set(uint32_t x_feat)
{
    HWCE->HWCE_X_FEAT_STRIDE_LENGTH_REG = x_feat;
}

static inline void hwce_x_in_0_addr_set(uint32_t x_in)
{
    HWCE->HWCE_X_IN_REG = x_in;
}

static inline void hwce_w_base_addr_set(uint32_t write)
{
    HWCE->HWCE_W_REG = write;
}

static inline void hwce_gen_config0_set(uint32_t config)
{
    HWCE->HWCE_GEN_CONFIG0_REG = config;
}

static inline void hwce_gen_config1_set(uint32_t config)
{
    HWCE->HWCE_GEN_CONFIG1_REG = config;
}

static inline void hwce_job_config0_set(uint32_t config)
{
    HWCE->HWCE_JOB_CONFIG0_REG = config;
}

static inline void hwce_job_config1_set(uint32_t config)
{
    HWCE->HWCE_JOB_CONFIG1_REG = config;
}

static inline void hwce_trigger_job(void)
{
    HWCE->HWCE_TRIGGER_REG = 0;
}

static inline int32_t hwce_acquire_job(void)
{
    return HWCE->HWCE_ACQUIRE_REG;
}

static inline void hwce_soft_clear(void)
{
    HWCE->HWCE_SOFT_CLEAR_REG = 1;
    for(uint32_t i = 0; i < 3; i++);
    HWCE->HWCE_SOFT_CLEAR_REG = 0;
}

static inline void hwce_wait_event( void )
{
    EU_EVT_MaskWaitAndClr(1 << EU_HWCE_EVENT);
}

static inline void plp_hwce_enable( void )
{
    *(volatile uint32_t*) (CORE_PERI_BASE + (3 << 3)) |= 0xc00;
}

static inline void plp_hwce_disable( void )
{
    *(volatile uint32_t*) (CORE_PERI_BASE + (3 << 3)) &= 0xc00;
}

static inline uint32_t hwce_stride_length_value(uint32_t stride, uint32_t length)
{
    uint32_t res = 0;
    res = __BIT_INSERT(0  , stride, 16, 16);
    res = __BIT_INSERT(res, length, 16, 0);
    return res;
}

static inline uint32_t hwce_gen_config0_value(uint32_t wstride, uint32_t ncp,
                                              uint32_t conv, unsigned vect,
                                              uint32_t uns, uint32_t ny,
                                              uint32_t nf, uint32_t qf, uint32_t rnd)
{
    uint32_t res = 0;
    res = __BIT_INSERT(0  , wstride, 16, 16);
    res = __BIT_INSERT(res, rnd    , 1 , 14);
    res = __BIT_INSERT(res, ncp    , 1 , 13);
    res = __BIT_INSERT(res, conv   , 2 , 11);
    res = __BIT_INSERT(res, vect   , 2 , 9);
    res = __BIT_INSERT(res, uns    , 1 , 8);
    res = __BIT_INSERT(res, ny     , 1 , 7);
    res = __BIT_INSERT(res, nf     , 1 , 6);
    res = __BIT_INSERT(res, qf     , 6 , 0);
    return res;
}

static inline uint32_t hwce_gen_config1_value(uint32_t pixshiftr, uint32_t pixmode,
                                              uint32_t pixshiftl)
{
    uint32_t res;
    res = __BIT_INSERT(0  , pixshiftr, 5, 16);
    res = __BIT_INSERT(res, pixmode  , 2, 8);
    res = __BIT_INSERT(res, pixshiftl, 5, 0);
    return res;
}

static inline uint32_t hwce_job_config0_value(uint32_t noyconst, uint32_t lbuflen) {
    uint32_t res = 0;
    res = __BIT_INSERT(0  , noyconst, 16, 16);
    res = __BIT_INSERT(res, lbuflen , 10, 0);
    return res;
}

static inline uint32_t hwce_job_config1_value(uint32_t lo, uint32_t wif,
                                              uint32_t wof, uint32_t vect_disable_mask)
{
    uint32_t res;
    res = __BIT_INSERT(0  , lo               , 2, 24);
    res = __BIT_INSERT(res, wif              , 6, 16);
    res = __BIT_INSERT(res, wof              , 6, 8);
    res = __BIT_INSERT(res, vect_disable_mask, 4, 0);
    return res;
}



#endif /* FEATURE_CLUSTER */

/* @} */

#endif /* _GAP_DMAMCHAN_H_ */
