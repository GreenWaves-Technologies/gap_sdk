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

#ifndef __GAP_HYPERBUS_IO_H__
#define __GAP_HYPERBUS_IO_H__

#include "stdlib.h"
#include "stdarg.h"
#include "gap_common.h"
#include "pinmap.h"
#include "PeripheralPins.h"
#include "gap_malloc.h"

/*
 * HYPERFLASH S26KL256S
 *
 * ERASE SECTOR = 256kB
 * NB_SECTOR = TOTAL / 256kB = 128
 *
 * ADDRESS :
 * 0x00000000 - 0x0001FFFF
 * ----
 * 0xFE000000 - 0x00FFFFFF
 *
 * Line Buffer  = 512B
 * NB_LINE/SECTOR = 256kB / 512B = 512
 *
 * Half-Page    = 16B
 * NB_HALF/SECTOR = 256kB / 16B / 2 = 8192
 *
 * 1 word = 2B
 */

/*!
 * @addtogroup hyperbus_IO
 * @{
 */

/*******************************************************************************
 * Variables, macros, structures,... definitions
 ******************************************************************************/

/* Error codes. */
#define HYPERBUS_IO_OK        0x0
#define HYPERBUS_IO_ERROR     0x1
#define HYPERBUS_IO_BUSY      0x2

/*! @brief HyperFlash command sequence structure. */
typedef struct
{
    uint16_t data;              /*!< Command data. */
    uint16_t addr;              /*!< Commad address. */
} cmdSeq;

/*******************************************************************************
 * APIs
 ******************************************************************************/
#if defined(__cplusplus)
extern "C" {
#endif /* __cplusplus */

/*!
 * @brief Initialize Hyperbus used to access HyperFlash or HyperRam.
 *
 * @param device  Type of device to configure : uHYPERBUS_Flash or uHYPERBUS_Ram.
 *
 * @return HYPERBUS_IO_OK if Huperbus initialized, else HYPERBUS_IO_ERROR.
 */
int32_t HYPERBUS_IO_Init( uint8_t device );

/*!
 * @brief Free Hyperbus instance.
 *
 */
void HYPERBUS_IO_Deinit( void );

/*!
 * @brief Erase elements of HyperFlash.
 *
 * @param addr   Address to start erasing.
 *
 * @return HYPERBUS_IO_OK if successfully erase, else HYPERBUS_IO_ERROR.
 */
int32_t HYPERBUS_IO_Erase( uint32_t addr );

/*!
 * @brief Write a buffer to the HYPERBUS Flash.
 *
 * @param addr           Address to write in the device.
 * @param size           Size of the buffer to write, in Bytes.
 * @param buf            Pointer to the buffer to output.
 * @param device         Device accessed.
 * @param device_access  Type of access, register or memory.
 *
 * @return HYPERBUS_IO_OK if successfully erase, else HYPERBUS_IO_ERROR.
 */
int32_t HYPERBUS_IO_Write( uint32_t addr, uint32_t size, void *buf, uint8_t device, uint8_t device_access );

/*!
 * @brief Read a buffer to the HYPERBUS Flash.
 *
 * @param addr           Address to read from the device.
 * @param size           Size of the buffer to read, in Bytes.
 * @param buf            Pointer to the buffer to input.
 * @param device         Device accessed.
 * @param device_access  Type of access, register or memory.
 *
 * @return HYPERBUS_IO_OK if successfully erase, else HYPERBUS_IO_ERROR.
 */
int32_t HYPERBUS_IO_Read( uint32_t addr, uint32_t size, void *buf, uint8_t device, uint8_t device_access );

/*!
 * @brief Synchronise Page Programming/Erasing.
 *
 */
int32_t HYPERBUS_IO_Sync( void );

/*!
 * @brief Initialize external memory allocation in HyperRam.
 *
 * @param addr           Base address for the memory allocator.
 * @param size           Size of the memory allocator.
 *
 * @return uStatus_Success if a memory allocator is initialized, uStatus_Fail otherwise.
 */
uint32_t HYPERBUS_IO_Malloc_Init( void *addr, uint32_t size );

/*! @brief Free the memory bloc usedby the memory allocator. */
void HYPERBUS_IO_Malloc_Deinit();

/*!
 * @brief Allocate memory from external memory HyperRam.
 *
 * @param size           Size of the memory to allocate.
 *
 * @return Pointer to the allocated memory chunk if memory has been allocated else NULL.
 */
void *HYPERBUS_IO_Malloc( uint32_t size );

/*!
 * @brief Free an allocated memory chunk of external memory HyperRam.
 *
 * This function needs the pointer to allocated memory chunk and its size
 * in order to free the memory chunk.
 *
 * @param chunk          Pointer to the allocated memory chunk to free.
 * @param size           Size of the chunk.
 *
 * @return uStatus_Success if the memory chunk has been freed, uStatus_Fail otherwise.
 */
uint32_t HYPERBUS_IO_MallocFree( void *chunk, uint32_t size );

#if defined(__cplusplus)
}
#endif /* __cplusplus */

/*! @} */

#endif /* __GAP_HYPERBUS_IO_H__ */
