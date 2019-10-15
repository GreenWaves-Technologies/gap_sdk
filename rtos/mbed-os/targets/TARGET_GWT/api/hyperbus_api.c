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

#if DEVICE_HYPERBUS

#include "cmsis.h"
#include "pinmap.h"
#include "mbed_error.h"
#include "PeripheralPins.h"
#include "gap_hyperbus.h"
#include "hyperbus_api.h"

static HYPERBUS_Type *const hyperbus_address[] = HYPERBUS_BASE_PTRS;

/* HYPERBUS read return value */
static uint16_t HYPERBUS_WRITE_VALUE;
static uint32_t HYPERBUS_READ_VALUE;

void hyperbus_init(hyperbus_t *obj, PinName dq0, PinName dq1, PinName dq2, PinName dq3, PinName dq4, PinName dq5, PinName dq6, PinName dq7, PinName ck, PinName ckn, PinName rwds, PinName csn0, PinName csn1)
{
    uint32_t hyperbus_ck, hyperbus_ckn, hyperbus_rwds, hyperbus_csn0, hyperbus_csn1;
    uint32_t hyperbus_dq0, hyperbus_dq1, hyperbus_dq2, hyperbus_dq3;
    uint32_t hyperbus_dq4, hyperbus_dq5, hyperbus_dq6, hyperbus_dq7;

    // determine the HYPERBUS to use
    hyperbus_ck   = pinmap_peripheral(ck, PinMap_HYPERBUS_CLK);
    hyperbus_ckn  = pinmap_peripheral(ckn, PinMap_HYPERBUS_CLKN);
    hyperbus_rwds = pinmap_peripheral(rwds, PinMap_HYPERBUS_RWDS);
    hyperbus_csn0 = pinmap_peripheral(csn0, PinMap_HYPERBUS_CSN0);
    hyperbus_csn1 = pinmap_peripheral(csn1, PinMap_HYPERBUS_CSN1);
    hyperbus_dq0  = pinmap_peripheral(dq0, PinMap_HYPERBUS_DQ0);
    hyperbus_dq1  = pinmap_peripheral(dq1, PinMap_HYPERBUS_DQ1);
    hyperbus_dq2  = pinmap_peripheral(dq2, PinMap_HYPERBUS_DQ2);
    hyperbus_dq3  = pinmap_peripheral(dq3, PinMap_HYPERBUS_DQ3);
    hyperbus_dq4  = pinmap_peripheral(dq4, PinMap_HYPERBUS_DQ4);
    hyperbus_dq5  = pinmap_peripheral(dq5, PinMap_HYPERBUS_DQ5);
    hyperbus_dq6  = pinmap_peripheral(dq6, PinMap_HYPERBUS_DQ6);
    hyperbus_dq7  = pinmap_peripheral(dq7, PinMap_HYPERBUS_DQ7);

    // pin out the hyperbus pins
    pinmap_pinout(ck,   PinMap_HYPERBUS_CLK);
    pinmap_pinout(ckn,  PinMap_HYPERBUS_CLKN);
    pinmap_pinout(rwds, PinMap_HYPERBUS_RWDS);
    pinmap_pinout(csn0, PinMap_HYPERBUS_CSN0);
    pinmap_pinout(csn1, PinMap_HYPERBUS_CSN1);
    pinmap_pinout(dq0,  PinMap_HYPERBUS_DQ0);
    pinmap_pinout(dq1,  PinMap_HYPERBUS_DQ1);
    pinmap_pinout(dq2,  PinMap_HYPERBUS_DQ2);
    pinmap_pinout(dq3,  PinMap_HYPERBUS_DQ3);
    pinmap_pinout(dq4,  PinMap_HYPERBUS_DQ4);
    pinmap_pinout(dq5,  PinMap_HYPERBUS_DQ5);
    pinmap_pinout(dq6,  PinMap_HYPERBUS_DQ6);
    pinmap_pinout(dq7,  PinMap_HYPERBUS_DQ7);

    uint32_t hyperbus_data0 = pinmap_merge(hyperbus_dq0, hyperbus_dq1);
    uint32_t hyperbus_data1 = pinmap_merge(hyperbus_dq2, hyperbus_dq3);
    uint32_t hyperbus_data2 = pinmap_merge(hyperbus_dq4, hyperbus_dq5);
    uint32_t hyperbus_data3 = pinmap_merge(hyperbus_dq6, hyperbus_dq7);

    uint32_t hyperbus_data4 = pinmap_merge(hyperbus_data0, hyperbus_data1);
    uint32_t hyperbus_data5 = pinmap_merge(hyperbus_data2, hyperbus_data3);

    uint32_t hyperbus_data  = pinmap_merge(hyperbus_data4, hyperbus_data5);

    uint32_t hyperbus_cntl0  = pinmap_merge(hyperbus_ck, hyperbus_ckn);
    uint32_t hyperbus_cntl1  = pinmap_merge(hyperbus_csn0, hyperbus_csn1);
    uint32_t hyperbus_cntl2  = pinmap_merge(hyperbus_cntl0, hyperbus_cntl1);
    uint32_t hyperbus_cntl   = pinmap_merge(hyperbus_rwds, hyperbus_cntl2);

    obj->hyperbus.instance = pinmap_merge(hyperbus_data, hyperbus_cntl);
    MBED_ASSERT((int)obj->hyperbus.instance != NC);

    /* Set the transfer status to idle */
    obj->hyperbus.status = uHYPERBUS_Idle;

    hyperbus_config_t  config;

    HYPERBUS_GetDefaultConfig(&config);

    HYPERBUS_Init(hyperbus_address[obj->hyperbus.instance], &config, SystemCoreClock);
}

