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

#ifndef _GAP_I2C_H_
#define _GAP_I2C_H_

#include "gap_udma.h"

/*!
 * @addtogroup i2c_driver
 * @{
 */
/*******************************************************************************
 * Definitions
 ******************************************************************************/

/*! @brief  I2C status return codes. */
enum _i2c_status
{
    uStatus_I2C_Busy = MAKE_STATUS(uStatusGroup_I2C, 0),            /*!< I2C is busy with current transfer. */
    uStatus_I2C_Idle = MAKE_STATUS(uStatusGroup_I2C, 1),            /*!< Bus is Idle. */
    uStatus_I2C_Error = MAKE_STATUS(uStatusGroup_I2C, 2)            /*!< Error received during transfer. */
};

/*! @brief I2C  configuration structure.*/
typedef struct _i2c_config
{
    uint32_t baudRate_Bps;     /*!< Baud rate configuration of I2C peripheral. */
} i2c_config_t;

/*! @brief I2C transfer request type definition.*/
#define  i2c_req_t udma_req_t

/*! @brief I2C  handle typedef. */
typedef struct _i2c_handle i2c_handle_t;

/*! @brief I2C  transfer callback typedef. */
typedef void (*i2c_transfer_callback_t)(I2C_Type *base,
                                               i2c_handle_t *handle,
                                               status_t status,
                                               void *userData);

/*! @brief I2C  transfer structure. */
typedef struct _i2c_transfer
{
    uint8_t *txData;          /*!< Send buffer. */
    uint8_t *rxData;          /*!< Receive buffer. */
    volatile size_t txDataSize; /*!< TX Transfer bytes. */
    volatile size_t rxDataSize; /*!< RX Transfer bytes. */

    uint32_t configFlags; /*!< Transfer transfer configuration flags; is used for slave.*/
} i2c_transfer_t;


/*! @brief I2C  handle structure. */
struct _i2c_handle
{
    i2c_transfer_t transfer;                    /*!< I2C  transfer copy. */
    size_t transferSize;                               /*!< Total bytes to be transferred. */
    uint8_t state;                                     /*!< A transfer state maintained during transfer. */
    i2c_transfer_callback_t callback; /*!< A callback function called when the transfer is finished. */
    void *userData;                                    /*!< A callback parameter passed to the callback function. */
};

/*******************************************************************************
 * APIs
 ******************************************************************************/

#if defined(__cplusplus)
extern "C" {
#endif /* __cplusplus */

/*!
 * @name Initialization and deinitialization
 * @{
 */
/*!
 * @brief Initializes the I2C .
 *
 * This function initializes the I2C configuration. This is an example use case.
 *  @code
 *   i2c_config_t  Config;
 *   Config.baudRate_Bps = 100000;
 *   I2C_Init(base, &Config, srcClock_Hz);
 *  @endcode
 *
 * @param base I2C peripheral address.
 * @param Config Pointer to the structure i2c_config_t.
 * @param srcClock_Hz Module source input clock in Hertz.
 */
void I2C_Init(I2C_Type *base, const i2c_config_t *Config, uint32_t srcClock_Hz);

/*!
 * @brief De-initializes the I2C peripheral. Call this API to gate the I2C clock.
 * The I2C module can't work unless the I2C_Init is called.
 * @param base I2C base pointer
 */
void I2C_Deinit(I2C_Type *base);

/*!
 * @brief  Sets the I2C configuration structure to default values.
 *
 * The purpose of this API is to get the configuration structure initialized for use in the I2C_Configure().
 * Use the initialized structure unchanged in the I2C_Configure() or modify
 * the structure before calling the I2C_Configure().
 * This is an example.
 * @code
 * i2c_config_t config;
 * I2C_GetDefaultConfig(&config);
 * @endcode
 * @param Config A pointer to the  configuration structure.
*/
void I2C_GetDefaultConfig(i2c_config_t *Config);


/* @} */

/*!
 * @name Bus Operations
 * @{
 */

/*!
 * @brief Sets the I2C transfer baud rate.
 *
 * @param base I2C base pointer
 * @param baudRate_Bps the baud rate value in bps
 * @param srcClock_Hz Source clock
 */
void I2C_SetBaudRate(I2C_Type *base, uint32_t baudRate_Bps, uint32_t srcClock_Hz);


/*!
 * @brief Generate a start signal.
 *
 * @param base I2C base pointer
 */
int I2C_Start(I2C_Type *base);

/*!
 * @brief Generate a stop signal.
 *
 * @param base I2C base pointer
 */
int I2C_Stop(I2C_Type *base);

/*!
 * @brief Write one 8 bits data into I2C slave.
 *
 * @param base I2C base pointer
 * @param data 8 bits data want to write
 */
int I2C_ByteWrite(I2C_Type *base, uint8_t data);

/*!
 * @brief Write datas into I2C slave.
 *
 *
 * @param base I2C base pointer
 * @param address Target address
 * @param data The pointer to write buffer
 * @param length The length of read buffer
 * @param stop Send a stop in the end or not
 */
int I2C_Write(I2C_Type *base, uint32_t address, const char *data, int length, int stop);

/*!
 * @brief Read datas from I2C slave
 *
 *
 * @param base I2C base pointer
 * @param address Target address
 * @param data The pointer to read buffer
 * @param length The length of read buffer
 * @param stop Send a stop in the end or not
 */
int I2C_Read(I2C_Type *base, uint32_t address, char *data, int length, int stop);

/*!
 * @brief Performs a transfer on the I2C bus.
 *
 * @note The API does not return until the transfer succeeds or fails due
 * to arbitration lost or receiving a NAK.
 *
 * @param base I2C peripheral base address.
 * @param tx_buffer Udma tx buffer base address.
 * @param tx_length Udma tx buffer size.
 * @param rx_buffer Udma rx buffer base address.
 * @param rx_length Udma rx buffer size.
 * @param bit_width Udma transfer bit width.
 */
status_t I2C_TransferBlocking(I2C_Type *base, const void *tx_buffer, int tx_length, void *rx_buffer, int rx_length, unsigned char bit_width);


/*!
 * @brief Performs a interrupt non-blocking transfer on the I2C bus.
 *
 * @note Calling the API returns immediately after transfer initiates. When all
 * data is transferred, the callback function is called.
 *
 * @param base I2C base pointer.
 * @param handle pointer to i2c_handle_t structure which stores the transfer state.
 * @param transfer pointer to i2c_transfer_t structure.
 * @retval uStatus_Success Successfully start the data transmission.
 * @retval uStatus_I2C_Busy Previous transmission still not finished.
 * @retval uStatus_I2C_Error Transfer error, wait signal timeout.
 */
status_t I2C_TransferNonBlocking(I2C_Type *base, i2c_handle_t *handle, i2c_transfer_t *transfer);

/*!
 * @brief  interrupt handler.
 *
 * @param base I2C base pointer.
 * @param handle pointer to i2c_handle_t structure.
 */
void I2C_TransferHandleIRQ(I2C_Type *base, i2c_handle_t *handle);

/*!
 * @brief Initializes the I2C handle which is used in transactional functions.
 *
 * @param base I2C base pointer.
 * @param handle pointer to i2c_handle_t structure to store the transfer state.
 * @param callback pointer to user callback function.
 * @param userData user parameter passed to the callback function.
 */
void I2C_TransferCreateHandle(I2C_Type *base,
                                    i2c_handle_t *handle,
                                    i2c_transfer_callback_t callback,
                                    void *userData);
/* @} */

#if defined(__cplusplus)
}
#endif /* __cplusplus */

/* @} */

#endif /*_GAP_I2C_H_*/
