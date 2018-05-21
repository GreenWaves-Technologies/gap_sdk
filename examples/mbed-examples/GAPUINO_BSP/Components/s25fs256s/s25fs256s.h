/**
  ******************************************************************************
  * @file    s25fs256s.h
  * @author  GreenWaves technologies Application Team
  * @version V1.0.0
  * @date    18-April-2018
  * @brief   This file contains all the description of the S25FS256S QSPI memory.
  ******************************************************************************/
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
#ifndef __S25FS256S_H
#define __S25FS256S_H

#ifdef __cplusplus
 extern "C" {
#endif

/* Includes ------------------------------------------------------------------*/

/** @addtogroup BSP
  * @{
  */

/** @addtogroup Components
  * @{
  */

/** @addtogroup s25fs256s
  * @{
  */

/** @defgroup S25FS256S_Exported_Types
  * @{
  */

/**
  * @}
  */

/** @defgroup S25FS256S_Exported_Constants
  * @{
  */

/**
  * @brief  S25FS256S Configuration
  */
#define S25FS256S_FLASH_SIZE                0x2000000 /* 256 MBits => 32MBytes */
#define S25FS256S_4K_SECTOR_SIZE            0x1000    /* 8 blocks of 4KBytes at the bottom or top according CR1NV[2] 0 for bottom 1 for top */
                                                      /* Only exist when hybird sector architecture is chosed according to CR3V[3], 0 for hybrid 1 for uniform 64KBytes sector  */
#define S25FS256S_32K_SECTOR_SIZE           0x8000    /* One 32KBytes block, when has 4-kB Sectors, 64-kB Physical Uniform Sectors */
#define S25FS256S_64K_SECTOR_SIZE           0x10000   /* 512 blocks of uinform 64KBytes,  according to CR3V[1] 0 */
#define S25FS256S_224K_SECTOR_SIZE          0x38000   /* One 224KBytes block, when has 4-kB Sectors, 256-kB Physical Uniform Sectors*/
#define S25FS256S_256K_SECTOR_SIZE          0x40000   /* 128 blocks of uinform 256KBytes, according to CR3V[1] 1 */
#define S25FS256S_256_PAGE_SIZE             0x100     /* 32768 pages of 256 bytes, according to CR3V[4] 0 */
#define S25FS256S_512_PAGE_SIZE             0x200     /* 32768 pages of 512 bytes, according to CR3V[4] 1 */

#define S25FS256S_DUMMY_CYCLES_READ         8

#define S25FS256S_ALT_BYTES_PE_MODE         0xA5
#define S25FS256S_ALT_BYTES_NO_PE_MODE      0x0A

#define S25FS256S_CHIP_ERASE_MAX_TIME           360000
#define S25FS256S_4K_64K_SECTOR_ERASE_MAX_TIME  725
#define S25FS256S_256K_SECTOR_ERASE_MAX_TIME    2900

/**
  * @brief  S25FS256S Commands
  */
/* Read Operations */
#define READ_CMD                             0x03
#define READ4_CMD                            0x13
#define FAST_READ_CMD                        0x0B
#define FAST4_READ_CMD                       0x0C
#define DUAL_IO_READ_CMD                     0xBB
#define DUAL_IO_READ4_CMD                    0xBC
#define QUAD_IO_READ_CMD                     0xEB
#define QUAD_IO_READ4_CMD                    0xEC
#define DDR_QUAD_IO_READ_CMD                 0xED
#define DDR_QUAD_IO_READ4_CMD                0xEE

/* Program Operations */
#define PAGE_PROG_CMD                        0x02
#define PAGE_PROG4_CMD                       0x12

/* Erase Operations */
#define PARAMETER_4K_SECTOR_ERASE_CMD        0x20
#define PARAMETER_4K_SECTOR_ERASE4_CMD       0x21
#define SECTOR_ERASE_64K_256K_CMD            0xD8
#define SECTOR_ERASE_64K_256K4_CMD           0xDC

#define BULK_ERASE_CMD                       0x60
#define BULK_ERASE_CMD_2                     0xC7

#define PROG_ERASE_SUSPEND_CMD               0x75
#define PROG_ERASE_SUSPEND_CMD_2             0x85
#define PROG_ERASE_SUSPEND_CMD_3             0xB0
#define PROG_ERASE_RESUME_CMD                0x7A
#define PROG_ERASE_RESUME_CMD_2              0x8A
#define PROG_ERASE_RESUME_CMD_3              0x30

/* Identification Operations */
#define READ_ID_CMD                          0x9F
#define READ_SERIAL_FLASH_DISCO_PARAM_CMD    0x5A
#define READ_QUAD_ID_CMD                     0xAF

/* Register Operations */
#define READ_STATUS_REG1_CMD                 0x05
#define READ_STATUS_REG2_CMD                 0x07
#define READ_CFG_REG1_CMD                    0x35
#define READ_ANY_REG_CMD                     0x65
#define WRITE_STATUS_CFG_REG_CMD             0x01
/* Write Operations */
#define WRITE_DISABLE_CMD                    0x04
#define WRITE_ENABLE_CMD                     0x06
#define WRITE_ANY_REG_CMD                    0x71

#define CLEAR_STATUS_REG1_CMD                0x30
#define CLEAR_STATUS_REG1_CMD_2              0x82
#define ENTER_4B_ADDR_MODE                   0xB7
/* Burst Operations */
#define SET_BURST_LENGTH_CMD                 0xC0
#define EVALUATE_ERASE_STATUS_CMD            0xD0
#define ECC_READ_CMD                         0x19
#define ECC_READ4_CMD                        0x18
#define DATA_LEARNING_PATTERN_READ_CMD       0x41
#define PROG_NV_DATA_LEARNING_REG_CMD        0x43
#define WRITE_VOL_LEARNING_REG_CMD           0x4A

/* Power Down Operations */
#define DEEP_POWER_DOWN_CMD                  0xB9
#define RELEASE_DEEP_POWER_DOWN_CMD          0xAB


/* One-Time Programmable Operations */
#define OTP_PROG_CMD                         0x42
#define OTP_READ_CMD                         0x4B

/* Advanced Sectpr Protection Operations */
#define DYB_READ_CMD                         0xFA
#define DYB_READ4_CMD                        0xE0
#define DYB_WRITE_CMD                        0xFB
#define DYB_WRITE4_CMD                       0xF1
#define PPB_READ_CMD                         0xFC
#define PPB_READ4_CMD                        0xE2
#define PPB_PROG_CMD                         0xFD
#define PPB_PROG4_CMD                        0xE3
#define PPB_ERASE_CMD                        0xE4
#define ASP_READ_CMD                         0x2B
#define ASP_PROG_CMD                         0x2F
#define PPB_LOCK_BIT_READ_CMD                0xA7
#define PPB_LOCK_BIT_WRITE_CMD               0xA6
#define PASS_READ_CMD                        0xE7
#define PASS_PROG_CMD                        0xE8
#define PASS_UNLOCK_CMD                      0xE9

/* Reset Operations */
#define RESET_ENABLE_CMD                     0x66
#define RESET_MEMORY_CMD                     0x99
#define LEGAL_SW_RESET_CMD                   0xF0
#define MODE_BIT_RESET_CMD                   0xFF

/**
  * @brief  S25FS256S Registers
  */
/* Status Register 1 */
#define S25FS256S_SR1_WIP                    ((uint8_t)0x01)    /*!< Write in progress */
#define S25FS256S_SR1_WEL                    ((uint8_t)0x02)    /*!< Write enable latch */
#define S25FS256S_SR1_BP                     ((uint8_t)0x1C)    /*!< Block protect */
#define S25FS256S_SR1_E_ERR                  ((uint8_t)0x20)    /*!< Erase error occurred */
#define S25FS256S_SR1_P_ERR                  ((uint8_t)0x40)    /*!< Programming error occurred */
#define S25FS256S_SR1_SRWD                   ((uint8_t)0x80)    /*!< Status register write disable */

/* Status Register 2 */
#define S25FS256S_SR2_PS                     ((uint8_t)0x01)    /*!< Program suspend */
#define S25FS256S_SR2_ES                     ((uint8_t)0x02)    /*!< Erase suspend */
#define S25FS256S_SR2_ESTAT                  ((uint8_t)0x04)    /*!< Erase status */

/* Configuration Register 1 */
#define S25FS256S_CR1_FREEZE                 ((uint8_t)0x01)    /*!< Lock-down Block protection until next power cycle */
#define S25FS256S_CR1_QUAD                   ((uint8_t)0x02)    /*!< Quad I/O mode */

/* Configuration Register 2 */
#define S25FS256S_CR2_RL_NV                  ((uint8_t)0x0F)    /*!< Read latency */
#define S25FS256S_CR2_IO3R_NV                ((uint8_t)0x20)    /*!< IO3 reset */
#define S25FS256S_CR2_QA_NV                  ((uint8_t)0x40)    /*!< QPI */
#define S25FS256S_CR2_AL_NV                  ((uint8_t)0x80)    /*!< Address length */

/* Configuration Register 3 */
#define S25FS256S_CR3_F0H_NV                 ((uint8_t)0x01)    /*!< F0h software reset is enable or disable(ignore) */
#define S25FS256S_CR3_D8H_NV                 ((uint8_t)0x02)    /*!< 256-KB Erase or 64-KB Erase */
#define S25FS256S_CR3_30H_NV                 ((uint8_t)0x04)    /*!< 30h is erasing or Program resume command or is clear status command */
#define S25FS256S_CR3_20H_NV                 ((uint8_t)0x08)    /*!< 4-KB Erase disable or enable */
#define S25FS256S_CR3_02H_NV                 ((uint8_t)0x10)    /*!< Wrap at 512 bytes or 256 bytes */
#define S25FS256S_CR3_BCH_NV                 ((uint8_t)0x20)    /*!< Blank check during erase enable or disable */

/* Configuration Register 3 */
#define S25FS256S_CR4_WL_O                   ((uint8_t)0x03)    /*!< Wrap enable or disable */
#define S25FS256S_CR4_WE_O                   ((uint8_t)0x10)    /*!< Wrap legnth 8, 16. 32. 64 */

/* ECC Status Register */
#define S25FS256S_ECCSR_ECCDI                ((uint8_t)0x01)    /*!< ECC disable */
#define S25FS256S_ECCSR_EECCD                ((uint8_t)0x02)    /*!< Error in ECC unit data */
#define S25FS256S_ECCSR_EECC                 ((uint8_t)0x04)    /*!< Error in ECC */

/* ASP Status Register */
#define S25FS256S_ASPR_PSTMLB                ((uint8_t)0x02)    /*!< Persistent protection mode lock bit */
#define S25FS256S_ASPR_PWDMLB                ((uint8_t)0x04)    /*!< Password protection mode lock bit */

/* Password Register */
#define S25FS256S_PASS_PWD                   ((uint64_t)0xFFFFFFFFFFFFFFFF)    /*!< Hidden password */

/* PPB Lock Register */
#define S25FS256S_PPBL_PPBLOCK               ((uint8_t)0x01)    /*!< Protect PPB Array */

/* PPB Access Register */
#define S25FS256S_PPBAR_PPB                  ((uint8_t)0xFF)    /*!< Read or program per sector PPB */

/* DYB Access Register */
#define S25FS256S_DYBAR_DYB                  ((uint8_t)0xFF)    /*!< Read or program per sector DYB */

/* SPI DDR Data Learning Register */
#define S25FS256S_DLR_DLP                    ((uint8_t)0xFF)    /*!< Data Learning pattern */

/**
  * @brief  S25FS256S Registers Address Map
  */
/* Non-Valatile Registers */
#define SR1NV                                ((uint32_t)0x00)
#define CR1NV                                ((uint32_t)0x02)
#define CR2NV                                ((uint32_t)0x03)
#define CR3NV                                ((uint32_t)0x04)
#define CR4NV                                ((uint32_t)0x05)
#define NVDLR                                ((uint32_t)0x10)
#define PASS0                                ((uint32_t)0x20)
#define PASS1                                ((uint32_t)0x21)
#define PASS2                                ((uint32_t)0x22)
#define PASS3                                ((uint32_t)0x23)
#define PASS4                                ((uint32_t)0x24)
#define PASS5                                ((uint32_t)0x25)
#define PASS6                                ((uint32_t)0x26)
#define PASS7                                ((uint32_t)0x27)
#define ASPR0                                ((uint32_t)0x30)
#define ASPR1                                ((uint32_t)0x31)

/* Valatile Registers */
#define SR1V                                ((uint32_t)0x800000)
#define SR2V                                ((uint32_t)0x800001)
#define CR1V                                ((uint32_t)0x800002)
#define CR2V                                ((uint32_t)0x800003)
#define CR3V                                ((uint32_t)0x800004)
#define CR4V                                ((uint32_t)0x800005)
#define VDLR                                ((uint32_t)0x800010)
#define PPBL                                ((uint32_t)0x800040)

/**
  * @}
  */

/** @defgroup S25FS256S_Exported_Functions
  * @{
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

/**
  * @}
  */

#ifdef __cplusplus
}
#endif

#endif /* __S25FS256S_H */

/************************ (C) COPYRIGHT STMicroelectronics *****END OF FILE****/
