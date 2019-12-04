/*
 * Copyright (c) 2019, GreenWaves Technologies, Inc.
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

#ifndef __HYPERBUS_INTERNAL_H__
#define __HYPERBUS_INTERNAL_H__

#include "pmsis.h"
#include "pmsis_hal/pmsis_hal.h"

/*******************************************************************************
 * Definitions
 ******************************************************************************/

#define BURST_SIZE 512

/*******************************************************************************
 * Driver data
 *****************************************************************************/

/* HyperFlash status register device ready offset. */
#define DEVICE_READY_OFFSET     7
/* Write and read address */
#define SA                      0x0000

/*! @brief HyperFlash command sequence structure. */
typedef struct
{
    uint16_t data;              /*!< Command data. */
    uint16_t addr;              /*!< Commad address. */
} cmdSeq;


/* Sector erase sequence */
static cmdSeq Erase_Seq[6] = {{0xAA, 0x555}, {0x55, 0x2AA}, {0x80, 0x555},
                              {0xAA, 0x555}, {0x55, 0x2AA}, {0x30, SA}};

/* Configure register0 sequence */
static cmdSeq VCR_Seq[4]   = {{0xAA, 0x555}, {0x55, 0x2AA}, {0x38, 0x555}, {0x8e0b, 0x0}};

/* Read status register sequence */
static cmdSeq Reg_Seq      = {0x70, 0x555};

/* Write 512/4 = 128 word to Sector addr 0x4xxx */
static cmdSeq WP_Seq[3]    = {{0xAA, 0x555}, {0x55, 0x2AA}, {0xA0, 0x555}};

/*******************************************************************************
 * Function declaration
 ******************************************************************************/

/* Hyper config. */
void __pi_hyper_settings(uint8_t chip_select);

/* Pad settings. */
void __pi_hyper_pin_settings();
void __pi_hyper_pin_reset_settings();

/* Hyperflash config. */
void __pi_hyper_flash_config(struct pi_device *device);

/* Handler. */
void hyper_handler(void *arg);

/* Extern memory alloc. */
uint32_t __pi_hyper_alloc_init(malloc_t *alloc, uint32_t heapstart, int32_t size);
void __pi_hyper_alloc_deinit(malloc_t *alloc);

/* Copy in UDMA. */
void __pi_hyper_copy(int device, struct hyper_transfer_s *transfer,
                     struct pi_task *task);
void __pi_hyper_copy_2d(int device, struct hyper_transfer_s *transfer,
                        struct pi_task *task);

#endif  /* __HYPERBUS_INTERNAL_H__ */
