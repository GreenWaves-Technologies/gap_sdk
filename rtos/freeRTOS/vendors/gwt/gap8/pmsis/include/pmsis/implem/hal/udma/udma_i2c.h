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

#ifndef __PI_HAL_UDMA_I2C_H__
#define __PI_HAL_UDMA_I2C_H__

#include "pmsis/targets/target.h"
#include "pmsis/implem/hal/udma/udma_core.h"
#include "pmsis/implem/hal/udma/udma_ctrl.h"

/* I2C udma configuration. */
static inline void i2c_udma_channel_set(uint32_t device_id, udma_channel_e channel,
                                        uint32_t l2buf, uint32_t size, uint32_t cfg)
{
    udma_enqueue_channel(&(i2c(device_id)->udma), l2buf, size, cfg, channel);
}


/* Status register. */
static inline void i2c_status_set(uint32_t device_id, uint32_t status)
{
    hal_write32(&(i2c(device_id)->status), status);
}

static inline uint32_t i2c_status_get(uint32_t device_id)
{
    return hal_read32(&(i2c(device_id)->status));
}


/* Setup register. */
static inline void i2c_setup_set(uint32_t device_id, uint32_t setup)
{
    hal_write32(&(i2c(device_id)->setup), setup);
}

static inline uint32_t i2c_setup_get(uint32_t device_id)
{
    return hal_read32(&(i2c(device_id)->setup));
}


/*! STATUS. */
/* i2c busy check. */
static inline uint32_t hal_i2c_busy_get(uint32_t device_id)
{
    return (i2c_status_get(device_id) & I2C_STATUS_BUSY_MASK);
}

/* Enable arbitration lost error. */
static inline void hal_i2c_arbitration_set(uint32_t device_id, uint8_t value)
{
    i2c_status_set(device_id,
                   (i2c_status_get(device_id) & ~I2C_STATUS_ARB_LOST_MASK)
                   | I2C_STATUS_ARB_LOST(value));
}

static inline uint32_t hal_i2c_arbitration_get(uint32_t device_id)
{
    uint32_t status = i2c_status_get(device_id);
    return  ((status & I2C_STATUS_ARB_LOST_MASK) >> I2C_STATUS_ARB_LOST_SHIFT);
}


/*! SETUP. */
/* Abort/Reset on-going transfer & clear busy & arbitration flags. */
static inline void hal_i2c_reset_set(uint32_t device_id, uint8_t value)
{
    i2c_setup_set(device_id, I2C_SETUP_DO_RST(value));
}

static inline uint32_t hal_i2c_do_reset_get(uint32_t device_id)
{
    return i2c_setup_get(device_id) & ~I2C_SETUP_DO_RST_MASK;
}


/*! UDMA.  */
/* Enqueue transfer on UDMA channels. */
static inline void hal_i2c_enqueue(uint32_t device_id, uint32_t channel,
                                   uint32_t l2buf, uint32_t size, uint32_t cfg)
{
    /* Enqueue l2 buffer & start transfer. */
    i2c_udma_channel_set(device_id, channel, l2buf, size, cfg);
}

#endif  /* __PI_HAL_UDMA_I2C_H__ */
