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

#ifndef _GAP_SPI_H_
#define _GAP_SPI_H_

#include "gap_udma.h"
/*!
 * @addtogroup spi
 * @{
 */
/*******************************************************************************
 * Definitions
 ******************************************************************************/
/*! @brief Status for the SPI driver.*/
enum _spi_status
{
    uStatus_SPI_Busy = MAKE_STATUS(uStatusGroup_SPI, 0),      /*!< SPI transfer is busy.*/
    uStatus_SPI_Error = MAKE_STATUS(uStatusGroup_SPI, 1),     /*!< SPI driver error. */
    uStatus_SPI_Idle = MAKE_STATUS(uStatusGroup_SPI, 2),      /*!< SPI is idle.*/
    uStatus_SPI_OutOfRange = MAKE_STATUS(uStatusGroup_SPI, 3) /*!< SPI transfer out of range. */
};

/*! @brief SPI Chip Select (csn) configuration (which csn to configure).*/
typedef enum _spi_which_csn
{
    uSPI_csn0 = 0U, /*!< csn[0] */
    uSPI_csn1 = 1U  /*!< csn[1] */
} spi_which_csn_t;

/*! @brief SPI clock polarity configuration.*/
typedef enum _spi_clock_polarity
{
    uSPI_ClockPolarityActiveHigh = 0U, /*!< CPOL=0. Active-high SPI clock (idles low).*/
    uSPI_ClockPolarityActiveLow = 1U   /*!< CPOL=1. Active-low  SPI clock (idles high).*/
} spi_clock_polarity_t;

/*! @brief SPI clock phase configuration.*/
typedef enum _spi_clock_phase
{
    uSPI_ClockPhaseFirstEdge = 0U, /*!< CPHA=0. Data is captured on the leading edge
                                     of the SCLK and changed on the following edge.*/
    uSPI_ClockPhaseSecondEdge = 1U /*!< CPHA=1. Data is changed on the leading edge
                                     of the SCLK and captured on the following edge.*/
} spi_clock_phase_t;

/*! @brief SPI data shifter direction.*/
typedef enum _spi_shift_direction
{
    uSPI_MsbFirst = 0U, /*!< Data transfers start with most significant bit.*/
    uSPI_LsbFirst = 1U  /*!< Data transfers start with least significant bit.*/
} spi_shift_direction_t;


/*! @brief SPI transfer mode - two wire normal mode or QPI mode.*/
typedef enum _spi_qpi {
    uSPI_Single = 0, /*!< Singl-wire for TX and RX (MISO, MOSI).*/
    uSPI_Quad = 1    /*!< QPI mode 4-wire for TX and RX.*/
} spi_qpi_t;

/*! @brief SPI master configuration structure.*/
typedef struct _spi_master_config
{
    spi_which_csn_t whichCsn;/*!< The desired Chip Select (csn). */
    uint32_t  baudRate;/*!< The baud rate of Tx and Rx. */
    uint8_t   bitsPerFrame;/*!< The desired bits of unit, default = 8. */
    spi_clock_polarity_t   cpol;/*!< The polariry of clock. */
    spi_clock_phase_t   cpha;/*!< The phase of clock. */
    spi_shift_direction_t   direction;/*!< The shift direction of data. */
    spi_qpi_t   qpi;/*!< The desired SPI mode QPI or not. */
    uint16_t   clkDiv;/*!< The SPI master clock divider. */
} spi_master_config_t;

/*! @brief SPI transfer status.*/
enum _spi_transfer_state
{
    uSPI_Idle = 0x0U, /*!< Nothing in the transmitter/receiver. */
    uSPI_Busy,        /*!< Transfer queue is not finished. */
    uSPI_Error        /*!< Transfer error. */
};

/*! @brief SPI transfer request type definition.*/
#define  spi_req_t udma_req_t

/*! @brief SPI master/slave transfer structure.*/
typedef struct _spi_transfer
{
    uint8_t *txData;            /*!< Send buffer. */
    uint8_t *rxData;            /*!< Receive buffer. */
    volatile size_t txDataSize; /*!< TX Transfer bytes. */
    volatile size_t rxDataSize; /*!< RX Transfer bytes. */
    uint32_t configFlags;       /*!< Transfer transfer configuration flags.*/
} spi_transfer_t;

