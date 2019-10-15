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

#include "gap_i2c.h"

/*******************************************************************************
 * Definitions
 ******************************************************************************/
#define I2C_REPEAT_TRANSFER_MAX_LENGH 128

/*! @brief i2c transfer state. */
enum _i2c_transfer_states
{
    uI2C_Idle = 0x0U, /*!< Nothing in the transmitter/receiver. */
    uI2C_Busy,        /*!< Transfer queue is not finished. */
    uI2C_Error        /*!< Transfer error. */
};

/*! @brief Typedef for interrupt handler. */
typedef void (*i2c_isr_t)(I2C_Type *base, i2c_handle_t *handle);

/*******************************************************************************
 * Prototypes
 ******************************************************************************/
/*!
 * @brief Get instance number for I2C module.
 *
 * @param base I2C peripheral base address.
 */
uint32_t I2C_GetInstance(I2C_Type *base);


/*******************************************************************************
 * Variables
 ******************************************************************************/

/*! @brief Pointers to i2c bases for each instance. */
static I2C_Type *const s_i2cBases[] = I2C_BASE_PTRS;

/*! @brief Pointers to i2c handles for each instance. */
static void *s_i2cHandle[ARRAY_SIZE(s_i2cBases)];

/*! @brief Pointers to i2c IRQ number for each instance. */
/* static const IRQn_Type s_i2cIrqs[] = I2C_IRQS; */

/*! @brief Pointer to  IRQ handler for each instance. */
static i2c_isr_t s_i2cIsr;

/* I2C transfer command sequence array */
static uint8_t s_command_sequence[64];
/*******************************************************************************
 * Code
 ******************************************************************************/
uint32_t I2C_GetInstance(I2C_Type *base)
{
    uint32_t instance;

    /* Find the instance index from base address mappings. */
    for (instance = 0; instance < ARRAY_SIZE(s_i2cBases); instance++)
    {
        if (s_i2cBases[instance] == base)
        {
            break;
        }
    }

    assert(instance < ARRAY_SIZE(s_i2cBases));

    return instance;
}

void I2C_Init(I2C_Type *base, const i2c_config_t *Config, uint32_t srcClock_Hz)
{
    assert(Config);

    /* Enable I2C UDMA clock. */
    UDMA_Init((UDMA_Type *)base);

    /* Configure baud rate. */
    I2C_SetBaudRate(base, Config->baudRate_Bps, srcClock_Hz);
}

void I2C_Deinit(I2C_Type *base)
{
    /* Disable I2C UDMA clock. */
    UDMA_Deinit((UDMA_Type *)base);
}

void I2C_GetDefaultConfig(i2c_config_t *Config)
{
    assert(Config);
    /* Default baud rate at 100kbps. */
    Config->baudRate_Bps  = 100000;
}

void I2C_SetBaudRate(I2C_Type *base, uint32_t baudRate_Bps, uint32_t srcClock_Hz)
{
    assert(baudRate_Bps);

    int index = 0;
    uint32_t clkDivider = (srcClock_Hz >> 2) / baudRate_Bps;

    s_command_sequence[index++] = I2C_CMD_CFG;
    s_command_sequence[index++] = (clkDivider >> 8) & 0xFF;
    s_command_sequence[index++] = (clkDivider) & 0xFF;
    I2C_TransferBlocking(base, (const uint8_t*)&s_command_sequence, index, NULL, 0, 8);
}

int I2C_Start(I2C_Type *base)
{
    int index = 0;

    s_command_sequence[index++] = I2C_CMD_START;

    I2C_TransferBlocking(base,
                         (const uint8_t*)&s_command_sequence, index, NULL, 0, 8);

    return 0;
}

int I2C_Stop(I2C_Type *base)
{
    int index = 0;

    s_command_sequence[index++] = I2C_CMD_STOP;
    s_command_sequence[index++] = I2C_CMD_WAIT;
    s_command_sequence[index++] = 0xFF;

    I2C_TransferBlocking(base,
                         (const uint8_t*)&s_command_sequence, index, NULL, 0, 8);

    return 0;
}