void hyperbus_free(hyperbus_t *obj)
{
    HYPERBUS_DeInit(hyperbus_address[obj->hyperbus.instance]);
}

void hyperbus_frequency(hyperbus_t *obj, int hz)
{
    #if !defined(__GAP8__)
    HYPERBUS_FrequencyConfig(hyperbus_address[obj->hyperbus.instance], hz, SystemCoreClock);
    #endif
}

#if defined(__GAP8__)
void hyperbus_set_max_length(hyperbus_t *obj, int max_length_en, int max_length, int rd_wr, char device)
{
    HYPERBUS_SetMaxLength(hyperbus_address[obj->hyperbus.instance], max_length_en, max_length, rd_wr, device);
}

void hyperbus_set_timing(hyperbus_t *obj, int cshi, int css, int csh, int latency, int rd_wr, char device)
{
    HYPERBUS_SetTiming(hyperbus_address[obj->hyperbus.instance], cshi, css, csh, latency, rd_wr, device);
}
#else
void hyperbus_set_device_type(hyperbus_t *obj, int device, int dt0, int dt1)
{
    HYPERBUS_SetDevice(hyperbus_address[obj->hyperbus.instance], device, dt0, dt1);
}

void hyperbus_set_timing(hyperbus_t *obj, int latency0, int latency1, int recovery, int rwds_delay, int latency_autocheck_en , int csm)
{
    HYPERBUS_SetTiming(hyperbus_address[obj->hyperbus.instance], latency0, latency1, recovery, rwds_delay, latency_autocheck_en, csm);
}
#endif

int  hyperbus_busy(hyperbus_t *obj)
{
    return obj->hyperbus.status;
}

uint8_t hyperbus_get_module(hyperbus_t *obj)
{
    return obj->hyperbus.instance;
}

void hyperbus_write(hyperbus_t *obj, const int addr, unsigned short value, char reg_access, char device) {
    HYPERBUS_WRITE_VALUE = value;
    hyperbus_block_write(obj, addr, &HYPERBUS_WRITE_VALUE, 2, reg_access, device);
}

int hyperbus_read(hyperbus_t *obj, const int addr, char reg_access, char device) {

    hyperbus_block_read(obj, addr, &HYPERBUS_READ_VALUE, 2, reg_access, device);

    #if defined(__GAP8__)
    return ((HYPERBUS_READ_VALUE >> 16) & 0xFFFF);
    #elif defined(__GAP9__)
    return (HYPERBUS_READ_VALUE & 0xFFFF);
    #endif
}