/*!
 * @brief Forward declaration of the _spi_master_handle typedefs.
 */
typedef struct _spi_master_handle spi_master_handle_t;

/*!
 * @brief Completion callback function pointer type.
 *
 * @param base SPI peripheral address.
 * @param handle Pointer to the handle for the SPI master.
 * @param status Success or error code describing whether the transfer completed.
 * @param userData Arbitrary pointer-dataSized value passed from the application.
 */
typedef void (*spi_master_transfer_callback_t)(SPIM_Type *base,
                                               spi_master_handle_t *handle,
                                               status_t status,
                                               void *userData);



/*! @brief SPI master transfer handle structure used for transactional API. */
struct _spi_master_handle
{
    uint8_t *volatile txData;                  /*!< Send buffer. */
    uint8_t *volatile rxData;                  /*!< Receive buffer. */
    volatile size_t remainingSendByteCount;    /*!< A number of bytes remaining to send.*/
    volatile size_t remainingReceiveByteCount; /*!< A number of bytes remaining to receive.*/
    size_t totalByteCount;                     /*!< A number of transfer bytes*/

    volatile uint8_t state; /*!< SPI transfer state, see _spi_transfer_state.*/

    spi_master_transfer_callback_t callback; /*!< Completion callback. */
    void *userData;                           /*!< Callback user data. */
};


/*! @brief SPI master auto polling structure.*/
typedef enum _spi_match_type {
    uSPI_Match_Equal      = 0, /*!< Compare the polling value by exactly equal.*/
    uSPI_Match_Bits_One   = 1, /*!< Compare the polling value by only the bits 1.*/
    uSPI_Match_Bits_Zero  = 2, /*!< Compare the polling value by only the bits 0.*/
} spi_match_type_t;

/*! @brief SPI master auto polling structure.*/
typedef struct _spi_polling_config {
    uint8_t  csn;           /*!< The chip select to device */
    uint8_t  cmd;           /*!< Specifies the Instruction4 */
    uint8_t  cmd_mode;      /*!< Specifies the Instruction Mode 1 or 4 */
    uint8_t  match_type;    /*!< Specifies the match type */
    uint8_t  match_bits;    /*!< Specifies the match bits, up to 16 bits */
    uint16_t match_value;   /*!< Specifies the match value, up to 16 bits */
    uint8_t  data_mode;     /*!< Specifies the read match data Mode 1 or 4 */
    uint16_t time_out;      /*!< Specifies the polling times */
} spi_polling_config_t;

/*! @brief SPI master request configuration structure.*/
typedef struct _spi_req_config {
    uint32_t cfg;
} spi_req_config_t;

/*! @brief SPI master request command structure.*/
typedef struct _spi_req_cmd {
    uint32_t sot;
    uint32_t sendCmd;
    uint32_t eot;
} spi_req_cmd_t;

/*! @brief SPI master request register read structure.*/
typedef struct _spi_req_reg_rx {
    uint32_t sot;
    uint32_t sendCmd;
    uint32_t dummy;
    uint32_t rxData;
    uint32_t eot;
} spi_req_reg_rx_t;

/*! @brief SPI master request memory read structure.*/
typedef struct _spi_req_mem_rx {
    uint32_t sot;
    uint32_t sendCmd;
    uint32_t sendAddr;
    uint32_t addr;
    uint32_t sendMode;
    uint32_t dummy;
    uint32_t rxData;
    uint32_t eot;
} spi_req_mem_rx_t;

/*! @brief SPI master request register write structure.*/
typedef struct _spi_req_reg_tx {
    uint32_t sot;
    uint32_t sendCmd;
    uint32_t sendAddr;
    uint32_t addr;
    uint32_t eot;
} spi_req_reg_tx_t;

/*! @brief SPI master request memory write structure.*/
typedef struct _spi_req_mem_tx {
    uint32_t sot;
    uint32_t sendCmd;
    uint32_t sendAddr;
    uint32_t addr;
    uint32_t txData;
    uint32_t Data;
    uint32_t eot;
} spi_req_mem_tx_t;