int I2C_ByteWrite(I2C_Type *base, uint8_t data)
{
    int index = 0;

    s_command_sequence[index++] = I2C_CMD_WR;
    s_command_sequence[index++] = data;

    if(I2C_TransferBlocking(base,
                            (const uint8_t*)&s_command_sequence,
                            index, NULL, 0, 8) == uStatus_Success)
        return 1;
    else
        return -1;
}

int I2C_Write(I2C_Type *base, uint32_t address, const char *data, int length, int stop)
{
    status_t status = uStatus_Fail;
    int index = 0;
    int len = length;

    s_command_sequence[index++] = I2C_CMD_START;
    s_command_sequence[index++] = I2C_CMD_WR;
    s_command_sequence[index++] = address;

    while(len > 0) {
        int size  = (len > I2C_REPEAT_TRANSFER_MAX_LENGH) ? I2C_REPEAT_TRANSFER_MAX_LENGH : len;
        s_command_sequence[index++] = I2C_CMD_RPT;
        s_command_sequence[index++] = size;
        s_command_sequence[index++] = I2C_CMD_WR;

        /* Transfer sequence header with written data repeately (maximum I2C_REPEAT_TRANSFER_MAX_LENGH bytes) by udma */
        status = I2C_TransferBlocking(base,
                                      (const uint8_t*)&s_command_sequence,
                                      index, NULL, 0, 8);

        /* Transfer data section */
        status = I2C_TransferBlocking(base,
                                      data,
                                      size, NULL, 0, 8);

        /* Transfer stop signal */
        if ((len <= I2C_REPEAT_TRANSFER_MAX_LENGH) && stop) {
            index = 0;
            s_command_sequence[index++] = I2C_CMD_STOP;

            status = I2C_TransferBlocking(base,
                                          (const uint8_t*)&s_command_sequence,
                                          index, NULL, 0, 8);
        }

        index = 0;
        len  -= I2C_REPEAT_TRANSFER_MAX_LENGH;
        data += I2C_REPEAT_TRANSFER_MAX_LENGH;
    }

    if(status == uStatus_Success)
        return length;
    else
        return -1;

}

int I2C_Read(I2C_Type *base, uint32_t address, char *data, int length, int stop)
{

    status_t status = uStatus_Fail;
    int index = 0;
    int len = length;

    s_command_sequence[index++] = I2C_CMD_START;
    s_command_sequence[index++] = I2C_CMD_WR;
    s_command_sequence[index++] = address | 0x1;

    while(len > 0) {
        int size  = (len > I2C_REPEAT_TRANSFER_MAX_LENGH) ? I2C_REPEAT_TRANSFER_MAX_LENGH : len;

        s_command_sequence[index++] = I2C_CMD_RPT;
        s_command_sequence[index++] = size - 1;
        s_command_sequence[index++] = I2C_CMD_RD_ACK;
        s_command_sequence[index++] = I2C_CMD_RD_NACK;

        if ((size < I2C_REPEAT_TRANSFER_MAX_LENGH || len == I2C_REPEAT_TRANSFER_MAX_LENGH) && stop) {
            s_command_sequence[index++] = I2C_CMD_STOP;
            s_command_sequence[index++] = I2C_CMD_WAIT;
            s_command_sequence[index++] = 0xFF;
        }

        status = I2C_TransferBlocking(base,
                                      (const uint8_t*)&s_command_sequence,
                                      index, data, size, 8);
        index = 0;
        len  -= I2C_REPEAT_TRANSFER_MAX_LENGH;
        data += I2C_REPEAT_TRANSFER_MAX_LENGH;
    }

    if(status == uStatus_Success)
        return length;
    else
        return -1;
}

