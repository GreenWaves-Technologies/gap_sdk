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

/*
 * Created by Mathieu Barbe <mathieu.barbe@greenwaves-technologies.com>.
 * on 12/20/2019.
 */

#ifndef OTA_UTILITY_H
#define OTA_UTILITY_H

#include "stdint.h"
#include "pmsis.h"
#include "bsp/partition.h"
#include "bsp/flash_partition.h"

/// OTA_DATA states for checking operability of the app.
typedef enum {
    PI_OTA_IMG_NEW = 0x00U,         /*!< Monitor the first boot. In bootloader this state is changed to PI_OTA_IMG_PENDING_VERIFY. */
    PI_OTA_IMG_PENDING_VERIFY = 0x01U,         /*!< First boot for this app was. If while the second boot this state is then it will be changed to ABORTED. */
    PI_OTA_IMG_INVALID = 0x02U,         /*!< App was confirmed as non-workable. This app will not selected to boot at all. */
    PI_OTA_IMG_VALID = 0x03U,         /*!< App was confirmed as workable. This app will selected to boot at all. */
    PI_OTA_IMG_ABORTED = 0x04U,         /*!< App could not confirm the workable or non-workable. In bootloader IMG_PENDING_VERIFY state will be changed to IMG_ABORTED. This app will not selected to boot at all. */
    PI_OTA_IMG_BOOT_ONCE = 0x05U,         /*!< Boot to a specific application just once. In bootloader IMG_BOOT_ONCE state will be changed to IMG_VALID. */
    PI_OTA_IMG_UNDEFINED = 0xFFU,  /*!< Undefined. App can boot and work without limits. */
} ota_img_states_t;


typedef struct {
    uint8_t md5[16];
    uint32_t seq;
    uint8_t stable; // Subtype of stable app
    uint8_t previous_stable; // Subtype of previous stable app
    uint8_t once; // Partition to boot for the next reboot.
    uint8_t state;
} ota_state_t;

pi_err_t ota_utility_get_ota_state_from_partition_table(const pi_partition_table_t table, ota_state_t *ota_state);

void ota_utility_compute_md5(const ota_state_t *state, uint8_t *res);

bool ota_utility_state_is_valid(ota_state_t *state);

pi_err_t ota_utility_get_ota_state(pi_device_t *flash, const uint32_t partition_offset, ota_state_t *ota_state);

pi_err_t ota_utility_write_ota_data(const flash_partition_table_t *table, ota_state_t *ota_state);

void ota_utility_init_first_ota_state(ota_state_t *state);

#endif //OTA_UTILITY_H