/*! @brief SPI master request create sequence structure.*/
typedef struct _spi_command_sequence {
    uint8_t  csn;           /*!< The chip select to device */
    uint16_t cmd;           /*!< The command send to device */
    uint8_t  cmd_bits;      /*!< The bits of cmd send to device */
    uint8_t  cmd_mode;      /*!< Specifies the Instruction Mode 1 or 4 */
    uint8_t  addr_bits;     /*!< The bits of address send to device */
    uint32_t addr;          /*!< If addr_bits is not 0, the address sent device*/
    uint8_t  addr_mode;     /*!< Specifies the Address Mode 1 or 4 */
    uint32_t tx_bits;       /*!< The bits of data send to  device*/
    uint32_t tx_data;       /*!< If tx_bits is not 0, the data written to device*/
    uint8_t *tx_buffer;     /*!< If tx_bits is more than a word, the data buffer pointer written to device*/
    uint32_t rx_bits;       /*!< The bits of data read from device*/
    uint8_t  full_duplex;   /*!< Single SPI full duplex enable */
    uint8_t  alter_data;       /*!< Specifies the alternative data Mode (used for dummy cycles and data phases)*/
    uint8_t  alter_data_bits;  /*!< Specifies the alternative data bits */
    uint8_t  alter_data_mode;  /*!< Specifies the alternative data Mode 1 or 4 */
    uint8_t  dummy;         /*!< Specifies the Number of Dummy Cycles */
    uint8_t  data_mode;     /*!< Specifies the Date Mode 1 or 4 */
    uint8_t *rx_buffer;    /*!< If rx_bits is not 0, pointer of receive buffer*/
} spi_command_sequence_t;

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
 * @brief Initializes the SPI master.
 *
 * This function initializes the SPI master configuration. This is an example use case.
 *  @code
 *   spi_master_config_t  masterConfig;
 *   masterConfig.whichCsn                                 = uSPI_csn0;
 *   masterConfig.ctarConfig.baudRate                      = 500000000U;
 *   masterConfig.ctarConfig.bitsPerFrame                  = 8;
 *   masterConfig.ctarConfig.cpol                          = uSPI_ClockPolarityActiveHigh;
 *   masterConfig.ctarConfig.cpha                          = uSPI_ClockPhaseFirstEdge;
 *   masterConfig.ctarConfig.direction                     = uSPI_MsbFirst;
 *   masterConfig.qpi                                      = uSPI_Single;
 *   SPI_MasterInit(base, &masterConfig, srcClock_Hz);
 *  @endcode
 *
 * @param base SPI peripheral address.
 * @param masterConfig Pointer to the structure spi_master_config_t.
 * @param srcClock_Hz Module source input clock in Hertz.
 */
void SPI_MasterInit(SPIM_Type *base, spi_master_config_t *masterConfig, uint32_t srcClock_Hz);

/*!
 * @brief Sets the spi_master_config_t structure to default values.
 *
 * The purpose of this API is to get the configuration structure initialized for the SPI_MasterInit().
 * Users may use the initialized structure unchanged in the SPI_MasterInit() or modify the structure
 * before calling the SPI_MasterInit().
 * Example:
 * @code
 *  spi_master_config_t  masterConfig;
 *  SPI_MasterGetDefaultConfig(&masterConfig);
 * @endcode
 * @param masterConfig pointer to spi_master_config_t structure
 */
void SPI_MasterGetDefaultConfig(spi_master_config_t *masterConfig);

/*!
 * @brief Sets the spi frequecny.
 *
 * Example:
 * @code
 *  spi_master_config_t  masterConfig;
 *  SPI_SPI_MasterFrequencyConfig(&masterConfig);
 * @endcode
 * @param base SPI peripheral address.
 * @param masterConfig pointer to spi_master_config_t structure
 */
void SPI_MasterFrequencyConfig(SPIM_Type *base, spi_master_config_t *masterConfig);

/*!
 * @brief De-initializes the SPI peripheral, Clock Gating. Call this API to disable the SPI clock.
 * @param base SPI peripheral address.
 */
void SPI_MasterDeInit(SPIM_Type *base);

/*!
 *@}
*/

/*!
 * @name Transactional
 * @{
 */
/*Transactional APIs*/

