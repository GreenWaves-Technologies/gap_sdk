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

#ifndef __PI_BSP_WIFI_H__
#define __PI_BSP_WIFI_H__

#include "pmsis.h"

/**
 * @defgroup WIFI WIFI
 *
 * The WIFI driver provides support for wireless data transfer between the host
 * and an external peripheral such as a smartphone, using WiFi connection.
 */

/**
 * @addtogroup WIFI
 * @{
 */

/**
 * \brief WIFI specific API
 *
 * Structure holding WIFI specific API.
 */
typedef struct pi_wifi_api_s
{
    int32_t (*at_cmd)(struct pi_device *device, char *cmd, char *resp); /*!< Function to send AT command. */
    int8_t (*connect_to_ap)(struct pi_device *device, const char *ssid, const char *pwd, pi_task_t *callback); /*!< Function to connect WIFI device to an AP. */
    int8_t (*disconnect_from_ap)(struct pi_device *device); /*!< Function to disconnect WIFI device from a currently associated AP. */
    int8_t (*create_tcp_server)(struct pi_device *device, uint32_t port);
    int8_t (*wait_client_connection)(struct pi_device *device, char *resp, pi_task_t *callback);
    int8_t (*wait_client_event)(struct pi_device *device, char *resp, pi_task_t *callback);
} pi_wifi_api_t;


/**
 * \brief Open a WIFI device.
 *
 * This function opens and intializes a WIFI device.
 * This function must be called before using device.
 *
 * \param device         Pointer to the WIFI device structure.
 *
 * \retval 0             If the operation is successful.
 * \retval ERRNO         Error code otherwise.
 */
int32_t pi_wifi_open(struct pi_device *device);

/**
 * \brief Close a WIFI device.
 *
 * This function closes an opened WIFI device. It frees all allocated objects and
 * interfaces used.
 *
 * \param device         Pointer to the WIFI device structure.
 */
void pi_wifi_close(struct pi_device *device);

/**
 * \brief IOctl commands.
 *
 * This function is used to send special command to WIFI device.
 *
 * \param device         Pointer to the WIFI device structure.
 * \param cmd            Ioctl command.
 * \param arg            Ioctl command arg.
 *
 * \return Value         Value depends on ioctl command.
 */
int32_t pi_wifi_ioctl(struct pi_device *device, uint32_t cmd, void *arg);

/**
 * \brief Send AT command to WIFI device.
 *
 * This function is used to send AT commands to WIFI device.
 *
 * \param device         Pointer to the WIFI device structure.
 * \param cmd            Command string to send.
 * \param resp           Buffer to store response.
 *
 * \retval 0             If operation is successful.
 * \retval ERRNO         Error code otherwise.
 *
 * \note The command string should be plain command, i.e. without the "AT" part.
 * \note The command may or may not return a response.
 */
int32_t pi_wifi_at_cmd(struct pi_device *device, char *cmd, char *resp);

/**
 * \brief Connect to a peer.
 *
 * This function should be used to connect to a remote peer.
 *
 * \param device         Pointer to the WIFI device structure.
 * \param ssid           SSID in string format.
 * \param pwd            Password in string format.
 * \param callback       Task to notify when the module is connected to the AP.
 *
 * \retval 0             If operation is successful.
 * \retval ERRNO         Error code otherwise.
 */
int8_t pi_wifi_connect_to_ap(struct pi_device *device, const char *ssid, const char *pwd, pi_task_t *callback);

/**
 * \brief Close a connection to a peer.
 *
 * This function closes an existing peer connection.
 *
 * \param device         Pointer to the WIFI device structure.
 *
 * \retval 0             If operation is successful.
 * \retval ERRNO         Error code otherwise.
 */
int8_t pi_wifi_disconnect_from_ap(struct pi_device *device);

/**
 * \brief Send data to WIFI device. Blocking API.
 *
 * This function is used to send data to WIFI device.
 *
 * \param device         Pointer to the WIFI structure.
 * \param buffer         Buffer to send.
 * \param size           Size of data to send.
 *
 * \note This function is synchronous, caller is blocked until transfer is
 * finished. The pending asynchronous function is below : pi_wifi_data_send_async().
 */
void pi_wifi_data_send(struct pi_device *device, char *buffer, uint32_t size);

/**
 * \brief Send data to WIFI device. Blocking API.
 *
 * This function is used to send data to WIFI device.
 *
 * \param device         Pointer to the WIFI structure.
 * \param buffer         Buffer to send.
 * \param size           Size of data to send.
 * \param task           Event task used to check end of transfer.
 *
 * \note This function is asynchronous. The pending synchronous function is :
 * pi_wifi_data_send().
 */
void pi_wifi_data_send_async(struct pi_device *device, char *buffer,
                            uint32_t size, pi_task_t *task);

/**
 * \brief Get data from WIFI device. Blocking API.
 *
 * This function is used to retrieve data from WIFI device.
 *
 * \param device         Pointer to the WIFI structure.
 * \param buffer         Buffer to store data.
 * \param size           Size of data.
 *
 * \note This function is synchronous, caller is blocked until transfer is
 * finished. The pending asynchronous function is below : pi_wifi_data_get_async().
 */
void pi_wifi_data_get(struct pi_device *device, char *buffer, uint32_t size);

/**
 * \brief Get data from WIFI device. Non blocking API.
 *
 * This function is used to retrieve data from WIFI device.
 *
 * \param device         Pointer to the WIFI structure.
 * \param buffer         Buffer to store data.
 * \param size           Size of data.
 * \param task           Event task used to check end of transfer.
 *
 * \note This function is asynchronous. The pending synchronous function is :
 * pi_wifi_data_get().
 */
void pi_wifi_data_get_async(struct pi_device *device, char *buffer,
                           uint32_t size, pi_task_t *task);

/**
 * @brief Open a TCP server socket on the device
 * 
 * @param device Device to configure as a TCP server
 * @param port Local port number of the socket
 * 
 * @return int8_t Code for success/error operation
 * \retval 0 for success
 */
int8_t pi_wifi_create_tcp_server(struct pi_device *device, uint16_t port);

/**
 * @brief Wait for a client connection on the device (pre-configured as a TCP server)
 * 
 * @param device Device pre-configured as a TCP server
 * @param[out] resp Pointer for the response got from the client connection. Useful to know the IP and the port of the client.
 * @param[in] callback Task to notify when a client is connecting.
 * 
 * @return int8_t Code for success/error operation
 * \retval 0 for success
 */
int8_t pi_wifi_wait_client_connection(struct pi_device *device, char *resp, pi_task_t *callback);

/**
 * @brief Wait for a client's event
 * 
 * @param device Device pre-configured as a TCP server
 * @param[out] resp Buffer to save the message of the client
 * @param[in] callback Task to notify when an event appears
 * 
 * @return int8_t Code for success/error operation
 * \retval 0 for success
 */
int8_t pi_wifi_wait_client_event(struct pi_device *device, char *resp, pi_task_t *callback);

/**
 * @} addtogroup WIFI
 */

#endif  /* __PI_BSP_WIFI_H__ */
