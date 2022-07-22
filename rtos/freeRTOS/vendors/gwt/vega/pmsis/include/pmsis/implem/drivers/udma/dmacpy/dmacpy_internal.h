/*
 * Copyright (c) 2020, GreenWaves Technologies, Inc.
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

#ifndef __PMSIS_DRIVER_UDMA_DMACPY_DMACPY_INTERNAL_H__
#define __PMSIS_DRIVER_UDMA_DMACPY_DMACPY_INTERNAL_H__

#include "pmsis.h"

/*******************************************************************************
 * Definitions
 ******************************************************************************/

struct dmacpy_itf_data_s
{
    /* Best to use only one queue since both RX & TX can be used at the same time. */
    struct pi_task *fifo_head;  /*!< Head of SW fifo waiting transfers. */
    struct pi_task *fifo_tail;  /*!< Tail of SW fifo waiting transfers. */
    uint32_t device_id;         /*!< Device ID. */
    uint32_t nb_open;           /*!< Number of users on this device. */
};

/*
 * pi_task:
 * data[0] = l2_buf_0
 * data[1] = l2_buf_1
 * data[2] = cpy_src
 * data[3] = cpy_dst
 * data[4] = size
 * data[5] = dir
 * data[6] = ?
 */

/*******************************************************************************
 * Driver data
 *****************************************************************************/

/*******************************************************************************
 * Function declaration
 ******************************************************************************/

void __pi_dmacpy_conf_init(struct pi_dmacpy_conf *conf);

int32_t __pi_dmacpy_open(struct pi_dmacpy_conf *conf,
                         struct dmacpy_itf_data_s **device_data);

void __pi_dmacpy_close(uint32_t device_id);

int32_t __pi_dmacpy_copy(uint32_t device_id, void *src, void *dst,
                         uint32_t size, pi_dmacpy_dir_e dir, struct pi_task *task);

#endif  /* __PMSIS_DRIVER_UDMA_DMACPY_DMACPY_INTERNAL_H__ */