/*!
 * @brief This function set SPI chip select.
 *
 * @param base SPI peripheral address.
 * @param whichCsn which Chip select.
 * @param status   Chip select status.
 */
void SPI_Master_CS(SPIM_Type *base, int whichCsn, int status);

/*!
 * @brief This function does duplex write and read for 32bits or less.
 *
 * @param base SPI peripheral address.
 * @param tx_value  write value 32 bit or less.
 * @param rx_buffer Udma rx buffer base address.
 * @param bits      Udma tx rx bits, 32 bits or less.
 */
void SPI_Master_DulpexTransfer(SPIM_Type *base, int tx_value, void *rx_buffer, int bits);

/*!
 * @brief This function does a read polling in SPI master transfer.
 *
 * This function use polling the read value to compare with given value .
 *
 * @param base SPI peripheral address.
 * @param conf Pointer to the structure spi_polling_config_t.
 */
int SPI_Master_AutoPolling(SPIM_Type *base, spi_polling_config_t *conf);

/*!
 * @brief Use command sequence to configure SPI master transfer.
 *
 * This function use command sequence to do SPI master transfer. This is an example use case.
 *  @code
 *   spi_command_sequence_t  s_command;
 *   s_command.cmd       = 0x71;
 *   s_command.addr_bits = 32;
 *   s_command.addr      = 0x00800003;
 *   s_command.cmd_mode  = uSPI_Single;
 *   s_command.addr_mode = uSPI_Single;
 *   s_command.tx_bits   = 8;
 *   s_command.tx_data   = (0xC0 | DUMMY);
 *   s_command.data_mode = uSPI_Single;
 *   SPI_MasterTransferCommandSequence(base, &s_command);
 *  @endcode
 *
 * @param base SPI peripheral address.
 * @param seq  Pointer to the structure spi_command_sequence_t.
 */
int SPI_MasterTransferCommandSequence(SPIM_Type *base, spi_command_sequence_t* seq);

/*!
 * @brief Writes data into the data buffer master mode and waits till complete to return.
 *
 * In master mode, the 16-bit data is appended to the 16-bit command info. The command portion
 *
 * @param base SPI peripheral address.
 * @param tx_buffer Udma tx buffer base address.
 * @param tx_length Udma tx buffer size.
 * @param rx_buffer Udma rx buffer base address.
 * @param rx_length Udma rx buffer size.
 * @param bit_width Udma transfer bit width.
 */
void SPI_MasterTransferBlocking(SPIM_Type *base, const void *tx_buffer, int tx_length, void *rx_buffer, int rx_length, unsigned char bit_width);


/*!
 * @brief SPI master transfer data using interrupts.
 *
 * This function transfers data using interrupts. This is a non-blocking function, which returns right away. When all
 * data is transferred, the callback function is called.

 * @param base SPI peripheral base address.
 * @param handle Pointer to the spi_master_handle_t structure which stores the transfer state.
 * @param transfer Pointer to the spi_transfer_t structure.
 * @return status of status_t.
 */
status_t SPI_MasterTransferNonBlocking(SPIM_Type *base, spi_master_handle_t *handle, spi_transfer_t *transfer);

/*!
 * @brief SPI Master IRQ handler function.
 *
 * This function processes the SPI transmit and receive IRQ.

 * @param base SPI peripheral base address.
 * @param handle Pointer to the spi_master_handle_t structure which stores the transfer state.
 */
void SPI_MasterTransferHandleIRQ(SPIM_Type *base, spi_master_handle_t *handle);


/*!
 * @brief Initializes the SPI master handle.
 *
 * This function initializes the SPI handle, which can be used for other SPI transactional APIs.  Usually, for a
 * specified SPI instance,  call this API once to get the initialized handle.
 *
 * @param base SPI peripheral base address.
 * @param handle SPI handle pointer to spi_master_handle_t.
 * @param callback SPI callback.
 * @param userData Callback function parameter.
 */
void SPI_MasterTransferCreateHandle(SPIM_Type *base,
                                     spi_master_handle_t *handle,
                                     spi_master_transfer_callback_t callback,
                                     void *userData);

/*!
 *@}
*/

#if defined(__cplusplus)
}
#endif /* __cplusplus */

/* @} */

#endif /*_GAP_SPI_H_*/
