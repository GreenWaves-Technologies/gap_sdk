/*
 * Copyright (c) Nordic Semiconductor ASA
 * All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without modification,
 * are permitted provided that the following conditions are met:
 *
 *   1. Redistributions of source code must retain the above copyright notice, this
 *   list of conditions and the following disclaimer.
 *
 *   2. Redistributions in binary form must reproduce the above copyright notice, this
 *   list of conditions and the following disclaimer in the documentation and/or
 *   other materials provided with the distribution.
 *
 *   3. Neither the name of Nordic Semiconductor ASA nor the names of other
 *   contributors to this software may be used to endorse or promote products
 *   derived from this software without specific prior written permission.
 *
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
 *
 */

/**@file
 *
 * @defgroup nrf_dfu_types Types and definitions.
 * @{
 *
 * @ingroup nrf_dfu
 *
 * @brief Device Firmware Update module type and definitions.
 */

#ifndef DFU_TYPES_H__
#define DFU_TYPES_H__

#include <stdint.h>
#include "nrf_sdm.h"
#include "nrf.h"
#include "app_util.h"

#define NRF_UICR_BOOT_START_ADDRESS     (NRF_UICR_BASE + 0x14)                                          /**< Register where the bootloader start address is stored in the UICR register. */

#define CODE_REGION_1_START             SD_SIZE_GET(MBR_SIZE)                                           /**< This field should correspond to the size of Code Region 0, (which is identical to Start of Code Region 1), found in UICR.CLEN0 register. This value is used for compile safety, as the linker will fail if application expands into bootloader. Runtime, the bootloader will use the value found in UICR.CLEN0. */
#define SOFTDEVICE_REGION_START         MBR_SIZE                                                        /**< This field should correspond to start address of the bootloader, found in UICR.RESERVED, 0x10001014, register. This value is used for sanity check, so the bootloader will fail immediately if this value differs from runtime value. The value is used to determine max application size for updating. */

#ifdef NRF51
#ifdef SIGNING
#define BOOTLOADER_REGION_START         0x00039C00                                                    /**< This field should correspond to start address of the bootloader, found in UICR.RESERVED, 0x10001014, register. This value is used for sanity check, so the bootloader will fail immediately if this value differs from runtime value. The value is used to determine max application size for updating. */
#define BOOTLOADER_SETTINGS_ADDRESS     0x0003D800                                                    /**< The field specifies the page location of the bootloader settings address. */
#else
#define BOOTLOADER_REGION_START         0x0003C000                                                      /**< This field should correspond to start address of the bootloader, found in UICR.RESERVED, 0x10001014, register. This value is used for sanity check, so the bootloader will fail immediately if this value differs from runtime value. The value is used to determine max application size for updating. */
#define BOOTLOADER_SETTINGS_ADDRESS     0x0003FC00                                                      /**< The field specifies the page location of the bootloader settings address. */
#endif

#define CODE_PAGE_SIZE                  0x0400                                                          /**< Size of a flash codepage. Used for size of the reserved flash space in the bootloader region. Will be runtime checked against NRF_UICR->CODEPAGESIZE to ensure the region is correct. */
#elif NRF52
#define BOOTLOADER_REGION_START         0x0007B000                                                      /**< This field should correspond to start address of the bootloader, found in UICR.RESERVED, 0x10001014, register. This value is used for sanity check, so the bootloader will fail immediately if this value differs from runtime value. The value is used to determine max application size for updating. */
#define BOOTLOADER_SETTINGS_ADDRESS     0x0007F000                                                      /**< The field specifies the page location of the bootloader settings address. */
#define CODE_PAGE_SIZE                  0x1000                                                          /**< Size of a flash codepage. Used for size of the reserved flash space in the bootloader region. Will be runtime checked against NRF_UICR->CODEPAGESIZE to ensure the region is correct. */
#else
#error No target defined
#endif

#define DFU_REGION_TOTAL_SIZE           (BOOTLOADER_REGION_START - CODE_REGION_1_START)                 /**< Total size of the region between SD and Bootloader. */

#define DFU_APP_DATA_RESERVED           0x0000                                                          /**< Size of Application Data that must be preserved between application updates. This value must be a multiple of page size. Page size is 0x400 (1024d) bytes, thus this value must be 0x0000, 0x0400, 0x0800, 0x0C00, 0x1000, etc. */
#define DFU_BANK_PADDING                (DFU_APP_DATA_RESERVED % (2 * CODE_PAGE_SIZE))                  /**< Padding to ensure that image size banked is always page sized. */
#define DFU_IMAGE_MAX_SIZE_FULL         (DFU_REGION_TOTAL_SIZE - DFU_APP_DATA_RESERVED)                 /**< Maximum size of an application, excluding save data from the application. */
#define DFU_IMAGE_MAX_SIZE_BANKED       ((DFU_REGION_TOTAL_SIZE - \
                                          DFU_APP_DATA_RESERVED - \
                                          DFU_BANK_PADDING) / 2)                                        /**< Maximum size of an application, excluding save data from the application. */

#define DFU_BL_IMAGE_MAX_SIZE           (BOOTLOADER_SETTINGS_ADDRESS - BOOTLOADER_REGION_START)         /**< Maximum size of a bootloader, excluding save data from the current bootloader. */