static status_t I2C_TransferStart(I2C_Type *base, i2c_transfer_t *transfer)
{
    int32_t status = 0;

    /*Start  transfer*/
    if (transfer->rxDataSize) {
        i2c_req_t *RX = UDMA_FindAvailableRequest();

        RX->info.dataAddr  = (uint32_t)  transfer->rxData;
        RX->info.dataSize  = transfer->rxDataSize;
        RX->info.isTx      = 0;
        RX->info.channelId = UDMA_EVENT_I2C0_RX + (I2C_GetInstance(base) << 1);
        RX->info.ctrl        = UDMA_CTRL_DUAL_RX;
        RX->info.configFlags = UDMA_CFG_DATA_SIZE((transfer->configFlags >> 4));
        RX->info.task        = 1;
        RX->info.repeat.size = 0;

        UDMA_SendRequest((UDMA_Type *)base, RX);
    }

    if (transfer->txDataSize) {
        i2c_req_t *TX = UDMA_FindAvailableRequest();

        TX->info.dataAddr = (uint32_t) transfer->txData;
        TX->info.dataSize  = transfer->txDataSize;
        TX->info.isTx      = 1;
        TX->info.channelId   = UDMA_EVENT_I2C0_TX + (I2C_GetInstance(base) << 1);
        TX->info.configFlags = UDMA_CFG_DATA_SIZE((transfer->configFlags >> 4));

        if(transfer->rxDataSize){
            TX->info.ctrl = UDMA_CTRL_DUAL_TX;
            TX->info.task = 0;
        } else {
            TX->info.ctrl = UDMA_CTRL_NORMAL;
            TX->info.task = 1;
        }
        TX->info.repeat.size      = 0;

        UDMA_SendRequest((UDMA_Type *)base, TX);
    }
    return status;
}

status_t I2C_TransferBlocking(I2C_Type *base, const void *tx_buffer, int tx_length, void *rx_buffer, int rx_length, unsigned char bit_width)
{
    status_t status = uStatus_Success;
    udma_req_info_t info;

    info.ctrl        = UDMA_CTRL_NORMAL;

    if (rx_length) {
        info.dataAddr    = (uint32_t) rx_buffer;
        info.dataSize    = (uint32_t) rx_length;
        info.configFlags = UDMA_CFG_DATA_SIZE((bit_width >> 4)) | UDMA_CFG_EN(1);
        info.isTx        = 0;

        status = UDMA_BlockTransfer((UDMA_Type *)base, &info, UDMA_NO_WAIT);
    }

    if (tx_length) {
        info.dataAddr    = (uint32_t) tx_buffer;
        info.dataSize    = (uint32_t) tx_length;
        info.configFlags = UDMA_CFG_DATA_SIZE((bit_width >> 4)) | UDMA_CFG_EN(1);
        info.isTx        = 1;

        if(rx_length)
            status = UDMA_BlockTransfer((UDMA_Type *)base, &info, UDMA_WAIT_RX);
        else
            status = UDMA_BlockTransfer((UDMA_Type *)base, &info, UDMA_WAIT);
    }

    return status;
}

status_t I2C_TransferNonBlocking(I2C_Type *base, i2c_handle_t *handle, i2c_transfer_t *transfer)
{
    assert(handle);

    /* Check that we're not busy.*/
    if (handle->state == uI2C_Busy) {
        return uStatus_I2C_Busy;
    }

    handle->state = uI2C_Busy;

    s_i2cIsr = I2C_TransferHandleIRQ;

    /*Start transfer*/
    I2C_TransferStart(base, transfer);

    return uStatus_Success;
}

void I2C_TransferCreateHandle(I2C_Type *base,
                              i2c_handle_t *handle,
                              i2c_transfer_callback_t callback,
                              void *userData)
{
    assert(handle);

    /* Zero the handle. */
    memset(handle, 0, sizeof(*handle));

    s_i2cHandle[I2C_GetInstance(base)] = handle;

    handle->callback = callback;
    handle->userData = userData;
}

void I2C_TransferHandleIRQ(I2C_Type *base, i2c_handle_t *handle)
{
    assert(handle);

    status_t status = 0;
    if (handle->state == uI2C_Error)
    {
        status = uStatus_I2C_Error;
    }
    else
    {
        status = uStatus_Success;
    }

    handle->state = uI2C_Idle;

    if (handle->callback)
    {
        handle->callback(base, handle, status, handle->userData);
    }
}

static void I2C_CommonIRQHandler(I2C_Type *base, void *param)
{
    s_i2cIsr(base, (i2c_handle_t *)param);
}

#if defined(I2C0)
void I2C0_DriverIRQHandler(void)
{
    assert(s_i2cHandle[0]);
    I2C_CommonIRQHandler(I2C0, s_i2cHandle[0]);
}
#endif


#if defined(I2C1)
void I2C1_DriverIRQHandler(void)
{
    assert(s_i2cHandle[1]);
    I2C_CommonIRQHandler(I2C1, s_i2cHandle[1]);
}
#endif
