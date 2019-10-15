/******************************************************************************************
 *
 * Copyright (c) 2018 , GreenWaves Technologies, Inc.
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
 *******************************************************************************************/


/****************************************************************************/
/* Includes. */
#include "stdlib.h"
#include "gap_common.h"
#include "pinmap.h"
#include "PeripheralPins.h"

/****************************************************************************/
/* Error codes. */
#define SPI_FLASH_OK        0x0
#define SPI_FLASH_ERROR     0x1
#define SPI_FLASH_BUSY      0x2

/****************************************************************************/

/* Variables used. */
#define S25FS256_SR_WIP      ( ( uint8_t ) 0x00 )    /*!< Write in progress bit position */
#define DUMMY 15

/* Array of SPIM peripheral base address. */
static SPIM_Type *const spi_address[] = SPIM_BASE_PTRS;
/* SPI transfer command sequence array */
static uint32_t s_command_sequence[6];

spi_command_sequence_t s_command;
spi_master_config_t master_config;
static uint8_t syncPolling = 0;

/****************************************************************************/

/*!
 * @brief Exported functions.
 *
 * These functions are used to configure the SPI Flash and send commands( read,
 * write, erase ).
 */

/*!
 * @brief Initialize SPI Flash.
 *
 * @param mosi     SPI MOSI pin.
 * @param miso     SPI MISO pin.
 * @param sclk     SPI Clock pin.
 * @param ssel     SPI Select pin.
 * @param bits     Number of bits per frame.
 * @param mode     Phase and polarity configuration.
 * @param hz       SPI Frequency.
 * @param qspi     Enable QSPI mode.
 * @param polling  Synchronisation using polling mode or not.
 *
 * @return SPI_FLASH_OK if SPI Flash initialized, else SPI_FLASH_ERROR.
 */
int32_t SPI_FLASH_Init( PinName mosi, PinName miso, PinName sclk, PinName ssel,
                  uint32_t bits, uint32_t mode, uint32_t hz, uint8_t qspi, uint8_t polling );

/*!
 * @brief Free an instance of SPI.
 *
 */
void SPI_FLASH_Deinit( void );

/*!
 * @brief Erase elements of SPI Flash.
 *
 * @param addr   Address to start erasing.
 * @param size   Size of elements to erase, in Bytes.
 *
 * @return SPI_FLASH_OK if successfully erase, else SPI_FLASH_ERROR.
 */
int32_t SPI_FLASH_Erase( uint32_t addr, uint32_t size );

/*!
 * @brief Write a buffer to the SPI Flash.
 *
 * @param addr   Address to write in the flash.
 * @param size   Size of the buffer to write, in Bytes.
 * @param buf    Pointer to the buffer to output.
 *
 * @return SPI_FLASH_OK if successfully erase, else SPI_FLASH_ERROR.
 */
int32_t SPI_FLASH_Write( uint32_t addr, uint32_t size, void *buf );

/*!
 * @brief Read a buffer from the SPI Flash.
 *
 * @param addr   Address to read from the flash.
 * @param size   Size of the buffer to read, in Bytes.
 * @param buf    Pointer to the buffer to input.
 *
 * @return SPI_FLASH_OK if successfully erase, else SPI_FLASH_ERROR.
 */
int32_t SPI_FLASH_Read( uint32_t addr, uint32_t size, void *buf );

/*!
 * @brief Synchronise Page Programming/Erasing.
 *
 */
int32_t SPI_FLASH_Sync( void );
int32_t SPI_FLASH_Sync_Polling( void );

/****************************************************************************/
