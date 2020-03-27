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

#ifndef __PI_BSP_BLE_H__
#define __PI_BSP_BLE_H__

#include "pmsis.h"

/**
 * @defgroup BLE BLE
 *
 * The BLE driver provides support for wireless data transfer between the host
 * and an external peripheral such as a smartphone, using Bluetooth connection.
 */

/**
 * @addtogroup BLE
 * @{
 */

/**
 * \struct pi_ble_api_t
 *
 * \brief BLE specific API
 *
 * Structure holding BLE specific API.
 */
typedef struct pi_ble_api_s
{
    int32_t (*at_cmd)(struct pi_device *device, const char *cmd,
                      char *resp, uint32_t size); /*!< Function to send AT command. */
    int32_t (*peer_connect)(struct pi_device *device, const char *addr); /*!< Function to connect BLE device to a remote peer. */
    int32_t (*peer_disconnect)(struct pi_device *device, const char *addr); /*!< Function to disconnect BLE device to a remote peer. */
} pi_ble_api_t;


/**
 * \brief Open a BLE device.
 *
 * This function opens and intializes a BLE device.
 * This function must be called before using device.
 *
 * \param device         Pointer to the BLE device structure.
 *
 * \retval 0             If the operation is successful.
 * \retval ERRNO         Error code otherwise.
 */
int32_t pi_ble_open(struct pi_device *device);

/**
 * \brief Close a BLE device.
 *
 * This function closes an opened BLE device. It frees all allocated objects and
 * interfaces used.
 *
 * \param device         Pointer to the BLE device structure.
 */
void pi_ble_close(struct pi_device *device);

/**
 * \brief IOctl commands.
 *
 * This function is used to send special command to BLE device.
 *
 * \param device         Pointer to the BLE device structure.
 * \param cmd            Ioctl command.
 * \param arg            Ioctl command arg.
 *
 * \return Value         Value depends on ioctl command.
 */
int32_t pi_ble_ioctl(struct pi_device *device, uint32_t cmd, void *arg);

/**
 * \brief Send AT command to BLE device.
 *
 * This function is used to send AT commands to BLE device.
 *
 * \param device         Pointer to the BLE device structure.
 * \param cmd            Command string to send.
 * \param resp           Buffer to store response.
 * \param size           Size of the response to store, in Bytes.
 *
 * \retval 0             If operation is successful.
 * \retval ERRNO         Error code otherwise.
 *
 * \note The command string should be plain command, i.e. without the "AT" part.
 * \note The command may or may not return a response.
 */
int32_t pi_ble_at_cmd(struct pi_device *device, const char *cmd, char *resp, uint32_t size);

/**
 * \brief Connect to a peer.
 *
 * This function should be used to connect to a remote peer.
 *
 * \param device         Pointer to the BLE device structure.
 * \param addr           Address in string format.
 *
 * \retval 0             If operation is successful.
 * \retval ERRNO         Error code otherwise.
 */
int32_t pi_ble_peer_connect(struct pi_device *device, const char *addr);

/**
 * \brief Close a connection to a peer.
 *
 * This function closes an existing peer connection.
 *
 * \param device         Pointer to the BLE device structure.
 * \param addr           Address in string format.
 *
 * \retval 0             If operation is successful.
 * \retval ERRNO         Error code otherwise.
 */
int32_t pi_ble_peer_disconnect(struct pi_device *device, const char *addr);

/**
 * \brief Send data to BLE device. Blocking API.
 *
 * This function is used to send data to BLE device.
 *
 * \param device         Pointer to the BLE structure.
 * \param buffer         Buffer to send.
 * \param size           Size of data to send.
 *
 * \note This function is synchronous, caller is blocked until transfer is
 * finished. The pending asynchronous function is below : pi_ble_data_send_async().
 */
void pi_ble_data_send(struct pi_device *device, uint8_t *buffer, uint32_t size);

/**
 * \brief Send data to BLE device. Blocking API.
 *
 * This function is used to send data to BLE device.
 *
 * \param device         Pointer to the BLE structure.
 * \param buffer         Buffer to send.
 * \param size           Size of data to send.
 * \param task           Event task used to check end of transfer.
 *
 * \note This function is asynchronous. The pending synchronous function is :
 * pi_ble_data_send().
 */
void pi_ble_data_send_async(struct pi_device *device, uint8_t *buffer,
                            uint32_t size, pi_task_t *task);

/**
 * \brief Get data from BLE device. Blocking API.
 *
 * This function is used to retrieve data from BLE device.
 *
 * \param device         Pointer to the BLE structure.
 * \param buffer         Buffer to store data.
 * \param size           Size of data.
 *
 * \note This function is synchronous, caller is blocked until transfer is
 * finished. The pending asynchronous function is below : pi_ble_data_get_async().
 */
void pi_ble_data_get(struct pi_device *device, uint8_t *buffer, uint32_t size);

/**
 * \brief Get data from BLE device. Non blocking API.
 *
 * This function is used to retrieve data from BLE device.
 *
 * \param device         Pointer to the BLE structure.
 * \param buffer         Buffer to store data.
 * \param size           Size of data.
 * \param task           Event task used to check end of transfer.
 *
 * \note This function is asynchronous. The pending synchronous function is :
 * pi_ble_data_get().
 */
void pi_ble_data_get_async(struct pi_device *device, uint8_t *buffer,
                           uint32_t size, pi_task_t *task);

/**
 * @} addtogroup BLE
 */

#endif  /* __PI_BSP_BLE_H__ */
