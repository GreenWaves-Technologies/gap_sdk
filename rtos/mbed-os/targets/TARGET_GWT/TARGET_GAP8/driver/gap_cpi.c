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

#include "gap_cpi.h"

/*******************************************************************************
 * Definitions
 ******************************************************************************/
typedef void (*cpi_isr_t)(CPI_Type *base, cpi_handle_t *handle);

typedef struct {
    PinName pin;
    int peripheral;
    int function;
} PinMap;

extern const PinMap PinMap_CPI_PCLK[];
extern const PinMap PinMap_CPI_HSYNC[];
extern const PinMap PinMap_CPI_VSYNC[];
extern const PinMap PinMap_CPI_DATA0[];
extern const PinMap PinMap_CPI_DATA1[];
extern const PinMap PinMap_CPI_DATA2[];
extern const PinMap PinMap_CPI_DATA3[];
extern const PinMap PinMap_CPI_DATA4[];
extern const PinMap PinMap_CPI_DATA5[];
extern const PinMap PinMap_CPI_DATA6[];
extern const PinMap PinMap_CPI_DATA7[];

/*******************************************************************************
 * Prototypes
 ******************************************************************************/
/*!
 * @brief Get instance number for CPI module.
 *
 * @param base CPI peripheral base address.
 */
uint32_t CPI_GetInstance(CPI_Type *base);

/*******************************************************************************
 * Variables
 ******************************************************************************/

/*! @brief Pointers to cpi bases for each instance. */
static CPI_Type *const s_cpiBases[] = CPI_BASE_PTRS;

/*! @brief Pointers to cpi handles for each instance. */
static void *s_cpiHandle[ARRAY_SIZE(s_cpiBases)];

/*! @brief Pointer to master IRQ handler for each instance. */
static cpi_isr_t s_cpiMasterIsr;

/* Array of GPIO peripheral base address. */
static PORT_Type *const port_addrs[] = PORT_BASE_PTRS;

/*******************************************************************************
 * Code
 ******************************************************************************/
static void pin_function(PinName pin, int function)
{
    int pin_num = (pin & 0xFF) - GAP_PIN_OFFSET;

    if (0 <= pin_num && pin_num < GAP_PORT_PIN_NUM )
        PORT_SetPinMux(port_addrs[GET_GPIO_PORT(pin)], pin_num, (port_mux_t)function);
}

static void pinmap_pinout(PinName pin, const PinMap *map) {
    if (pin == NC)
        return;

    while (map->pin != NC) {
        if (map->pin == pin) {
            pin_function(pin, map->function);

            return;
        }
        map++;
    }
}

uint32_t CPI_GetInstance(CPI_Type *base)
{
    uint32_t instance;

    /* Find the instance index from base address mappings. */
    for (instance = 0; instance < ARRAY_SIZE(s_cpiBases); instance++)
    {
        if (s_cpiBases[instance] == base)
        {
            break;
        }
    }

    assert(instance < ARRAY_SIZE(s_cpiBases));

    return instance;
}

void CPI_Init(CPI_Type *base, PinName pclk, PinName hsync, PinName vsync,
              PinName data0, PinName data1, PinName data2, PinName data3,
              PinName data4, PinName data5, PinName data6, PinName data7)
{
    pinmap_pinout(pclk,  PinMap_CPI_PCLK);
    pinmap_pinout(hsync, PinMap_CPI_HSYNC);
    pinmap_pinout(vsync, PinMap_CPI_VSYNC);
    pinmap_pinout(data0, PinMap_CPI_DATA0);
    pinmap_pinout(data1, PinMap_CPI_DATA1);
    pinmap_pinout(data2, PinMap_CPI_DATA2);
    pinmap_pinout(data3, PinMap_CPI_DATA3);
    pinmap_pinout(data4, PinMap_CPI_DATA4);
    pinmap_pinout(data5, PinMap_CPI_DATA5);
    pinmap_pinout(data6, PinMap_CPI_DATA6);
    pinmap_pinout(data7, PinMap_CPI_DATA7);

    /* UDMA CPI device on */
    UDMA_Init((UDMA_Type *)base);
}

void CPI_Deinit(CPI_Type *base)
{
    CPI_Disable(base);

    /* UDMA CPI device off */
    UDMA_Deinit((UDMA_Type *)base);
}

void CPI_GetDefaultConfig(cpi_config_t *masterConfig)
{
    assert(masterConfig);
    masterConfig->resolution = QVGA;
    masterConfig->row_len = 320;
    masterConfig->format = BYPASS_BIGEND;
    masterConfig->fps = 30;
    masterConfig->slice_en = 0;
    masterConfig->shift = 0;
    masterConfig->frameDrop_en = 0;
    masterConfig->frameDrop_value = 0;
}

static status_t CPI_ReceptionStart(CPI_Type *base, cpi_transfer_t *transfer) {
    cpi_req_t *RX = UDMA_FindAvailableRequest();

    RX->info.dataAddr = (uint32_t) transfer->data;
    RX->info.dataSize  = transfer->dataSize;
    RX->info.isTx      = 0;
    RX->info.channelId   = UDMA_EVENT_CPI_RX;
    RX->info.configFlags = transfer->configFlags;
    RX->info.ctrl = UDMA_CTRL_NORMAL;

    RX->info.task = 1;
    RX->info.repeat.size = 0;

    UDMA_SendRequest((UDMA_Type *)base, RX);

    return uStatus_CPI_Busy;
}


status_t CPI_ReceptionBlocking(CPI_Type *base, cpi_transfer_t *transfer)
{
    udma_req_info_t info;

    info.dataAddr    = (uint32_t) transfer->data;
    info.dataSize    = (uint32_t) transfer->dataSize;
    info.configFlags = transfer->configFlags | UDMA_CFG_EN(1);
    info.isTx        = 0;

    UDMA_BlockTransfer((UDMA_Type *)base, &info, UDMA_WAIT);

    return uStatus_Success;
}

status_t CPI_ReceptionNonBlocking(CPI_Type *base, cpi_handle_t *handle, cpi_transfer_t * transfer)
{
    assert(handle);

    if(handle->state == uStatus_CPI_Busy) {
        return uStatus_CPI_Busy;
    }

    handle->state = uStatus_CPI_Busy;

    s_cpiMasterIsr = CPI_ReceptionHandleIRQ;

    CPI_ReceptionStart(base, transfer);

    return handle->state;

}

void CPI_ReceptionCreateHandle(CPI_Type *base, cpi_handle_t *handle, cpi_transfer_callback_t callback, void *userData)
{
    assert(handle);

    /* Zero the handle. */
    memset(handle, 0, sizeof(*handle));

    s_cpiHandle[CPI_GetInstance(base)] = handle;

    handle->callback = callback;
    handle->userData = userData;
}

void CPI_ReceptionHandleIRQ(CPI_Type *base, cpi_handle_t *handle)
{
    status_t status = 0;
    if (handle->state == uStatus_CPI_Error)
    {
        status = uStatus_CPI_Error;
    }
    else
    {
        status = uStatus_CPI_Idle;
    }

    handle->state = status;

    if (handle->callback)
    {
        handle->callback(base, handle, status, handle->userData);
    }
}

void CPI_DriverIRQHandler(void)
{
    assert(s_cpiHandle[0]);
    s_cpiMasterIsr(CPI, (cpi_handle_t *)s_cpiHandle[0]);
}
