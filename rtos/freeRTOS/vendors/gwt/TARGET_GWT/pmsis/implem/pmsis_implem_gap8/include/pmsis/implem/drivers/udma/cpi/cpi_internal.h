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

#ifndef __PMSIS_IMPLEM_DRIVERS_UDMA_CPI_CPI_INTERNAL_H__
#define __PMSIS_IMPLEM_DRIVERS_UDMA_CPI_CPI_INTERNAL_H__

#include "pmsis.h"
#include "pmsis/task.h"
#include "pmsis/drivers/cpi.h"
#include "pmsis/implem/hal/hal.h"

/*******************************************************************************
 * Definitions
 ******************************************************************************/

/*
 * pi_task:
 * data[0] = l2_buf
 * data[1] = size
 * data[2] = repeat_size ?
 * data[3] = skip current event handling
 * data[4] = udma cfg
 */

struct cpi_itf_data_s
{
    struct pi_task *hw_buffer[2]; /* 2 HW buffers for RX */
    struct pi_task *fifo_head;
    struct pi_task *fifo_tail;
    uint32_t device_id;
    uint32_t nb_open;
};

/*******************************************************************************
 * Driver data
 *****************************************************************************/

/*******************************************************************************
 * Function declaration
 ******************************************************************************/

/* Init conf. */
void __pi_cpi_conf_init(struct pi_cpi_conf *conf);

/* Open device. */
int32_t __pi_cpi_open(struct pi_cpi_conf *conf, struct cpi_itf_data_s **device_data);

/* Close device. */
void __pi_cpi_close(struct cpi_itf_data_s *device_data);

/* Copy in UDMA. */
void __pi_cpi_copy(struct cpi_itf_data_s *device_data, void *l2_buf,
                   uint32_t size, struct pi_task *task);

/*******************************************************************************
 * API implementation
 ******************************************************************************/

static inline void pi_cpi_control_start(struct pi_device *device)
{
    struct cpi_itf_data_s *device_data =  (struct cpi_itf_data_s *) device->data;
    cpi_glob_en_set(device_data->device_id, 1);
}

static inline void pi_cpi_control_stop(struct pi_device *device)
{
    struct cpi_itf_data_s *device_data =  (struct cpi_itf_data_s *) device->data;
    cpi_glob_en_set(device_data->device_id, 0);
}

static inline void pi_cpi_set_format(struct pi_device *device,
                                     pi_cpi_format_e format)
{
    struct cpi_itf_data_s *device_data =  (struct cpi_itf_data_s *) device->data;
    cpi_format_set(device_data->device_id, format);
}

static inline void pi_cpi_set_frame_drop(struct pi_device *device,
                                         uint32_t nb_frame_dropped)
{
    struct cpi_itf_data_s *device_data =  (struct cpi_itf_data_s *) device->data;
    cpi_framedrop_en_set(device_data->device_id, nb_frame_dropped);
    cpi_framedrop_val_set(device_data->device_id, nb_frame_dropped);
}

static inline void pi_cpi_set_filter(struct pi_device *device, uint32_t r_coeff,
                                     uint32_t g_coeff, uint32_t b_coeff, uint32_t shift)
{
    struct cpi_itf_data_s *device_data =  (struct cpi_itf_data_s *) device->data;
    cpi_shift_set(device_data->device_id, shift);
    hal_cpi_filter_rgb_coeff_set(device_data->device_id, r_coeff, g_coeff, b_coeff);
}

static inline void pi_cpi_set_slice(struct pi_device *device,
                                    uint32_t x, uint32_t y, uint32_t w, uint32_t h)
{
    struct cpi_itf_data_s *device_data =  (struct cpi_itf_data_s *) device->data;
    cpi_frameslice_en_set(device_data->device_id, w);
    if (w)
    {
        /* Lower left. */
        hal_cpi_frameslice_ll_set(device_data->device_id, x, y);
        /* Upper right. */
        hal_cpi_frameslice_ur_set(device_data->device_id, (x + w - 1),  (y + h - 1));
        /* Size. */
        cpi_rowlen_set(device_data->device_id, (w - 1));
    }
}

#endif  /* __PMSIS_IMPLEM_DRIVERS_UDMA_CPI_CPI_INTERNAL_H__ */
