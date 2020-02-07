/*
 * Copyright (C) 2019 GreenWaves Technologies
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *     http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

#ifndef __PI_BSP_BLE_NINA_B112_H__
#define __PI_BSP_BLE_NINA_B112_H__

#include "bsp/ble.h"

/**
 * @addtogroup BLE
 * @{
 */

/**
 * @defgroup NINA_B112 NINA_B112
 *
 * The nina_b112 driver provides support for for data transfer using a BLE module,
 * here a NINA B112 BLE module.
 * This module is interfaced on GAPPOC through UART.
 *
 * @addtogroup NINA_B112
 * @{
 */

#define PI_AT_RESP_ARRAY_LENGTH ( 64 ) /*!< RESP array length. */

/**
 * \struct pi_nina_b112_conf
 *
 * \brief NINA_B112 configuration structure.
 *
 * This structure holds BLE configuration(interface used, baudrate,...).
 */
struct pi_nina_b112_conf
{
    uint8_t uart_itf;           /*!< UART interface used to connect BLE device. */
    //pi_device_api_t *api;       /*!< NINA_B112 API binding to BLE API. */
    char local_name[30];        /*!< BLE device name(visible by others). */
    uint32_t baudrate;        /*!< UART baudrate. */
    uint8_t parity_bits;
    uint8_t stop_bits;
};

struct pi_nina_b112_id
{
    char manufacturer_id[20];
    char model_id[20];
    char sw_ver_id[20];
    char serial_num[20];
};

typedef enum
{
    PI_NINA_B112_MODEL_INFO       = 0, /*!< BLE device info. */
    PI_NINA_B112_SERVER_CONFIGURE,     /*!< Configure BLE device as a server. */
    PI_NINA_B112_CLIENT_CONFIGURE,     /*!< Configure BLE device as a client. */
    PI_NINA_B112_UART_CONFIGURE,       /*!< Configure HCI UART. */
    PI_NINA_B112_DATA_MODE_ENTER,      /*!< Enter Data Mode. */
    PI_NINA_B112_DATA_MODE_EXIT,       /*!< Exit Data Mode. */
    PI_NINA_B112_WAIT_FOR_EVENT        /*!< Wait for some responses/events from BLE device. */
} pi_ble_ioctl_cmd_e;


/**
 * \brief Initialize NINA_B112 configuration structure.
 *
 * \param device         Pointer to the BLE device structure.
 * \param conf           Pointer to NINA_B112 configuration structure.
 */
void pi_ble_nina_b112_conf_init(struct pi_device *device, struct pi_nina_b112_conf *conf);

/**
 * @} addtogroup NINA_B112
 */

/**
 * @} addtogroup BLE
 */

#endif  /* __PI_BSP_BLE_NINA_B112_H__ */