int hyperbus_block_write(hyperbus_t *obj, const int addr, const void *tx, size_t tx_length, char reg_access, char device) {
    hyperbus_transfer_t xfer;

    /*Start transfer*/
    xfer.txData = (uint16_t *)tx;
    xfer.txDataSize = tx_length;
    xfer.rxData = 0;
    xfer.rxDataSize = 0;
    xfer.configFlags = 32;
    xfer.addr = addr;
    xfer.device = device;
    xfer.reg_access = reg_access;

    HYPERBUS_TransferBlocking(hyperbus_address[obj->hyperbus.instance], &xfer);

    return tx_length;
}

int hyperbus_block_read(hyperbus_t *obj, const int addr, void *rx, size_t rx_length, char reg_access, char device) {
    hyperbus_transfer_t xfer;

    /*Start transfer*/
    xfer.txData = 0;
    xfer.txDataSize = 0;
    xfer.rxData = (uint16_t *)rx;
    xfer.rxDataSize = rx_length;
    xfer.configFlags = 32;
    xfer.addr = addr;
    xfer.device = device;
    xfer.reg_access = reg_access;

    HYPERBUS_TransferBlocking(hyperbus_address[obj->hyperbus.instance], &xfer);

    return rx_length;
}


#if DEVICE_HYPERBUS_ASYNCH
void hyperbus_transfer(hyperbus_t *obj, const int addr, const void *tx, size_t tx_length, void *rx, size_t rx_length, uint8_t bit_width, uint8_t device, uint32_t handler, uint32_t event, DMAUsage hint) {
    /* Asynchronious UDMA transfer */
    if(hyperbus_active(obj)) {
        return;
    }

    /* check corner case */
    if(tx_length == 0) {
        tx = (void*) 0;
    }

    if(rx_length == 0) {
        rx = (void*) 0;
    }

    /* Save handler */
    HYPERBUS_TransferCreateHandle(hyperbus_address[obj->hyperbus.instance],
                                        &obj->hyperbus.hyperbus_handle,
                                        (hyperbus_transfer_callback_t)handler, NULL);

    hyperbus_transfer_t xfer;

    /*Start transfer*/
    xfer.txData      = (uint16_t *)tx;
    xfer.txDataSize  = tx_length;
    xfer.rxData      = (uint16_t *)rx;
    xfer.rxDataSize  = rx_length;
    xfer.configFlags = 32;
    xfer.addr        = addr;
    xfer.device      = device;
    xfer.reg_access  = uHYPERBUS_Mem_Access;

    /* Busy transferring */
    if(tx_length && !(HYPERBUS_TXPending(hyperbus_address[obj->hyperbus.instance])))
        obj->hyperbus.status = uHYPERBUS_Idle;
    else if(rx_length && (!HYPERBUS_RXPending(hyperbus_address[obj->hyperbus.instance])))
        obj->hyperbus.status = uHYPERBUS_Idle;
    else
        obj->hyperbus.status = uHYPERBUS_Busy;

    if(HYPERBUS_TransferNonBlocking(hyperbus_address[obj->hyperbus.instance],
                                          &obj->hyperbus.hyperbus_handle,
                                          &xfer) != uStatus_Success)
    {
        obj->hyperbus.status = uHYPERBUS_Idle;
    }
}

uint32_t hyperbus_irq_handler_asynch(hyperbus_t *obj)
{
    obj->hyperbus.status = uHYPERBUS_Idle;

    return HYPERBUS_EVENT_COMPLETE;
}

void hyperbus_abort_asynch(hyperbus_t *obj)
{
    // If we're not currently transferring, then there's nothing to do here
    if(hyperbus_active(obj) == 0) {
        return;
    }
    HYPERBUS_ClearAndStop(hyperbus_address[obj->hyperbus.instance]);

    obj->hyperbus.status = uHYPERBUS_Idle;
}

uint8_t hyperbus_active(hyperbus_t *obj)
{
    return obj->hyperbus.status;
}

#endif // HYPERBUS_ASYNCH

#endif // HYPERBUS_DEVICE