#define DFU_BANK_0_REGION_START         CODE_REGION_1_START                                             /**< Bank 0 region start. */
#define DFU_BANK_1_REGION_START         (DFU_BANK_0_REGION_START + DFU_IMAGE_MAX_SIZE_BANKED)           /**< Bank 1 region start. */

#define EMPTY_FLASH_MASK                0xFFFFFFFF                                                      /**< Bit mask that defines an empty address in flash. */

#define INVALID_PACKET                  0x00                                                            /**< Invalid packet identifies. */
#define INIT_PACKET                     0x01                                                            /**< Packet identifies for initialization packet. */
#define STOP_INIT_PACKET                0x02                                                            /**< Packet identifies for stop initialization packet. Used when complete init packet has been received so that the init packet can be used for pre validaiton. */
#define START_PACKET                    0x03                                                            /**< Packet identifies for the Data Start Packet. */
#define DATA_PACKET                     0x04                                                            /**< Packet identifies for a Data Packet. */
#define STOP_DATA_PACKET                0x05                                                            /**< Packet identifies for the Data Stop Packet. */

#define DFU_UPDATE_SD                   0x01                                                            /**< Bit field indicating update of SoftDevice is ongoing. */
#define DFU_UPDATE_BL                   0x02                                                            /**< Bit field indicating update of bootloader is ongoing. */
#define DFU_UPDATE_APP                  0x04                                                            /**< Bit field indicating update of application is ongoing. */

#define DFU_INIT_RX                     0x00                                                            /**< Op Code identifies for receiving init packet. */
#define DFU_INIT_COMPLETE               0x01                                                            /**< Op Code identifies for transmission complete of init packet. */

// Safe guard to ensure during compile time that the DFU_APP_DATA_RESERVED is a multiple of page size.
STATIC_ASSERT((((DFU_APP_DATA_RESERVED) & (CODE_PAGE_SIZE - 1)) == 0x00));

/**@brief Structure holding a start packet containing update mode and image sizes.
 */
typedef struct
{
    uint8_t  dfu_update_mode;                                                                           /**< Packet type, used to identify the content of the received packet referenced by data packet. */
    uint32_t sd_image_size;                                                                             /**< Size of the SoftDevice image to be transferred. Zero if no SoftDevice image will be transfered. */
    uint32_t bl_image_size;                                                                             /**< Size of the Bootloader image to be transferred. Zero if no Bootloader image will be transfered. */
    uint32_t app_image_size;                                                                            /**< Size of the application image to be transmitted. Zero if no Bootloader image will be transfered. */
} dfu_start_packet_t;

/**@brief Structure holding a bootloader init/data packet received.
 */
typedef struct
{
    uint32_t   packet_length;                                                                           /**< Packet length of the data packet. Each data is word size, meaning length of 4 is 4 words, not bytes. */
    uint32_t * p_data_packet;                                                                           /**< Data Packet received. Each data is a word size entry. */
} dfu_data_packet_t;

/**@brief Structure for holding dfu update packet. Packet type indicate the type of packet.
 */
typedef struct
{
    uint32_t   packet_type;                                                                             /**< Packet type, used to identify the content of the received packet referenced by data packet. */
    union
    {
        dfu_data_packet_t    data_packet;                                                               /**< Used when packet type is INIT_PACKET or DATA_PACKET. Packet contains data received for init or data. */
        dfu_start_packet_t * start_packet;                                                              /**< Used when packet type is START_DATA_PACKET. Will contain information on software to be updtaed, i.e. SoftDevice, Bootloader and/or Application along with image sizes. */
    } params;
} dfu_update_packet_t;

/**@brief DFU status error codes.
*/
typedef enum
{
    DFU_UPDATE_APP_COMPLETE,                                                                            /**< Status update of application complete.*/
    DFU_UPDATE_SD_COMPLETE,                                                                             /**< Status update of SoftDevice update complete. Note that this solely indicates that a new SoftDevice has been received and stored in bank 0 and 1. */
    DFU_UPDATE_SD_SWAPPED,                                                                              /**< Status update of SoftDevice update complete. Note that this solely indicates that a new SoftDevice has been received and stored in bank 0 and 1. */
    DFU_UPDATE_BOOT_COMPLETE,                                                                           /**< Status update complete.*/
    DFU_BANK_0_ERASED,                                                                                  /**< Status bank 0 erased.*/
    DFU_TIMEOUT,                                                                                        /**< Status timeout.*/
    DFU_RESET                                                                                           /**< Status Reset to indicate current update procedure has been aborted and system should reset. */
} dfu_update_status_code_t;

/**@brief Structure holding DFU complete event.
*/
typedef struct
{
    dfu_update_status_code_t status_code;                                                               /**< Device Firmware Update status. */
    uint16_t                 app_crc;                                                                   /**< CRC of the recieved application. */
    uint32_t                 sd_size;                                                                   /**< Size of the recieved SoftDevice. */
    uint32_t                 bl_size;                                                                   /**< Size of the recieved BootLoader. */
    uint32_t                 app_size;                                                                  /**< Size of the recieved Application. */
    uint32_t                 sd_image_start;                                                            /**< Location in flash where the received SoftDevice image is stored. */
} dfu_update_status_t;

/**@brief Update complete handler type. */
typedef void (*dfu_complete_handler_t)(dfu_update_status_t dfu_update_status);

#endif // DFU_TYPES_H__

/**@} */
