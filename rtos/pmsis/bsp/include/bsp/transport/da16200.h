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

#ifndef __PI_BSP_TRANSPORT_DA16200_H__
#define __PI_BSP_TRANSPORT_DA16200_H__

#include "pmsis_types.h"

/**
 * \struct pi_da16200_conf
 * 
 * @brief DA16200 configuration structure
 */
struct pi_da16200_conf
{
	uint8_t uart_itf;
	uint32_t uart_baudrate;
	char parity_bits; /* n (None), e (Even), o (Odd) */
	uint8_t stop_bits;
	uint8_t use_ctrl_flow;
};

/**
 * \enum bsp_da16200_ioctl_cmd_e
 * 
 * @brief IOCTL commands for DA16200 module
 */
typedef enum
{
    BSP_DA16200_TCP_SERVER_CONFIGURE,   /*!< Configure device as a tcp server. */
    BSP_DA16200_TCP_CLIENT_CONFIGURE,   /*!< Configure device as a tcp client. */
    BSP_DA16200_UART_CONFIGURE,         /*!< Configure HCI UART. */
} bsp_da16200_ioctl_cmd_e;

/**
 * @brief Initialize AD16200 configuration structure
 * 
 * @param[out] device    Pointer to the AD16200 device
 * @param[out] conf      Pointer to the AD16200 configuration structure
 */
void pi_wifi_da16200_conf_init(struct pi_device *device, struct pi_da16200_conf *conf);

/**
 * @brief Set struct da16200_t's client_ip_port field
 * 
 * @param[in] device 
 * @param[in] client_ip_port 
 */
void pi_wifi_da16200_client_ip_port_set(struct pi_device *device, const char *client_ip_port);

#endif
