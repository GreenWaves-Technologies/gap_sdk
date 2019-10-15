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
 * @defgroup BLE BLE
 *
 * The nina_b112 driver provides supprt for for data transfer using a BLE module,
 * here a NINA B112 BLE module.
 * This module is interfaced on GAPOC through UART.
 */

/**
 * @addtogroup BLE
 * @{
 */

/*******************************************************************************
 * Definitions
 ******************************************************************************/
/* Factory default S3(CR) & S4(LF) response format character.  */
#define S3char  '\r'            /*!< CR char, ASCII 0x0D. */
#define S4char  '\n'            /*!< LF char, ASCII 0x0A. */
/* Same, but to use in string functions (strcmp, etc...). */
#define S3str  "\r"             /*!< CR string.  */
#define S4str  "\n"             /*!< LF string. */

#define PI_AT_CMD_ARRAY_LENGTH  ( 32 ) /*!< CMD array length. */
#define PI_AT_RESP_ARRAY_LENGTH ( 64 ) /*!< RESP array length. */

#define PI_NINA_UART_AT_BAUDRATE_bps ( 115200 ) /*!< Baudrate used by NINA_B112 module(default value). */

struct pi_nina_b112_conf
{
//    struct ble_conf ble;
//    uint32_t uart_itf;          /* uart ID. */
    char skip_pads_config;
};

/* @brief NINA Ble module struct.  */
typedef struct
{
    struct pi_device uart_device; /*!< UART interface used to communicate with BLE module.  */
    uint8_t *rx_char;             /*!< Byte used to receive response. */
    uint8_t *rx_buffer;           /*!< Buffer used to receive response. */
} pi_nina_ble_t;

/* @brief AT command's response state. */
typedef enum _at_resp_state
{
    PI_AT_RESP_NOT_STARTED,     /*!< Response not received yet fron BLE module.  */
    PI_AT_RESP_IN_PROGRESS,     /*!< Response transmission has started. */
    PI_AT_RESP_DONE             /*!< Response transmission received, with end of resp. */
} at_resp_state_t;

/*! @brief Response type received for an AT command. */
typedef enum
{
    WR_RES_OK    = 0,          /*!< OK repsonse. */
    WR_RES_ERR   = -1,         /*!< ERROR response. */
    WR_RES_UNSOL = -2,         /*!< UNSOLLICITED_RESPONSE response. */
    WR_RES_NA    = -3          /*!< Non authorized response. */
} write_res_t;

/* @brief BLE callback function used for asynchronous transfers. */
typedef void (*ble_callback_f)(void *arg);

/*******************************************************************************
 * API
 ******************************************************************************/

/**
 * @brief Initialize ble configuration.
 *
 * This function initializes a ble module, opening IO interface(UART here).
 *
 * @param ble            Structure holding IO device.
 *
 * @note This function is not implemented.
 */
void pi_nina_b112_conf_init(pi_nina_ble_t *ble);

/**
 * @brief Open ble module.
 *
 * This function initializes and sets up a ble module.
 * It does all the needed configuration to enable the ble module(buffer allocation,
 * opening IO interface,...).
 * Thus this function must be called before using ble module.
 *
 * @param ble            Structure holding IO device.
 *
 * @retval 0 if the operation is successful.
 * @retval error otherwise.
 */
int32_t pi_nina_b112_open(pi_nina_ble_t *ble);

/**
 * @brief Close ble module.
 *
 * This function closes a ble module.
 * This function should be called to a ble module once it is not needed and to
 * free allocated objects.
 *
 * @param ble            Structure holding IO device.
 */
void pi_nina_b112_close(pi_nina_ble_t *ble);

/**
 * @brief Send an AT command to ble module.
 *
 * This function sends an AT command to a ble module.
 *
 * @param ble            Structure holding IO device.
 * @param cmd            Command to send.
 *
 * @note This function is used by pi_nina_b112_AT_send(). pi_nina_b112_AT_send()
 * should be used to send AT commands since this function does not handle
 * responses sent by ble module.
 */
void pi_nina_b112_AT_cmd_send(pi_nina_ble_t *ble, const char* cmd);

/**
 * @brief Send an AT command to ble module.
 *
 * This function sends an AT command to a ble module, and waits for response
 * from ble module.
 * This function should be used to configure ble module(name, baudrate,...).
 *
 * @param ble            Structure holding IO device.
 * @param cmd            Command to send.
 *
 * @retval WR_RES_OK     if successfully configured.
 * @retval WR_RES_ERROR  in case an error occurred.
 * @retval WR_RES_UNSOL  Response sent when not waiting for a response.
 *
 * @note Use this function to configure ble module instead of pi_nina_b112_AT_cmd_send().
 */
uint32_t pi_nina_b112_AT_send(pi_nina_ble_t *ble, const char* cmd);

/**
 * @brief Send an AT command request to ble module, and expect a response.
 *
 * This function sends an AT command request to a ble module, and it should return a response
 * for the command request.
 *
 * @param ble            Structure holding IO device.
 * @param cmd            Command request to send.
 * @param resp           Buffer to store response.
 */
void pi_nina_b112_AT_query(pi_nina_ble_t *ble, const char* cmd, char* resp);

/**
 * @brief Wait for a (unsollicited) response.
 *
 * This function sends an AT command request to a ble module, and it should return a response
 * for the command request.
 *
 * @param ble            Structure holding IO device.
 * @param resp           Buffer to store response.
 */
void pi_nina_b112_wait_for_event(pi_nina_ble_t *ble, char* resp);

/**
 * @brief Get data from ble module. Blocking API.
 *
 * This function is used to retrieve data from ble module. The data is stored
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
 * @brief Get data from ble module. Non blocking API.
 *
 * This function is used to retrieve data from ble module. The data is stored
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
 * @brief Send data to ble module. Blocking API.
 *
 * This function is used to send data to ble module.
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
 * @brief Send data from ble module. Non blocking API.
 *
 * This function is used to send data to ble module.
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
 * ble module's configuration. In data mode, AT commands can not be sent, only
 * data can be sent.
 *
 * @param ble            Structure holding IO device.
 *
 * @note For NINA B112 module, a delay of 1s is needed before and after the call
 * to this function.
 */
void pi_nina_b112_exit_data_mode(pi_nina_ble_t *ble);

/**
 * @}
 */

#endif  /* __BSP_NINA_B112_H__ */
