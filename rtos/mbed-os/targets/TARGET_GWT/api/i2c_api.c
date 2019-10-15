/*
 * Copyright (c) 2018, GreenWaves Technologies, Inc.
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

#include <math.h>
#include "mbed_assert.h"

#if DEVICE_I2C

#include "cmsis.h"
#include "pinmap.h"
#include "mbed_error.h"
#include "PeripheralPins.h"
#include "i2c_api.h"
#include "gap_i2c.h"


static I2C_Type *const i2c_address[] = I2C_BASE_PTRS;

/* I2C read return value */
static uint8_t i2c_reg_value;

/* I2C slave temporary saved address */
static uint8_t slave_address = 0;

void i2c_init(i2c_t *obj, PinName sda, PinName scl)
{
    uint32_t i2c_sda = pinmap_peripheral(sda, PinMap_I2C_SDA);
    uint32_t i2c_scl = pinmap_peripheral(scl, PinMap_I2C_SCL);

    obj->i2c.instance = pinmap_merge(i2c_sda, i2c_scl);
    MBED_ASSERT((int)obj->i2c.instance != NC);

    pinmap_pinout(sda, PinMap_I2C_SDA);
    pinmap_pinout(scl, PinMap_I2C_SCL);

    i2c_config_t config;

    I2C_GetDefaultConfig(&config);

    I2C_Init(i2c_address[obj->i2c.instance], &config, SystemCoreClock);
}

static inline void i2c_buffer_set(i2c_t *obj, const void *tx, uint32_t tx_length, void *rx, uint32_t rx_length, uint8_t bit_width)
{
    obj->tx_buff.buffer = (void *)tx;
    obj->rx_buff.buffer = rx;
    obj->tx_buff.length = tx_length;
    obj->rx_buff.length = rx_length;
    obj->tx_buff.pos = 0;
    obj->rx_buff.pos = 0;
    obj->tx_buff.width = bit_width;
    obj->rx_buff.width = bit_width;
}

void i2c_frequency(i2c_t *obj, int hz)
{
    uint32_t busClock = SystemCoreClock;
    I2C_SetBaudRate(i2c_address[obj->i2c.instance], hz, busClock);
}

/** Send START command
 *
 *  @param obj The I2C object
 */
int  i2c_start(i2c_t *obj)
{
    return I2C_Start(i2c_address[obj->i2c.instance]);
}

/** Send STOP command *
 *  @param obj The I2C object
 */
int i2c_stop(i2c_t *obj)
{
    return I2C_Stop(i2c_address[obj->i2c.instance]);
}

int i2c_read(i2c_t *obj, int address, char *data, int length, int stop)
{
    return I2C_Read(i2c_address[obj->i2c.instance], address, data, length, stop);
}

int i2c_write(i2c_t *obj, int address, const char *data, int length, int stop)
{
    return I2C_Write(i2c_address[obj->i2c.instance], address, data, length, stop);
}

int i2c_byte_read(i2c_t *obj, int last)
{
    I2C_Read(i2c_address[obj->i2c.instance], slave_address, (char *)&i2c_reg_value, 1, last);

    return i2c_reg_value;
}

int i2c_byte_write(i2c_t *obj, int data){

    return I2C_ByteWrite(i2c_address[obj->i2c.instance], (uint8_t)data);

}

/**@}*/

#if DEVICE_I2C_ASYNCH

/**
 * \defgroup hal_AsynchI2C Asynchronous I2C Hardware Abstraction Layer
 * @{
 */
void i2c_transfer_asynch(i2c_t *obj, const void *tx, size_t tx_length, void *rx, size_t rx_length, uint32_t address, uint32_t stop, uint32_t handler, uint32_t event, DMAUsage hint)
{
    I2C_TransferBlocking(i2c_address[obj->i2c.instance], tx, tx_length, rx, rx_length, 8);
}

// TODO
uint32_t i2c_irq_handler_asynch(i2c_t *obj)
{
    return 0;
}

uint8_t i2c_active(i2c_t *obj){return 1;}

void i2c_abort_asynch(i2c_t *obj){}

#endif
/**@}*/

#endif

/** @}*/
