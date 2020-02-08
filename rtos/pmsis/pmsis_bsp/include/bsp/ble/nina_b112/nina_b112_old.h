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

#ifndef __BSP_NINA_B112_H__
#define __BSP_NINA_B112_H__

#include "pmsis.h"

/**
 * @addtogroup BLE
 * @{
 */

/**
 * \deprecated This API is no longer supported.
 *             Please refer to @ref BLE API.
 *
 * @defgroup NINA_B112_OLD NINA_B112_OLD
 * \brief Deprecated
 *
 * The nina_b112 driver provides support for for data transfer using a BLE module,
 * here a NINA B112 BLE module.
 * This module is interfaced on GAPPOC through UART.
 *
 * @addtogroup NINA_B112_OLD
 * @{
 */

/*******************************************************************************
 * Definitions
 ******************************************************************************/

#define PI_AT_RESP_ARRAY_LENGTH ( 64 ) /*!< RESP array length. */

/* @brief NINA Ble module struct.  */
typedef struct
{
    struct pi_device uart_device; /*!< UART interface used to communicate with BLE module.  */
    uint8_t *rx_char;             /*!< Byte used to receive response. */
    uint8_t *rx_buffer;           /*!< Buffer used to receive response. */
} pi_nina_ble_t;

/* @brief BLE callback function used for asynchronous transfers. */
typedef void (*ble_callback_f)(void *arg);

/*******************************************************************************
 * API
 ******************************************************************************/

/**
 * @brief Initialize BLE configuration.
 *
 * This function initializes a BLE module, opening IO interface(UART here).
 *
 * @param ble            Structure holding IO device.
 *
 * @note This function is not implemented.
 */
void pi_nina_b112_conf_init(pi_nina_ble_t *ble);

/**
 * @brief Open BLE module.
 *
 * This function initializes and sets up a BLE module.
 * It does all the needed configuration to enable the BLE module(buffer allocation,
 * opening IO interface, etc.).
 * Thus this function must be called before using BLE module.
 *
 * @param ble            Structure holding IO device.
 *
 * @retval 0 if the operation is successful.
 * @retval error otherwise.
 */
int32_t pi_nina_b112_open(pi_nina_ble_t *ble);

/**
 * @brief Close BLE module.
 *
 * This function closes a BLE module.
 * This function should be called to a BLE module once it is not needed and to
 * free allocated objects.
 *
 * @param ble            Structure holding IO device.
 */
void pi_nina_b112_close(pi_nina_ble_t *ble);

/**
 * @brief Send an AT command to BLE module.
 *
 * This function sends an AT command to a BLE module.
 *
 * @param ble            Structure holding IO device.
 * @param cmd            Command to send.
 *
 * @note This function is used by pi_nina_b112_AT_send(). pi_nina_b112_AT_send()
 * should be used to send AT commands since this function does not handle
 * responses sent by BLE module.
 */
void pi_nina_b112_AT_cmd_send(pi_nina_ble_t *ble, const char* cmd);

/**
 * @brief Send an AT command to BLE module.
 *
 * This function sends an AT command to a BLE module, and waits for response
 * from BLE module.
 * This function should be used to configure BLE module(name, baudrate,...).
 *
 * @param ble            Structure holding IO device.
 * @param cmd            Command to send.
 *
 * @retval WR_RES_OK     if successfully configured.
 * @retval WR_RES_ERROR  in case an error occurred.
 * @retval WR_RES_UNSOL  Response sent when not waiting for a response.
 *
 * @note Use this function to configure BLE module instead of pi_nina_b112_AT_cmd_send().
 */
uint32_t pi_nina_b112_AT_send(pi_nina_ble_t *ble, const char* cmd);

/**
 * @brief Send an AT command request to BLE module, and expect a response.
 *
 * This function sends an AT command request to a BLE module, and it should return a response
 * for the command request.
 *
 * @param ble            Structure holding IO device.
 * @param cmd            Command request to send.
 * @param resp           Buffer to store response.
 */
void pi_nina_b112_AT_query(pi_nina_ble_t *ble, const char* cmd, char* resp);

/**
 * @brief Wait for a (unsolicited) response.
 *
 * This function sends an AT command request to a BLE module, and it should return a response
 * for the command request.
 *
 * @param ble            Structure holding IO device.
 * @param resp           Buffer to store response.
 */
void pi_nina_b112_wait_for_event(pi_nina_ble_t *ble, char* resp);

/**
 * @brief Get data from BLE module. Blocking API.
 *
 * This function is used to retrieve data from BLE module. The data is stored
 * into given buffer.
 *
 * @param ble            Structure holding IO device.
 * @param buffer         Buffer to store response.
 * @param size           Size of response data.
 *
 * @note This function is synchronous, caller can not do anything else until transfer
 * is finished. The pending asynchronous function is below : pi_nina_b112_get_data().
 */
void pi_nina_b112_get_data_blocking(pi_nina_ble_t *ble, uint8_t* buffer,
                                    uint32_t size);

/**
 * @brief Get data from BLE module. Non blocking API.
 *
 * This function is used to retrieve data from BLE module. The data is stored
 * into given buffer.
 * Given struct pi_task can be used to wait end of transfer if needed.
 *
 * @param ble            Structure holding IO device.
 * @param buffer         Buffer to store response.
 * @param size           Size of response data.
 * @param task           Task to use to check end of transfer.
 *
 * @note This function is asynchronous, caller can do something else while transfer is
 * enqueued The pending synchronous function is below : pi_nina_b112_get_data_blocking().
 */
void pi_nina_b112_get_data(pi_nina_ble_t *ble, uint8_t* buffer,
                           uint32_t size, struct pi_task *task);

/**
 * @brief Send data to BLE module. Blocking API.
 *
 * This function is used to send data to BLE module.
 *
 * @param ble            Structure holding IO device.
 * @param buffer         Buffer to send.
 * @param size           Size of data to send.
 *
 * @note This function is synchronous, caller can not do anything else until transfer
 * is finished. The pending asynchronous function is below : pi_nina_b112_send_data().
 */
void pi_nina_b112_send_data_blocking(pi_nina_ble_t *ble, const uint8_t* buffer,
                                     uint32_t size);

/**
 * @brief Send data from BLE module. Non blocking API.
 *
 * This function is used to send data to BLE module.
 * Given struct pi_task can be used to wait end of transfer if needed.
 *
 * @param ble            Structure holding IO device.
 * @param buffer         Buffer to send.
 * @param size           Size of data to send.
 * @param task           Task to use to check end of transfer.
 *
 * @note This function is asynchronous, caller can dos something else while transfer is
 * enqueued The pending synchronous function is below : pi_nina_b112_send_data_blocking().
 */
void pi_nina_b112_send_data(pi_nina_ble_t *ble, const uint8_t* buffer, uint32_t size, struct pi_task *task);

/**
 * @brief Exit data mode when entered.
 *
 * This function should be used to exit data mode. Data mode is entered after
 * BLE module's configuration. In data mode, AT commands can not be sent, only
 * data can be sent.
 *
 * @param ble            Structure holding IO device.
 *
 * @note For NINA B112 module, a delay of 1s is needed before and after the call
 * to this function.
 */
void pi_nina_b112_exit_data_mode(pi_nina_ble_t *ble);

/**
 * @} addtogroup NINA_B112_OLD
 */

/**
 * @} addtogroup BLE
 */

#endif  /* __BSP_NINA_B112_H__ */
