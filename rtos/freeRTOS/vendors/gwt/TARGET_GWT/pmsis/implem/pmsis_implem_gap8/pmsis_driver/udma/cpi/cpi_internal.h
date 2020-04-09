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

#ifndef __CPI_INTERNAL_H__
#define __CPI_INTERNAL_H__

#include "pmsis.h"
#include "pmsis/task.h"
#include "pmsis/drivers/cpi.h"
#include "pmsis_hal/pmsis_hal.h"

/*******************************************************************************
 * Definitions
 ******************************************************************************/

struct cpi_transfer_s
{
    void *buffer;
    uint32_t size;
    int8_t device_id;
};

#if 1
struct cpi_cb_args_s
{
    struct pi_task *cb;
    struct cpi_transfer_s transfer;
    struct cpi_cb_args_s *next;
};
#endif

struct cpi_driver_fifo_s
{
    struct pi_task *hw_buffer[2];
    struct pi_task *hw_fifo_head;
    struct pi_task *hw_fifo_tail;
    struct cpi_cb_args_s *fifo_head;
    struct cpi_cb_args_s *fifo_tail;
};

/*******************************************************************************
 * Driver data
 *****************************************************************************/

/*******************************************************************************
 * Function declaration
 ******************************************************************************/

/* Copy in UDMA. */
//void __pi_cpi_copy(struct pi_device *device, struct cpi_transfer_s *transfer, struct pi_task *task);
void __pi_cpi_copy(struct cpi_driver_fifo_s *fifo, struct cpi_transfer_s *transfer, struct pi_task *task);
//void __pi_cpi_copy(struct cpi_transfer_s *transfer, struct pi_task *task);

/* Handler. */
void cpi_handler(void *arg);

/*******************************************************************************
 * API implementation
 ******************************************************************************/

static inline void pi_cpi_control_start(struct pi_device *device)
{
    struct pi_cpi_conf *conf = (struct pi_cpi_conf *) device->config;
    cpi_glob_en_set(conf->itf, 1);
}

static inline void pi_cpi_control_stop(struct pi_device *device)
{
    struct pi_cpi_conf *conf = (struct pi_cpi_conf *) device->config;
    cpi_glob_en_set(conf->itf, 0);
}

static inline void pi_cpi_set_format(struct pi_device *device,
                                     pi_cpi_format_e format)
{
    struct pi_cpi_conf *conf = (struct pi_cpi_conf *) device->config;
    cpi_format_set(conf->itf, format);
}

static inline void pi_cpi_set_frame_drop(struct pi_device *device,
                                         uint32_t nb_frame_dropped)
{
    struct pi_cpi_conf *conf = (struct pi_cpi_conf *) device->config;
    cpi_framedrop_en_set(conf->itf, nb_frame_dropped);
    cpi_framedrop_val_set(conf->itf, nb_frame_dropped);
}

static inline void pi_cpi_set_filter(struct pi_device *device, uint32_t r_coeff,
                                     uint32_t g_coeff, uint32_t b_coeff, uint32_t shift)
{
    struct pi_cpi_conf *conf = (struct pi_cpi_conf *) device->config;
    cpi_shift_set(conf->itf, shift);
    cpi_filter_b_coeff_set(conf->itf, b_coeff);
    cpi_filter_g_coeff_set(conf->itf, g_coeff);
    cpi_filter_r_coeff_set(conf->itf, r_coeff);
}

static inline void pi_cpi_set_slice(struct pi_device *device,
                                    uint32_t x, uint32_t y, uint32_t w, uint32_t h)
{
    struct pi_cpi_conf *conf = (struct pi_cpi_conf *) device->config;
    cpi_frameslice_en_set(conf->itf, w);
    if (w)
    {
        /* Lower left. */
        cpi_frameslice_llx_set(conf->itf, x);
        cpi_frameslice_lly_set(conf->itf, y);
        /* Upper right. */
        cpi_frameslice_urx_set(conf->itf, x + w - 1);
        cpi_frameslice_ury_set(conf->itf, y + h - 1);
        /* Size. */
        cpi_rowlen_set(conf->itf, w - 1);
    }
}

#endif /* __CPI_INTERNAL_H__ */
