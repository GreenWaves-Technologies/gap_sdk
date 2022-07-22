/****************************************************************************
** Copyright (C) 2020 MikroElektronika d.o.o.
** Contact: https://www.mikroe.com/contact
**
** Permission is hereby granted, free of charge, to any person obtaining a copy
** of this software and associated documentation files (the "Software"), to deal
** in the Software without restriction, including without limitation the rights
** to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
** copies of the Software, and to permit persons to whom the Software is
** furnished to do so, subject to the following conditions:
** The above copyright notice and this permission notice shall be
** included in all copies or substantial portions of the Software.
**
** THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND,
** EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES
** OF MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT.
** IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM,
** DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT
** OR OTHERWISE, ARISING FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE
**  USE OR OTHER DEALINGS IN THE SOFTWARE.
****************************************************************************/

/*!
 * @file lpwifi.h
 * @brief This file contains API for LP WiFi Click Driver.
 */

#ifndef LPWIFI_H
#define LPWIFI_H

#include "pmsis.h"

#define PAD_UART1_RX        (PI_PAD_044)
#define PAD_UART1_TX        (PI_PAD_045)
#define PAD_RST             (PI_PAD_086)
#define PAD_CS              (PI_PAD_034)
#define PAD_GPIO_3V3_PERIPH (PI_PAD_035)

/**
 * @brief LP WiFi - Basic AT commands.
 * @details Specified basic AT Commands for communication with LP WiFi Click board.
 */
#define LPWIFI_CMD_AT            "AT"
#define LPWIFI_CMD_ATZ           "ATZ"
#define LPWIFI_CMD_ATE           "ATE"
#define LPWIFI_CMD_ATF           "ATF"

/**
 * @brief LP WiFi - WiFi AT commands.
 * @details Specified WiFi AT Commands for communication with LP WiFi Click board.
 */
#define LPWIFI_CMD_WFMODE        "AT+WFMODE"
#define LPWIFI_CMD_WFJAPA        "AT+WFJAPA"

/**
 * @brief LP WiFi - Transfer AT commands.
 * @details Specified Transfer AT Commands for communication with LP WiFi Click board.
 */
#define LPWIFI_CMD_TRTS          "AT+TRTS"
#define LPWIFI_CMD_TRUSE         "AT+TRUSE"

/**
 * @brief LP WiFi - PWM output test commands.
 * @details Specified PWM output test commands for communication with LP WiFi Click board.
 */
#define LPWIFI_CMD_PWMINIT       "AT+PWMINIT"
#define LPWIFI_CMD_PWMSTART      "AT+PWMSTART"
#define LPWIFI_CMD_PWMSTOP       "AT+PWMSTOP"

/**
 * @brief LP WiFi - I2C read/write test commands.
 * @details Specified I2C read/write test commands for communication with LP WiFi Click board.
 */
#define LPWIFI_CMD_I2CINIT       "AT+I2CINIT"
#define LPWIFI_CMD_I2CREAD       "AT+I2CREAD"
#define LPWIFI_CMD_I2CWRITE      "AT+I2CWRITE"

/*! @} */ // lpwifi_reg

/**
 * @defgroup lpwifi_set LP WiFi Device Settings
 * @brief Settings for registers of LP WiFi Click driver.
 */

/**
 * @addtogroup lpwifi_set
 * @{
 */

/**
 * @brief LP WiFi - WiFi mode setting.
 * @details Specified setting for WiFi mode.
 */
#define LPWIFI_MODE_STA          0
#define LPWIFI_MODE_SOFT_AP      1

/**
 * @brief LP WiFi driver buffer size.
 * @details Specified size of driver ring buffer.
 * @note Increase buffer size if needed.
 */
#define DRV_BUFFER_SIZE          400


typedef enum
{
    LPWIFI_OK = 0,
    LPWIFI_ERROR = -1

} lpwifi_return_value_t;

static int enable_3v3_periph(void);
static int8_t enable_rst(void);
static int8_t enable_cs(void);
static void setup_pads(void);

int8_t lpwifi_init(struct pi_device* uart, int baudrate);
void lpwifi_deinit(struct pi_device* uart);
void enable_cs_wakup();
int lpwifi_generic_write ( struct pi_device* uart, char *data_buf, uint16_t len );
int lpwifi_generic_read (struct pi_device* uart, char *data_buf, uint16_t max_len );
void lpwifi_send_cmd ( struct pi_device* uart, char *cmd );
void lpwifi_send_cmd_payload ( struct pi_device* uart, char *cmd,int size );
void lpwifi_send_cmd_with_parameter (struct pi_device* uart, char *at_cmd_buf, char *param_buf );
void lpwifi_send_cmd_check ( struct pi_device* uart, char *at_cmd_buf );
void lpwifi_factory_reset_device ( struct pi_device* uart );

void lpwifi_connect_to_ap ( struct pi_device* uart, char *ssid, char *password );
void lpwifi_set_wifi_mode ( struct pi_device* uart, uint8_t mode );
void lpwifi_create_tcp_server ( struct pi_device* uart, uint16_t port );
void lpwifi_create_udp_socket ( struct pi_device* uart, uint16_t port );
#endif // LPWIFI_H
