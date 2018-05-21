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

/* Define to prevent recursive inclusion -------------------------------------*/
#ifndef __GAPUINO_S25FS_QSPI_H
#define __GAPUINO_S25FS_QSPI_H

#ifdef __cplusplus
 extern "C" {
#endif

/* Includes ------------------------------------------------------------------*/
#include "gap_common.h"
#include "PinNames.h"
#include "../Components/s25fs256s/s25fs256s.h"

/** @addtogroup BSP
  * @{
  */

/** @addtogroup GAPUINO
  * @{
  */

/** @addtogroup GAPUINO_S25FS_QSPI
  * @{
  */

/* Exported constants --------------------------------------------------------*/
/** @defgroup GAPUINO_S25FS_QSPI_Exported_Constants QSPI Exported Constants
  * @{
  */
/* QSPI Error codes */
#define QSPI_OK            ((uint8_t)0x00)
#define QSPI_ERROR         ((uint8_t)0x01)
#define QSPI_BUSY          ((uint8_t)0x02)
#define QSPI_NOT_SUPPORTED ((uint8_t)0x04)
#define QSPI_SUSPENDED     ((uint8_t)0x08)

/**
  * @}
  */

/* Exported types ------------------------------------------------------------*/
/** @defgroup GAPUINO_S25FS_QSPI_Exported_Types QSPI Exported Types
  * @{
  */
/* QSPI Info */
typedef struct {
  uint32_t FlashSize;          /*!< Size of the flash */
  uint32_t EraseSectorSize;    /*!< Size of sectors for the erase operation */
  uint32_t EraseSectorsNumber; /*!< Number of sectors for the erase operation */
  uint32_t ProgPageSize;       /*!< Size of pages for the program operation */
  uint32_t ProgPagesNumber;    /*!< Number of pages for the program operation */
} S25FS_QSPI_Info;

/**
  * @}
  */

/* Exported functions --------------------------------------------------------*/
/** @defgroup GAPUINO_S25FS_QSPI_Exported_Functions QSPI Exported Functions
  * @{
  */
uint8_t S25FS_QSPI_Init                  (void);
uint8_t S25FS_QSPI_DeInit                (void);
uint8_t S25FS_QSPI_Read                  (uint8_t* pData, uint32_t ReadAddr, uint32_t Size);
uint8_t S25FS_QSPI_Write                 (uint8_t* pData, uint32_t WriteAddr, uint32_t Size);
uint8_t S25FS_QSPI_Erase_Sector          (uint32_t Sector);
uint8_t S25FS_QSPI_Erase_Chip            (void);
uint8_t S25FS_QSPI_GetStatus             (void);
uint8_t S25FS_QSPI_GetStatus2            (void);
uint8_t S25FS_QSPI_GetInfo               (S25FS_QSPI_Info* pInfo);
uint8_t S25FS_QSPI_SuspendErase          (void);
uint8_t S25FS_QSPI_ResumeErase           (void);
uint8_t S25FS_QSPI_EnterDeepPowerDown    (void);
uint8_t S25FS_QSPI_LeaveDeepPowerDown    (void);
/**
  * @}
  */

/**
  * @}
  */

/**
  * @}
  */

/**
  * @}
  */

#ifdef __cplusplus
}
#endif

#endif /* __GAPUINO_S25FS_QSPI_H */
